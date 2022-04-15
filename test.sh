# !bin/bash
FILES=($(ls ../uf20-91/*.cnf))

for file in ${FILES[@]}
do
    echo $file

    ./build/main.o $file
done

