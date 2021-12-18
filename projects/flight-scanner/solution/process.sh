compile() {
  gcc main.c -std=c99 -pedantic -Wall -lm $1 -o main  
}

# Always compile first.
compile

# Main commands
[[ $1 != "test" ]] && ./main $1 $2 # run program with arg