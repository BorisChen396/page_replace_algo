#!/bin/bash

project_root="$(dirname "$(dirname "$(realpath "${BASH_SOURCE[0]}")")")"
input_dir="$(realpath "${project_root}/input")"
output_dir="$(realpath "${project_root}/output")"

proc_counter=0
# for algo_name in 'double_fifo'; do
for algo_name in 'fifo' 'mfu' 'esc' 'double_fifo'; do
    algo_output_dir="${output_dir}/${algo_name}"
    mkdir -p "${algo_output_dir}"
    while IFS= read -r -d '' file; do
        if ((proc_counter >= $(nproc))); then
            wait -n
            ((proc_counter--))
        fi
        ((proc_counter++))
        {
            echo -e "Execute:\talgo_name = '${algo_name}', file = '${file}'"
            output_file_path="${algo_output_dir}/$(basename $file)"
            printf 'Frame size\tDisk writes\tPage faults\n' > "${output_file_path}"
            for i in {1..100}; do
                printf $i'\t' >> "${output_file_path}"
                "${project_root}/build/benchmark/benchmark_algo_${algo_name}" $i "${file}" >> "${output_file_path}"
                if [ $? != 0 ]; then
                    echo -e "Error:\talgo_name = '${algo_name}', file = '${file}', i = ${i}"
                fi
            done
            echo -e "Complete:\talgo_name = '${algo_name}', file = '${file}'"
        } &
    done < <(find "${input_dir}" -type f -name "*.txt" -print0)
done
wait
