#!/usr/bin/bash
#NOTE: Dá para paralelizar aqui
for file in "$@"; do
    realResult=$(clasp "$file" | grep -Eo 'SATISFIABLE|UNSATISFIABLE' | tail -n1)
    current=$(./build/main.o "$file")
    exitCode=$?
    currentResult=$(echo "$current" | grep -Eo 'SATISFIABLE|UNSATISFIABLE' | tail -n1)
    if [[ exitCode -eq 1 ]]; then
        printf "\033[1;31mError while running solver\033[0m\n" 1>&2
        break
    fi
 if [[ "$realResult" != "$currentResult" ]]; then
        printf "\033[1;31mMismatch on file %s\033[0m\n" "$file"
        echo "  clasp: $realResult"
        echo "  yours: $currentResult"
    else
        printf "\033[1;32mMatch on file %s: %s\033[0m\n" "$file" "$currentResult"
    fi
done

