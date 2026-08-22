package=native_cdrkit
$(package)_version=1.1.11
$(package)_download_path=http://distro.ibiblio.org/fatdog/source/600/c
$(package)_file_name=cdrkit-$($(package)_version).tar.bz2
$(package)_sha256_hash=b50d64c214a65b1a79afe3a964c691931a4233e2ba605d793eb85d0ac3652564
$(package)_patches=cdrkit-deterministic.patch

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/cdrkit-deterministic.patch && \
  sed -i.old "1i cmake_minimum_required(VERSION 2.6)" CMakeLists.txt && \
  sed -i.old 's/MESSAGE(FATAL_ERROR "Error: found a Linux system but no libcap header. Install libcap-dev.")/MESSAGE(STATUS "libcap not found - wodim binary will be link-broken, but nothing in this depends package builds it")/' wodim/CMakeLists.txt
endef

# genisoimage links against wodimstuff, a small static helper library built
# by wodim/CMakeLists.txt (NOT the wodim/cdrecord binary itself) - so that
# subdirectory can't just be dropped from SUBDIRS() wholesale, it has to
# stay. Its CMakeLists.txt does hard-fail configure on Linux hosts without
# libcap-dev installed, but that check only guards the actual `wodim`
# executable target (which links -lcap); wodimstuff and everything
# genisoimage's build actually touches (`make genisoimage`, below) don't
# need it, so the FATAL_ERROR is downgraded to a warning above rather than
# installing libcap-dev (which needs root, unavailable here) or patching
# out the wodim executable target entirely.
#
# cdrkit's top-level CMakeLists.txt has no cmake_minimum_required() call at
# all (not just an old one) - modern CMake (3.31+) hard-fails on that with
# no override, unlike the "version too old" case CMAKE_POLICY_VERSION_MINIMUM
# covers, so one is injected above. CMAKE_POLICY_VERSION_MINIMUM below then
# satisfies the compatibility check that newly-added call triggers.
#
# genisoimage.c/jte.c also call several of their own helper functions
# (parse_checksum_algo, calculate_md5sum, mk_MD5Parse, ...) with no
# prototype in scope - always relied on old-style implicit function
# declarations. GCC 14+ made -Wimplicit-function-declaration a hard error
# by default (matching C23 dropping the construct). Separately, this
# 2007-era code also declares globals like `outfile` in shared headers
# without `extern`, relying on old "common symbol" linking to merge the
# resulting tentative definitions across translation units; GCC 10+
# defaults to -fno-common, which turns that into genuine duplicate-symbol
# link errors. Both are demoted/restored via CMAKE_C_FLAGS (this package's
# _config_cmds calls cmake directly rather than through the autoconf
# CFLAGS plumbing, so the flags have to go in explicitly here).
define $(package)_config_cmds
  cmake -DCMAKE_INSTALL_PREFIX=$(build_prefix) -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_FLAGS="-Wno-error=implicit-function-declaration -fcommon"
endef

define $(package)_build_cmds
  $(MAKE) genisoimage
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) -C genisoimage install
endef

define $(package)_postprocess_cmds
  rm bin/isovfy bin/isoinfo bin/isodump bin/isodebug bin/devdump
endef
