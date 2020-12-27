dir_file=$(dirname "$0")
dir_root=$dir_file/..
dir_out=$dir_root/build

entry_name="main.c"
entry_path=$dir_root/$entry_name
output_name="program"
output_path=$dir_out/$output_name

# Compile program.
function compile {
  eparams=()
  [[ $1 == 'debug' ]] && eparams+=(-ggdb3)
  gcc $entry_path -std=c99 -pedantic -o $output_path -Wall "${eparams[@]}"
}

function run {
  ./$output_path
} 

function testrun () {
  compile && run
}

function setup_memcheck() {
  docker build -t cmemorycheck:latest $dir_root/test/memcheck
}

function memcheck() {
  docker run -ti -v \
    $PWD:/workdir cmemorycheck:latest \
    bash -c "\
      cd /workdir/; \
      gcc ${entry_name} -std=c99 -pedantic \
        -o ${output_path} -Wall -ggdb3 && \
      valgrind --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=valgrind-out.txt \
        ./${output_path}"
}

function purgedata() {
  for filename in $dir_root/*.txt; do
    [ -e "$filename" ] || continue
    rm -f "$filename"
  done
}

function filldata() {
  purgedata
  for filename in $dir_root/test/preserved/*.txt; do
    [ -e "$filename" ] || continue
    cp "$filename" $dir_root
  done
}

function backupdata() {
  for filename in $dir_root/test/preserved/*.txt; do
    [ -e "$filename" ] || continue
    rm -f "$filename"
  done
  for filename in $dir_root/*.txt; do
    [ -e "$filename" ] || continue
    cp "$filename" $dir_root/test/preserved
  done
}

[[ $1 == "testrun" ]] && testrun
[[ $1 == "setup_memcheck" ]] && setup_memcheck
[[ $1 == "memcheck" ]] && memcheck
[[ $1 == "purgedata" ]] && purgedata
[[ $1 == "filldata" ]] && filldata
[[ $1 == "backupdata" ]] && backupdata
