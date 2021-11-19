FROM ubuntu:focal
# Install the required packages
RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    bison \
    build-essential\
    clang \
    curl \
    doxygen \
    flex \
    g++ \
    git \
    libffi-dev \
    libncurses5-dev \
    libtool \
    libsqlite3-dev \
    make \
    mcpp \
    python \
    sqlite \
    uuid-runtime \
    wget \
    zlib1g-dev

# Java is needed for some targets
RUN apt-get install default-jdk -y --no-install-recommends
ENV JAVA_HOME /usr/lib/jvm/default-java

# Set up baselisk.
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.10.1/bazelisk-linux-amd64 \
    -O /usr/local/bin/bazelisk && chmod a+x /usr/local/bin/bazelisk

# Rust is needed for authorization logic
ARG rustup_dir=/usr/local/cargo
ENV RUSTUP_HOME ${rustup_dir}
ENV CARGO_HOME ${rustup_dir}
ENV PATH "${rustup_dir}/bin:${PATH}"
RUN curl --location https://sh.rustup.rs > /tmp/rustup \
  && sh /tmp/rustup -y --default-toolchain=none \
  && chmod a+rwx ${rustup_dir} \
  && rustup --version

## Set the right version of rust
ARG RUST_VERSION=nightly-2021-06-30
RUN rustup toolchain install ${RUST_VERSION} \
    && rustup default ${RUST_VERSION}

# Souffle is needed to run the tests in authorization logic
RUN curl -s https://packagecloud.io/install/repositories/souffle-lang/souffle/script.deb.sh | bash
RUN apt-get update && apt-get install -y souffle

## TODO / WIP delete these. Used for local debugging with docker
ADD rust rust
ADD gcb gcb
