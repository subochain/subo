package=native_cctools
$(package)_version=807d6fd1be5d2224872e381870c0a75387fe05e6
$(package)_download_path=https://github.com/theuni/cctools-port/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=a09c9ba4684670a0375e42d9d67e7f12c1f62581a27f28f7c825d6d7032ccc6a
$(package)_build_subdir=cctools
$(package)_clang_version=3.7.1
# llvm.org pruned this old release tarball from releases.llvm.org at some
# point; Bitcoin Core mirrors the exact same file (this whole native_cctools
# package, clang pin included, traces back to their depends system) at the
# same hash, so use that instead.
$(package)_clang_download_path=https://bitcoincore.org/depends-sources
# bitcoincore.org's mirror renamed the "+" in llvm.org's original
# "clang+llvm-..." filename to "-" - match that for the remote fetch, the
# local cache name below already used the hyphenated form.
$(package)_clang_download_file=clang-llvm-$($(package)_clang_version)-x86_64-linux-gnu-ubuntu-14.04.tar.xz
$(package)_clang_file_name=clang-llvm-$($(package)_clang_version)-x86_64-linux-gnu-ubuntu-14.04.tar.xz
$(package)_clang_sha256_hash=99b28a6b48e793705228a390471991386daa33a9717cd9ca007fcdde69608fd9
$(package)_extra_sources=$($(package)_clang_file_name)
$(package)_patches=linux_sys_sysctl_stub.patch

define $(package)_fetch_cmds
$(call fetch_file,$(package),$($(package)_download_path),$($(package)_download_file),$($(package)_file_name),$($(package)_sha256_hash)) && \
$(call fetch_file,$(package),$($(package)_clang_download_path),$($(package)_clang_download_file),$($(package)_clang_file_name),$($(package)_clang_sha256_hash))
endef

define $(package)_extract_cmds
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(package)_sha256_hash)  $($(package)_source)" > $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  echo "$($(package)_clang_sha256_hash)  $($(package)_source_dir)/$($(package)_clang_file_name)" >> $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  mkdir -p toolchain/bin toolchain/lib/clang/3.5/include && \
  tar --strip-components=1 -C toolchain -xf $($(package)_source_dir)/$($(package)_clang_file_name) && \
  rm -f toolchain/lib/libc++abi.so* && \
  echo "#!/bin/sh" > toolchain/bin/$(host)-dsymutil && \
  echo "exit 0" >> toolchain/bin/$(host)-dsymutil && \
  chmod +x toolchain/bin/$(host)-dsymutil && \
  tar --strip-components=1 -xf $($(package)_source)
endef

# The bundled clang 3.7.1 (2015) only recognizes GCC version directories that
# existed back then, so on a host with a newer system GCC (e.g. GCC 15 under
# /usr/lib/gcc/x86_64-linux-gnu/15) it can't find crtbegin.o/libgcc to link
# when building cctools-port itself - it isn't cross-compiling here yet,
# this is cctools-port's own build tool running natively on Linux. Point it
# at the real directory directly (looked up fresh via gcc's own
# -print-file-name, not hardcoded, since the exact GCC version and
# multiarch triple vary by host) instead of relying on clang's own outdated
# auto-detection. This is still needed even with -stdlib=libc++ below,
# since crt startup objects and libgcc_s (stack unwinding) are separate
# from the C++ standard library.
$(package)_gcc_crtdir:=$(dir $(shell gcc -print-file-name=crtbegin.o 2>/dev/null))

# Earlier attempt pointed clang at the *system* GCC's C++ headers
# (-cxx-isystem, via g++ -E -v's search-list) so it could find <vector>,
# <map>, etc. That solves "header not found" but not the deeper problem:
# GCC 15's libstdc++ headers themselves use C++17 constructs (if constexpr,
# etc.) that a 2015-era clang 3.7.1 frontend cannot parse at all, no matter
# where they're found. Fix used instead: use the clang toolchain's OWN
# bundled libc++ (toolchain/include/c++/v1, same vintage as the compiler,
# so no syntax mismatch) via -stdlib=libc++.
#
# Two gaps that leaves:
#  1. libc++'s <locale> chain still #includes <xlocale.h>, a legacy BSD
#     header modern glibc (2.26+) folded into locale.h and dropped
#     outright (same story as the sys/sysctl.h patch below) - shimmed by
#     compat-libs/compat-include/xlocale.h, a one-line passthrough.
#  2. native_cctools's own extract_cmds deliberately deletes the bundled
#     lib/libc++abi.so* (it's meant to stay unused when the toolchain is
#     driving actual macOS cross-compiles, which link against the target
#     SDK's own libc++abi instead). Since we're now using this libc++ to
#     build cctools-port itself as a *native* Linux tool, it needs a real
#     libc++abi to link and run against. Only libc++abi.a (static) ships,
#     so _preprocess_cmds below relinks it into a libc++abi.so.1 using the
#     host's own g++ (this just repackages the existing object code into a
#     shared object - no recompilation, so no ABI/version concerns).
$(package)_compat_include_dir:=$(CURDIR)/compat-libs/compat-include

define $(package)_set_vars
$(package)_config_opts=--target=$(host) --disable-lto-support
$(package)_ldflags+=-Wl,-rpath=\\$$$$$$$$\$$$$$$$$ORIGIN/../lib
$(package)_ldflags+=-L$($(package)_extract_dir)/toolchain/lib -lc++abi
$(package)_cc=$($(package)_extract_dir)/toolchain/bin/clang
$(package)_cxx=$($(package)_extract_dir)/toolchain/bin/clang++
$(package)_cflags+=-B$($(package)_gcc_crtdir) -L$($(package)_gcc_crtdir) -isystem $($(package)_gcc_crtdir)include
$(package)_cxxflags+=-B$($(package)_gcc_crtdir) -L$($(package)_gcc_crtdir) -isystem $($(package)_gcc_crtdir)include
$(package)_cxxflags+=-stdlib=libc++ -isystem $($(package)_compat_include_dir)
endef

define $(package)_preprocess_cmds
  patch -p1 -d $($(package)_build_subdir) < $($(package)_patch_dir)/linux_sys_sysctl_stub.patch && \
  cd toolchain/lib && \
  $(build_CXX) -shared -o libc++abi.so.1 -Wl,--whole-archive libc++abi.a -Wl,--no-whole-archive -lpthread -ldl -lc && \
  ln -sf libc++abi.so.1 libc++abi.so && \
  cd ../.. && \
  cd $($(package)_build_subdir); ./autogen.sh && \
  sed -i.old "/define HAVE_PTHREADS/d" ld64/src/ld/InputFiles.h
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install && \
  cd $($(package)_extract_dir)/toolchain && \
  mkdir -p $($(package)_staging_prefix_dir)/lib/clang/$($(package)_clang_version)/include && \
  mkdir -p $($(package)_staging_prefix_dir)/bin $($(package)_staging_prefix_dir)/include && \
  cp bin/clang $($(package)_staging_prefix_dir)/bin/ &&\
  cp -P bin/clang++ $($(package)_staging_prefix_dir)/bin/ &&\
  cp lib/libLTO.so $($(package)_staging_prefix_dir)/lib/ && \
  cp -rf lib/clang/$($(package)_clang_version)/include/* $($(package)_staging_prefix_dir)/lib/clang/$($(package)_clang_version)/include/ && \
  cp bin/llvm-dsymutil $($(package)_staging_prefix_dir)/bin/$(host)-dsymutil && \
  if `test -d include/c++/`; then cp -rf include/c++/ $($(package)_staging_prefix_dir)/include/; fi && \
  if `test -d lib/c++/`; then cp -rf lib/c++/ $($(package)_staging_prefix_dir)/lib/; fi && \
  cp -P lib/libc++.so lib/libc++.so.1 lib/libc++.so.1.0 lib/libc++abi.so lib/libc++abi.so.1 $($(package)_staging_prefix_dir)/lib/ && \
  cp -L $(CURDIR)/compat-libs/libtinfo.so.5 $($(package)_staging_prefix_dir)/lib/libtinfo.so.5
endef

# The prebuilt clang/clang++ binaries need libtinfo.so.5 (see the
# libtinfo5 shim note at the top of the top-level Makefile) - their own
# baked-in RPATH is $ORIGIN/../lib, so dropping a copy directly into this
# package's staged lib/ (next to the bin/ they're copied into just above)
# lets them find it on their own. That covers every invocation of this
# staged clang/clang++ - not just ones made from within a `make` run
# inside depends/ itself, where LD_LIBRARY_PATH happens to be exported -
# in particular the real project's own top-level ./configure and
# ./make, invoked directly against this depends prefix's config.site.
