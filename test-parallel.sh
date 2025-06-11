#!/usr/bin/bash
# shellcheck disable=SC2016
printf '%s\n' "$@" | xargs -P "$(nproc)" -I {} bash -c '
    file="$1"
    claspResult=$(clasp "$file" | grep -Eo "SATISFIABLE|UNSATISFIABLE" | tail -n1)
    myOutput=$(./build/main.o "$file")
    exitCode=$?
    if [[ $exitCode -eq 1 ]]; then
        printf "\033[1;31mError while running solver on %s\033[0m\n" "$file" >&2
        exit 1
    fi
    myResult=$(echo "$myOutput" | grep -Eo "SATISFIABLE|UNSATISFIABLE" | tail -n1)
    if [[ "$claspResult" != "$myResult" ]]; then
        printf "\033[1;31mMismatch on file %s\033[0m\n" "$file"
        echo "  clasp: $claspResult"
        echo "  yours: $myResult"
    else
        printf "\033[1;32mMatch on file %s: %s\033[0m\n" "$file" "$myResult"
    fi
' _ {}
