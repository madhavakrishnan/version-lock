#!/bin/bash
# for a fixed-size workload
for dist in uniform zipfian; do
  for wl in a b c; do 
    for th in 1 2 4 8 16 24 32; do
      for index_type in vlht lfht rwht; do
        echo running ${index_type} with ${th} threads
        # allocation on numa node 1 and thread pinning inside code to node 0
        numactl --membind=1 ./ycsb ${index_type} ${wl} ${dist} ${th}
        echo =============================================================
        sleep 10
      done
    done
  done
done

