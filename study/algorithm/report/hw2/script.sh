#!/usr/bin/env bash

algos=(
  "Insertion Sort"
  "Merge Sort"
  "Quick Sort (Random Pivot)"
  "Quick Sort (Last Pivot)"
  "Heap Sort"
  "Radix Sort"
)

ns=(100 1000 10000 100000 1000000 10000000 100000000)

: > hw2_sort.log

for algo_id in 0 1 2 3 4 5; do
  echo "===== ${algos[$algo_id]} =====" | tee -a hw2_sort.log

  for n in "${ns[@]}"; do
    for seed in 0 1 2 3 4; do
      echo "Running algo=$algo_id, n=$n, seed=$seed ..." | tee -a hw2_sort.log

      output=$(timeout 180s ./hw2_sort.o "$algo_id" "$n" "$seed")
      status=$?

      if [ $status -eq 0 ]; then
        echo "$output" | tee -a hw2_sort.log
      elif [ $status -eq 124 ]; then
        echo "${algos[$algo_id]} | n=$n | seed=$seed | TIMEOUT (>180s) -> skipped" | tee -a hw2_sort.log
      else
        echo "${algos[$algo_id]} | n=$n | seed=$seed | ERROR (status=$status)" | tee -a hw2_sort.log
      fi
    done
    echo "----------------------------------------" | tee -a hw2_sort.log
  done
done