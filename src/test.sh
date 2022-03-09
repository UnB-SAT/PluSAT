# !bin/bash
FILES=($(ls ../data/*.cnf))

for file in ${FILES[@]}
do
    echo $file

    ./build/main.o $file
done

