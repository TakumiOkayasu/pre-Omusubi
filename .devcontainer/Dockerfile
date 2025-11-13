FROM ubuntu:latest

ARG USER_NAME
ARG USER_UID
ARG USER_GID

ENV TZ=Asia/Tokyo \
    LANG=ja_JP.UTF-8 \
    LANGUAGE=ja_JP:jp \
    LC_ALL=ja_JP.UTF-8 \
    DEVCONTAINER=true \
    DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade -y && apt install -y --no-install-recommends \
    # Basic utilities
    bash \
    curl \
    git \
    sudo \
    ssh \
    unzip \
    locales \
    tzdata \
    ca-certificates \
    # C++ compiler and build tools
    build-essential \
    clang \
    cmake \
    ninja-build \
    # Debug tools
    gdb \
    valgrind \
    pkg-config \
    # Code quality tools
    clang-format \
    clang-tidy \
    clangd \
    bear \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && locale-gen ja_JP.UTF-8 \
    && localedef -f UTF-8 -i ja_JP ja_JP.UTF-8


WORKDIR /tmp/scripts/

# terminal config
RUN curl -o ./create_user.sh https://raw.githubusercontent.com/TakumiOkayasu/shellscript_utility/refs/heads/main/create_user.sh \
    && curl -o ./terminal_config.sh https://raw.githubusercontent.com/TakumiOkayasu/shellscript_utility/refs/heads/main/terminal_config.sh \
    && chmod 777 ./*.sh \
    && ./create_user.sh \
    && ./terminal_config.sh \
    && rm *.sh

WORKDIR /workspace
USER ${USER_NAME}
SHELL ["/bin/bash", "-c"]
