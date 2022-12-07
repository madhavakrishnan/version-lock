#!/bin/bash
# for a fixed-size workload
for dist in uniform zipfian; do
  for wl in a b c d; do 
    for th in 1 2 4 8 16 24 32 48 64; do
      for index_type in vlht lfht rwht; do
        echo running ${index_type} with ${th} threads
        ./ycsb ${index_type} ${wl} ${dist} ${th}
        echo =============================================================
        sleep 10
      done
    done
  done
done

