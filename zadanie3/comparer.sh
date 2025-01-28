#!/bin/bash

files=(./2025*.txt)

for ((i = 0; i < ${#files[@]}; i++)); do
  for ((j = i + 1; j < ${#files[@]}; j++)); do
    file1="${files[i]}"
    file2="${files[j]}"
    diff_output=$(diff "$file1" "$file2")
    if [ -n "$diff_output" ]; then
      echo "Differences found between $file1 and $file2:"
      echo "$diff_output"
      echo "----------------------------------------"
    else
      echo "No differences between $file1 and $file2"
    fi
  done
done