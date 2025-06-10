#!/usr/bin/bash
# shellcheck disable=SC2016
printf '%s\n' "$@" | xargs -P "$(nproc)" -I {} bash -c '
    if ./build/main.o "$1"; then
        printf "\033[1;31mError while running solver on %s\033[0m\n" "$1" >&2
        exit 1
    fi
' _ {}
