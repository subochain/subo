package=libevent
$(package)_version=2.1.8
$(package)_download_path=https://github.com/libevent/libevent/releases/download/release-$($(package)_version)-stable
$(package)_file_name=$(package)-$($(package)_version)-stable.tar.gz
$(package)_sha256_hash=965cc5a8bb46ce4199a47e9b2c9e1cae3b137e8356ffdad6d94d3b9069b71dc2

define $(package)_preprocess_cmds
  ./autogen.sh
endef

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --disable-openssl --disable-libevent-regress
  $(package)_config_opts_release=--disable-debug-mode
  $(package)_config_opts_linux=--with-pic
endef

define $(package)_preprocess_cmds
  ./autogen.sh && \
  perl -i -0pe 's/arc4random_addrandom\(\(unsigned char\*\)buf,\s+n>\(size_t\)INT_MAX\s+\?\s+INT_MAX\s+:\s+\(int\)n\);/(void)buf; (void)n;/s' evutil_rand.c
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
endef
