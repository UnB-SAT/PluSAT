# !bin/bash
FILES=($(ls data/uf20-91/*.cnf))

for file in ${FILES[@]}
do
    echo $file

    ./build/main.o $file

    status=$?

    if [[$status -eq 1]]; then
        break
    fi
done

