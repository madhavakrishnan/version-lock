# version-lock
WSC coursework project

## Implementation

### Hash table variants:
1. vlht.hpp => protected by version lock
2. lfht.hpp => lock-free
3. rwht.hpp => protected by reader-writer lock

### Executable
ycsb.cpp => entry point for execution. Some of the important variables include
1. `LOAD_SIZE` : number of items in hash table before running a workload (set to 64M)

https://github.com/madhavakrishnan/version-lock/blob/main/ycsb.cpp#L160

2. `RUN_SIZE` : number of operations to run for a workload (set to 64M)

https://github.com/madhavakrishnan/version-lock/blob/main/ycsb.cpp#L161

3. `HASH_BUCKETS` : number of buckets for each hash table (set to 8M)

https://github.com/madhavakrishnan/version-lock/blob/main/ycsb.cpp#L163

To build the executable, run `make` and it will generate a binary named `ycsb`

To run the executable, specify four parameters on the command line as shown below

./ycsb [hash table type] [ycsb workload type] [access pattern] [number of threads]

These parameters are

- hash table type : specify either of `vlht`, `lfht`, `rwht` for hash table based on version lock, lock-free or reader-writer lock
- ycsb workload type : specify either of a, b , c or d
- access pattern : either `uniform` or `zipfian`
- number of threads : number of threads to use for a particular evaluation

NOTE:: before running the binary, make sure to generate the YCSB workloads as shown next

## Generating YCSB workloads
### Download YCSB source code
```
$ cd ./index-microbench
$ curl -O --location https://github.com/brianfrankcooper/YCSB/releases/download/0.11.0/ycsb-0.11.0.tar.gz
$ tar xfvz ycsb-0.11.0.tar.gz
$ mv ycsb-0.11.0 YCSB
```
### How to configure and generate workloads
Configure the options of each workload (a, b, c, d), change `$recordcount` and `$operationcount` (`$recordcount` should be same as `LOAD_SIZE` and `$operationcount` should be same as `RUN_SIZE` in ycsb.cpp as described above).
```
$ vi ./index-microbench/workload_spec/<workloada or workloadb or workloadc or workloadd>
```
In the same file, change the key distribution in `$requestdistribution` (set to either `uniform` or `zipfian`, this parameter is the same as `access pattern` when running the ycsb binary generated above)

```

Generate the workloads. This will generate random integer keys with the specified key distribution.
```
$ cd ./index-microbench/
$ mkdir workloads
$ bash generate_all_workloads.sh

```
This will produce .dat files for the load and run phase within `workloads` folder. We will move the generated .dat files into folders based on distribution and size of workload as follows
```
$ mkdir workloads/uniform 
$ mkdir workloads/zipfian
```
If we generated workload for 64M operations, create a new folder for the size
```
$ mkdir workloads/uniform/64M
$ mkdir workloads/zipfian/64M
```
Finally, move the generated .dat files from the workloads folder into the correct location. As an example, if we generated workload for 64M operations with uniform distribution, move the files by running
```
$ mv workloads/*.dat workloads/uniform/64M/
```

Once the workload is moved to the right location, run the `ycsb` executable as described above.






















