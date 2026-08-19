linux_CFLAGS=-pipe -std=gnu17 -fcommon \
  -Wno-error=implicit-function-declaration \
  -Wno-error=implicit-int \
  -Wno-error=int-conversion \
  -Wno-error=incompatible-pointer-types \
  -Wno-error=return-mismatch \
  -Wno-error=discarded-qualifiers \
  -Wno-error=attributes \
  -Wno-error=format-overflow \
  -Wno-error=stringop-overflow \
  -Wno-error=stringop-truncation \
  -Wno-error=array-bounds \
  -Wno-error=maybe-uninitialized \
  -Wno-error=use-after-free

linux_CXXFLAGS=-pipe -std=c++17 -fcommon \
  -Wno-error=implicit-function-declaration \
  -Wno-error=implicit-int \
  -Wno-error=int-conversion \
  -Wno-error=incompatible-pointer-types \
  -Wno-error=return-mismatch \
  -Wno-error=discarded-qualifiers \
  -Wno-error=attributes \
  -Wno-error=format-overflow \
  -Wno-error=stringop-overflow \
  -Wno-error=stringop-truncation \
  -Wno-error=array-bounds \
  -Wno-error=maybe-uninitialized \
  -Wno-error=use-after-free \
  -static-libstdc++ \
  -fpermissive

linux_release_CFLAGS=-O2
linux_release_CXXFLAGS=$(linux_release_CFLAGS)

linux_debug_CFLAGS=-O1
linux_debug_CXXFLAGS=$(linux_debug_CFLAGS)

linux_debug_CPPFLAGS=-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

ifeq (86,$(findstring 86,$(build_arch)))
i686_linux_CC=gcc -m32
i686_linux_CXX=g++ -m32
i686_linux_AR=ar
i686_linux_RANLIB=ranlib
i686_linux_NM=nm
i686_linux_STRIP=strip

x86_64_linux_CC=gcc -m64
x86_64_linux_CXX=g++ -m64
x86_64_linux_AR=ar
x86_64_linux_RANLIB=ranlib
x86_64_linux_NM=nm
x86_64_linux_STRIP=strip
else
i686_linux_CC=$(default_host_CC) -m32
i686_linux_CXX=$(default_host_CXX) -m32
x86_64_linux_CC=$(default_host_CC) -m64
x86_64_linux_CXX=$(default_host_CXX) -m64
endif
