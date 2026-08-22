package=native_libdmg-hfsplus
$(package)_version=0.1
$(package)_download_path=https://github.com/theuni/libdmg-hfsplus/archive
$(package)_file_name=libdmg-hfsplus-v$($(package)_version).tar.gz
$(package)_sha256_hash=6569a02eb31c2827080d7d59001869ea14484c281efab0ae7f2b86af5c3120b3
$(package)_build_subdir=build

define $(package)_preprocess_cmds
  mkdir build
endef

# adc.c's adc_decompress() is called from dmgfile.c/io.c with no prototype
# anywhere (no adc.h) - always relied on old-style implicit function
# declarations. GCC 14+ made -Wimplicit-function-declaration a hard error
# by default (matching C23 dropping the construct from the language), so
# demote it back to a warning rather than patching 2015-era source that
# only this native build tool uses. This package's _config_cmds calls
# cmake directly rather than through the autoconf CFLAGS plumbing (which
# only $(package)_cflags feeds), so the flag has to be passed explicitly
# via CMAKE_C_FLAGS instead.
define $(package)_config_cmds
  cmake -DCMAKE_INSTALL_PREFIX:PATH=$(build_prefix)/bin -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_FLAGS=-Wno-error=implicit-function-declaration ..
endef

define $(package)_build_cmds
  $(MAKE) -C dmg
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) -C dmg install
endef
