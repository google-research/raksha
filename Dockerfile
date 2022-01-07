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
ENV PATH "${PATH}:${rustup_dir}/bin"
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

### Prefetch Cargo Depdendencies
# These docker command leverage cargo-chef to prefetch rust dependencies
# as described
# [here](https://www.lpalmieri.com/posts/fast-rust-docker-builds/)

# Install chef.
# It might also be possible to leverage a docker image
# that already has chef with the commented out ocmmand
# FROM lukemathwalker/cargo-chef:latest-rust-1.53.0 as chef
RUN cargo install cargo-chef

# Copy just the auth logic files (so we can build the deps)
ARG AUTH_LOGIC_DIR=rust/tools/authorization-logic
COPY ./${AUTH_LOGIC_DIR} /chef_tmp/${AUTH_LOGIC_DIR}

# Use chef to build the recipe of dependencies
WORKDIR /chef_tmp/${AUTH_LOGIC_DIR}
RUN cargo chef prepare --recipe-path recipe.json

# Build just the dependencies for the project
RUN cargo chef cook --recipe-path recipe.json
