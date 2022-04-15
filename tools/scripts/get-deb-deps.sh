#!/bin/bash
# Very quick and dirty script to dump a very rough list of required packages
elf_file=$1
readarray -t libs <<< $(ldd ${elf_file} | grep -oP "/lib.*.so.* ")

LF=$'\n'
for lib in "${libs[@]}"; do
  dep=$(dpkg-query -S $(readlink ${lib}) 2>/dev/null | grep -oP '^\w+')
  deps="${deps}${dep}${LF}"
done

echo "${deps}" | sort | uniq -u
