FROM debian:bookworm

ARG BINUTILS_VERSION=2.42
ARG GCC_VERSION=13.2.0

ENV PREFIX=/opt/cross
ENV TARGET=i686-elf
ENV PATH=${PREFIX}/bin:${PATH}
ENV TMPDIR=/var/tmp

RUN mkdir -p /tmp /var/tmp && chmod 1777 /tmp /var/tmp

RUN apt-get update && apt-get install -y \
    build-essential \
    bison \
    flex \
    libgmp-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    curl \
    xz-utils \
    ca-certificates \
    make \
    python3 \
    grub-common \
    grub-pc-bin \
    xorriso \
    mtools \
    qemu-system-x86 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

RUN curl -LO https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz && \
    curl -LO https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz && \
    tar -xf binutils-${BINUTILS_VERSION}.tar.xz && \
    tar -xf gcc-${GCC_VERSION}.tar.xz

RUN mkdir build-binutils && cd build-binutils && \
    ../binutils-${BINUTILS_VERSION}/configure \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --with-sysroot \
        --disable-nls \
        --disable-werror && \
    make -j"$(nproc)" && \
    make install

RUN mkdir build-gcc && cd build-gcc && \
    ../gcc-${GCC_VERSION}/configure \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --enable-languages=c \
        --without-headers && \
    make -j"$(nproc)" all-gcc all-target-libgcc && \
    make install-gcc install-target-libgcc

WORKDIR /workspace
CMD ["/bin/bash"]
