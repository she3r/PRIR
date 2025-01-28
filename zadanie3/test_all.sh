#!/bin/bash

currdate="$(date +'%Y-%m-%d_%H-%M-%S')"


script_to_add="#!/bin/bash

 files=(./2025*.txt)

 for ((i = 0; i < \${#files[@]}; i++)); do
   for ((j = i + 1; j < \${#files[@]}; j++)); do
     file1=\"\${files[i]}\"
     file2=\"\${files[j]}\"
     diff_output=\$(diff \"\$file1\" \"\$file2\")
     if [ -n \"\$diff_output\" ]; then
       echo \"Differences found between \$file1 and \$file2:\"
       echo \"\$diff_output\"
       echo \"----------------------------------------\"
     else
       echo \"No differences between \$file1 and \$file2\"
     fi
   done
 done"

mkdir "$currdate"
cd "$currdate" || exit

echo "$script_to_add" >> "compare.sh"

parallel_exec_name="par.out"
seq_exec_name="seq.out"
# compile program in seq and parallel version
c++ -O2 -fopenmp ../*cpp -o "$parallel_exec_name"
c++ -O2 ../*cpp  -o "$seq_exec_name"

output_file="test_all.txt"

#clear if exists
> "$output_file"

echo "sequential:" >> "$output_file"
./"$seq_exec_name" >> "$output_file"
echo "" >> "$output_file"
for i in {1..10}
do
	export OMP_NUM_THREADS="$i"
	echo "number of threads: $i" >> "$output_file"
	./"$parallel_exec_name" >> "$output_file"
	echo "" >> "$output_file"
done
echo "output saved to $output_file"
