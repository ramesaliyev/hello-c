files=(
  "./test/1.txt"
  "./test/2.txt"
  "./test/3.txt"
  "./test/4.txt"
  "./test/5.txt"
  "./test/6.txt"
  "./test/7.txt"
  "./test/8.txt"
)

outputs=(
  "8 | 1 3 5"
  "16 | 5 4 3"
  "250 | 2 4"
  "2101 | 38 1 15 5 43 34 3 21 26 24 11 39 35 18 25 7"
  "1825 | 14 50 31 47 1 41 12 29 3 44 7 48 28"
  "1868 | 35 24 16 4 14 2 23 12 38 27 26 50 5 22"
  "1470 | 11 24 6 18 25 12 29 7 22 4 26 1"
  "5487 | 217 133 224 135 461 314 496 259 451 214 106 168 159 42 268 73 18 435 103 115 63 350 395 143 65 187 58 279 176 411 99 480 94 252 27 383 109 299 245 50 60"
)

compile() {
  gcc main.c -std=c99 -pedantic -Wall -lm $1 -o main  
}

test() {
  compile "-D TEST"

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
compile

# Main commands
[[ $1 = "test" ]] && test # run tests
[[ $1 != "test" ]] && ./main $1 $2 # run program with arg