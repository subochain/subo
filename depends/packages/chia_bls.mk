package=chia_bls
$(package)_version=v20181101
# It's actually from https://github.com/Chia-Network/bls-signatures, but we have so many patches atm that it's forked
$(package)_download_path=https://github.com/codablock/bls-signatures/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=b3ec74a77a7b6795f84b05e051a0824ef8d9e05b04b2993f01040f35689aa87c
$(package)_dependencies=gmp
#$(package)_patches=...TODO (when we switch back to https://github.com/Chia-Network/bls-signatures)

define $(package)_preprocess_cmds
  sed -i '1i include_directories($(host_prefix)/include)' contrib/relic/src/CMakeLists.txt && \
  sed -i 's|set(CMAKE_C_FLAGS "-pipe -std=c99|set(CMAKE_C_FLAGS "-pipe -std=c99 -I$(host_prefix)/include|' contrib/relic/CMakeLists.txt && \
  sed -i '/#pragma pack(push, 1)/d; /#pragma pack(pop)/d' contrib/relic/src/md/blake2.h && \
  sed -i 's|#define ALIGNME(x)  __attribute__((aligned(x)))|#define ALIGNME(x)|' contrib/relic/src/md/blake2.h && \
  sed -i 's/constexpr static std::size_t sigStackSize = 32768 >= MINSIGSTKSZ ? 32768 : MINSIGSTKSZ;/constexpr static std::size_t sigStackSize = 32768;/' contrib/catch/catch.hpp
endef

define $(package)_set_vars
  $(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_dir)/$(host_prefix)
  $(package)_config_opts+= -DCMAKE_PREFIX_PATH=$(host_prefix)
  $(package)_config_opts+= -DSTLIB=ON -DSHLIB=OFF -DSTBIN=ON
  $(package)_config_opts+= -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  $(package)_config_opts_linux=-DOPSYS=LINUX -DCMAKE_SYSTEM_NAME=Linux
  $(package)_config_opts_darwin=-DOPSYS=MACOSX -DCMAKE_SYSTEM_NAME=Darwin
  $(package)_config_opts_mingw32=-DOPSYS=WINDOWS -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SHARED_LIBRARY_LINK_C_FLAGS="" -DCMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS=""
  $(package)_config_opts_i686+= -DWSIZE=32
  $(package)_config_opts_x86_64+= -DWSIZE=64
  $(package)_config_opts_arm+= -DWSIZE=32
  $(package)_config_opts_armv7l+= -DWSIZE=32
  $(package)_config_opts_debug=-DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug

  ifneq ($(darwin_native_toolchain),)
    $(package)_config_opts_darwin+= -DCMAKE_AR="$(host_prefix)/native/bin/$($(package)_ar)"
    $(package)_config_opts_darwin+= -DCMAKE_RANLIB="$(host_prefix)/native/bin/$($(package)_ranlib)"
  endif
endef

define $(package)_config_cmds
  export CC="$($(package)_cc)" && \
  export CXX="$($(package)_cxx)" && \
  export CFLAGS="$($(package)_cflags) $($(package)_cppflags)" && \
  export CXXFLAGS="$($(package)_cxxflags) $($(package)_cppflags)" && \
  export LDFLAGS="$($(package)_ldflags)" && \
  mkdir -p build && cd build && \
  cmake ../ $($(package)_config_opts)
endef

define $(package)_build_cmds
  cd build && \
  $(MAKE) $($(package)_build_opts)
endef

define $(package)_stage_cmds
  cd build && \
  $(MAKE) install
endef
