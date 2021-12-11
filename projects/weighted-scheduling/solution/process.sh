files=(
  "./test/1.txt"
  "./test/2.txt"
)

outputs=(
  "8 | 1 3 5"
  "16 | 1 4 6"
)

test() {
  for i in ${!files[@]}; do
    output=$(./main ${files[$i]})
    
    if [[ $output = ${outputs[$i]} ]]
    then
      echo "($i) OK [${files[$i]}]"
    else
      echo "($i) ERR {${files[$i]}} expected: [${outputs[$i]}] got: [$output]"
    fi
  done
}

# Always compile first.
gcc main.c -std=c99 -pedantic -Wall -lm -o main

# Main commands
[[ $1 = "test" ]] && test # run tests
[[ $1 != "test" ]] && ./main $1 # run program with arg