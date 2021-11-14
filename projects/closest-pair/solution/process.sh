# Compile program
gcc main.c -std=c99 -pedantic -Wall -lm -o main

# Run program
for f in ./samples/*.txt
do
  ./main $f
  printf "\n"
done