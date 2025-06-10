#!/usr/bin/bash
#NOTE: Dá para paralelizar aqui
for file in "$@"; do
    if ./build/main.o "$file"; then
        printf "\033[1;31mError while running solver\033[0m\n" 1>&2
        break
    fi
done

