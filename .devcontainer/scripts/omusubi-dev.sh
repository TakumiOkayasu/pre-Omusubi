#!/bin/bash
# Omusubi開発環境の設定スクリプト
# /etc/profile.d/から自動的に読み込まれる

# PATHに~/.local/binを追加（PlatformIO用）
export PATH="${HOME}/.local/bin:${PATH}"

# PlatformIO設定
export PLATFORMIO_CORE_DIR="${HOME}/.platformio"

# Git補完機能とカスタムプロンプト
if [ -f /usr/share/bash-completion/completions/git-completion.bash ]; then
    source /usr/share/bash-completion/completions/git-completion.bash
fi
if [ -f /usr/share/bash-completion/completions/git-prompt.sh ]; then
    source /usr/share/bash-completion/completions/git-prompt.sh
    GIT_PS1_SHOWDIRTYSTATE=true
    GIT_PS1_SHOWUNTRACKEDFILES=true
    GIT_PS1_SHOWUPSTREAM=auto
fi
