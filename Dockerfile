# syntax=docker/dockerfile:1
ARG UBUNTU_TAG=22.04
FROM ubuntu:${UBUNTU_TAG}

# --- Configurable (can be overridden with --build-arg) ---
ARG CMAKE_VERSION=3.26.4
ARG CMAKE_BASE_URL=https://github.com/Kitware/CMake/releases/download
ARG GITHUB_TOKEN

ENV DEBIAN_FRONTEND=noninteractive

# --- Base system setup --------------------------------------------------------
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    curl \
    wget \
    tar \
    unzip \
    zip \
    pkg-config \
    sudo \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# Development tooling (optional)
RUN apt-get update && apt-get install -y \
    valgrind \
    gdb \
    perl \
    autoconf \
    automake \
    libtool \
    python3 \
    python3-venv \
    python3-pip \
 && rm -rf /var/lib/apt/lists/*

# --- Install CMake from official binaries (arch-aware) ------------------------
RUN set -eux; \
    ARCH="$(uname -m)"; \
    case "$ARCH" in \
      x86_64) CMAKE_ARCH=linux-x86_64 ;; \
      aarch64) CMAKE_ARCH=linux-aarch64 ;; \
      *) echo "Unsupported arch: $ARCH" >&2; exit 1 ;; \
    esac; \
    apt-get update && apt-get install -y wget tar && rm -rf /var/lib/apt/lists/*; \
    wget -q "${CMAKE_BASE_URL}/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-${CMAKE_ARCH}.tar.gz" -O /tmp/cmake.tgz; \
    tar --strip-components=1 -xzf /tmp/cmake.tgz -C /usr/local; \
    rm -f /tmp/cmake.tgz

# --- Create a non-root 'dev' user with passwordless sudo ----------------------
RUN useradd --create-home --shell /bin/bash dev && \
    echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers && \
    mkdir -p /workspace && chown dev:dev /workspace

USER dev
WORKDIR /workspace

# --- Optional Python venv for tools ------------------------------------------
RUN python3 -m venv /opt/venv && /opt/venv/bin/pip install --upgrade pip
ENV PATH="/opt/venv/bin:${PATH}"


# --- Build & install this project --------------------------------------------
COPY --chown=dev:dev . /workspace/the-macro-library
RUN mkdir -p /workspace/build/the-macro-library && \
    cd /workspace/build/the-macro-library && \
    cmake /workspace/the-macro-library && \
    make -j"$(nproc)" && sudo make install

CMD ["/bin/bash"]
