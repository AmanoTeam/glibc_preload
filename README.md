# GLibc preload

An `execve()` wrapper that allows executing binaries using a glibc installed in a non-standard location.

## How does it work?

Sometimes you may want to run a binary using a different version of glibc instead of using the one installed on your machine, either to test a new implementation of a newer version or simply to use binaries that were linked on a system with a newer glibc than yours.

The `libglibc_preload.so` library uses the [LD_PRELOAD](https://stackoverflow.com/a/426260) trick to replace calls to the `execve()` function with an alternative version that replaces the default interpreter with one installed in a non-standard location.

## Install

### Build glibc and install on a non-standard location.

```bash
$ git clone --depth='1' 'https://sourceware.org/git/glibc.git'
$ cd glibc
$ mkdir build; cd build
$ ../configure \
    --prefix="${HOME}/.local" \
    --with-headers=/usr/include \
    --enable-bind-now \
    --enable-cet \
    --enable-multi-arch \
    --enable-stack-protector=strong \
    --enable-systemtap \
    --disable-crypt \
    --disable-profile \
    --disable-werror \
    CFLAGS='-Os' \
    LDFLAGS='-s'
$ make all
$ make install
```

### Build glibc_preload and install

```bash
$ git clone --depth='1' 'https://github.com/AmanoTeam/glibc_preload.git'
$ cd glibc_preload
$ mkdir build; cd build
$ cmake \
    -DCMAKE_INSTALL_PREFIX="${HOME}/.local" \
    -DCMAKE_BUILD_TYPE='MinSizeRel' \
    ./
$ cmake --install ./
```

## Usage

```bash
LD_PRELOAD="${HOME}/.local/lib/libglibc_preload.so" command ...
```

## Limitations

* This won't work with statically linked binaries.
* This won't work with binaries that call `execve()` using syscalls instead of relying on the glibc implementation.