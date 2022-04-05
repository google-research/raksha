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
    grep \
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
ENV USE_BAZEL_VERSION=5.0.0
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.10.1/bazelisk-linux-amd64 \
    -O /usr/local/bin/bazelisk && chmod a+x /usr/local/bin/bazelisk

# Set up buildifier
RUN wget https://github.com/bazelbuild/buildtools/releases/download/4.2.5/buildifier-linux-amd64 \
    -O /usr/local/bin/buildifier && chmod a+x /usr/local/bin/buildifier
