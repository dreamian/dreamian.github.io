#!/usr/bin/env bash

set -u

commands=(
'./brute_force_100.o > brute_force_100.log'
'./brute_force_opt_100.o > brute_force_opt_100.log'
'./divide_and_conquer_100.o > divide_and_conquer_100.log'
'./incremental_approach_100.o > incremental_approach_100.log'
'./brute_force_1000.o > brute_force_1000.log'
'./brute_force_opt_1000.o > brute_force_opt_1000.log'
'./divide_and_conquer_1000.o > divide_and_conquer_1000.log'
'./incremental_approach_1000.o > incremental_approach_1000.log'
'./brute_force_10000.o > brute_force_10000.log'
'./brute_force_opt_10000.o > brute_force_opt_10000.log'
'./divide_and_conquer_10000.o > divide_and_conquer_10000.log'
'./incremental_approach_10000.o > incremental_approach_10000.log'
'./brute_force_100000.o > brute_force_100000.log'
'./brute_force_opt_100000.o > brute_force_opt_100000.log'
'./divide_and_conquer_100000.o > divide_and_conquer_100000.log'
'./incremental_approach_100000.o > incremental_approach_100000.log'
'./brute_force_1000000.o > brute_force_1000000.log'
'./brute_force_opt_1000000.o > brute_force_opt_1000000.log'
'./divide_and_conquer_1000000.o > divide_and_conquer_1000000.log'
'./incremental_approach_1000000.o > incremental_approach_1000000.log'
'./brute_force_10000000.o > brute_force_10000000.log'
'./brute_force_opt_10000000.o > brute_force_opt_10000000.log'
'./divide_and_conquer_10000000.o > divide_and_conquer_10000000.log'
'./incremental_approach_10000000.o > incremental_approach_10000000.log'
'./brute_force_100000000.o > brute_force_100000000.log'
'./brute_force_opt_100000000.o > brute_force_opt_100000000.log'
'./divide_and_conquer_100000000.o > divide_and_conquer_100000000.log'
'./incremental_approach_100000000.o > incremental_approach_100000000.log'
)

total=${#commands[@]}
i=0

for cmd in "${commands[@]}"; do
  ((i++))
  echo "[$i/$total] START: $cmd"
  start=$(date +%s)

  timeout 900s bash -c "$cmd"
  status=$?

  end=$(date +%s)
  elapsed=$((end - start))

  if [ $status -eq 0 ]; then
    echo "[$i/$total] DONE  (${elapsed}s)"
  elif [ $status -eq 124 ]; then
    echo "[$i/$total] TIMEOUT (${elapsed}s)"
  else
    echo "[$i/$total] FAIL status=$status (${elapsed}s)"
  fi
  echo
done
