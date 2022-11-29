#include <iostream>
#include <chrono>
#include <random>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "tbb/tbb.h"
#include <atomic>
#include <thread>
#include <sched.h>
#include <string.h>
#include <time.h>

#include "vlht.hpp"
#include "rwht.hpp"
#include "lfht.hpp"

using namespace std;

// hash table types
enum {
    VERSION_LOCK_HT,
    RW_LOCK_HT,
    LOCK_FREE_HT,
};

// operation types
enum {
    OP_INSERT,
    OP_READ,
};

// workload types
enum {
    WORKLOAD_A,
    WORKLOAD_B,
    WORKLOAD_C,
    WORKLOAD_D,
};

// key distributions
enum {
    UNIFORM,
    ZIPFIAN,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//	thread pinning functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////
cpu_set_t cpu_set[64];
enum {
    NUM_SOCKET = 2,
    NUM_PHYSICAL_CPU_PER_SOCKET = 16,
    SMT_LEVEL = 2,
};

const int OS_CPU_ID[NUM_SOCKET][NUM_PHYSICAL_CPU_PER_SOCKET][SMT_LEVEL] = {
    { /* socket id: 0 */
        { /* physical cpu id: 0 */
          0, 32,     },
        { /* physical cpu id: 1 */
          1, 33,     },
        { /* physical cpu id: 2 */
          2, 34,     },
        { /* physical cpu id: 3 */
          3, 35,     },
        { /* physical cpu id: 4 */
          4, 36,     },
        { /* physical cpu id: 5 */
          5, 37,     },
        { /* physical cpu id: 6 */
          6, 38,     },
        { /* physical cpu id: 7 */
          7, 39,     },
        { /* physical cpu id: 8 */
          8, 40,     },
        { /* physical cpu id: 9 */
          9, 41,     },
        { /* physical cpu id: 10 */
          10, 42,     },
        { /* physical cpu id: 11 */
          11, 43,     },
        { /* physical cpu id: 12 */
          12, 44,     },
        { /* physical cpu id: 13 */
          13, 45,     },
        { /* physical cpu id: 14 */
          14, 46,     },
        { /* physical cpu id: 15 */
          15, 47,     },
    },
    { /* socket id: 1 */
        { /* physical cpu id: 0 */
          16, 48,     },
        { /* physical cpu id: 1 */
          17, 49,     },
        { /* physical cpu id: 2 */
          18, 50,     },
        { /* physical cpu id: 3 */
          19, 51,     },
        { /* physical cpu id: 4 */
          20, 52,     },
        { /* physical cpu id: 5 */
          21, 53,     },
        { /* physical cpu id: 6 */
          22, 54,     },
        { /* physical cpu id: 7 */
          23, 55,     },
        { /* physical cpu id: 8 */
          24, 56,     },
        { /* physical cpu id: 9 */
          25, 57,     },
        { /* physical cpu id: 10 */
          26, 58,     },
        { /* physical cpu id: 11 */
          27, 59,     },
        { /* physical cpu id: 12 */
          28, 60,     },
        { /* physical cpu id: 13 */
          29, 61,     },
        { /* physical cpu id: 14 */
          30, 62,     },
        { /* physical cpu id: 15 */
          31, 63,     },
    },
};

int get_cpu_id()
{
	static int curr_sock = 0;
	static int curr_phy_cpu = 0;
	static int curr_smt = 0;
	int ret;

	ret = OS_CPU_ID[curr_sock][curr_phy_cpu][curr_smt];
	++curr_phy_cpu;
	if (curr_phy_cpu == NUM_PHYSICAL_CPU_PER_SOCKET) {
		curr_phy_cpu = 0;
		++curr_smt;
	}
	if (curr_smt == SMT_LEVEL) {
		++curr_sock;
		curr_smt = 0; 
		if (curr_sock == NUM_SOCKET) {
			curr_sock = 0;
		}
	}
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////
// # ops
static uint64_t LOAD_SIZE = 32000000;
static uint64_t RUN_SIZE = 32000000;
// # buckets in the hash table
static uint64_t HASH_BUCKETS = 6000000;

void ycsb_load_run_randint(int index_type, int wl, int ap, int num_thread,
        std::vector<uint64_t> &init_keys,
        std::vector<uint64_t> &keys,
        std::vector<int> &ops)
{
    std::string init_file;
    std::string txn_file;

    if (ap == UNIFORM) {
        if (wl == WORKLOAD_A) {
            init_file = "./index-microbench/workloads/32M/loada_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsa_unif_int.dat";
        } else if (wl == WORKLOAD_B) {
            init_file = "./index-microbench/workloads/32M/loadb_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsb_unif_int.dat";
        } else if (wl == WORKLOAD_C) {
            init_file = "./index-microbench/workloads/32M/loadc_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsc_unif_int.dat";
        } else if (wl == WORKLOAD_D) {
            init_file = "./index-microbench/workloads/32M/loadd_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsd_unif_int.dat";
        } else {
            std::cout << "incorrect workload type, exiting !!!" << std::endl;
        }
    } else {
        if (wl == WORKLOAD_A) {
            init_file = "./index-microbench/workloads/32M/loada_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsa_unif_int.dat";
        } else if (wl == WORKLOAD_B) {
            init_file = "./index-microbench/workloads/32M/loadb_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsb_unif_int.dat";
        } else if (wl == WORKLOAD_C) {
            init_file = "./index-microbench/workloads/32M/loadc_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsc_unif_int.dat";
        } else if (wl == WORKLOAD_D) {
            init_file = "./index-microbench/workloads/32M/loadd_unif_int.dat";
            txn_file = "./index-microbench/workloads/32M/txnsd_unif_int.dat";
        } else {
            std::cout << "incorrect workload type, exiting !!!" << std::endl;
        }
    }

    std::ifstream infile_load(init_file);

    std::string op;
    uint64_t key;

    std::string insert("INSERT");
    std::string read("READ");

    uint64_t count = 0;
    while ((count < LOAD_SIZE) && infile_load.good()) {
        infile_load >> op >> key;
        if (op.compare(insert) != 0) {
            std::cout << "READING LOAD FILE FAIL!\n";
            return;
        }
        init_keys.push_back(key);
        count++;
    }
    assert(count == LOAD_SIZE);

    std::ifstream infile_txn(txn_file);

    count = 0;
    while ((count < RUN_SIZE) && infile_txn.good()) {
        infile_txn >> op >> key;
        if (op.compare(insert) == 0) {
            ops.push_back(OP_INSERT);
            keys.push_back(key);
        } else if (op.compare(read) == 0) {
            ops.push_back(OP_READ);
            keys.push_back(key);
        } else {
            std::cout << "UNRECOGNIZED CMD!\n";
            return;
        }
        count++;
    }
    assert(count == RUN_SIZE);

    hash_table *hash_index;
    if (index_type == VERSION_LOCK_HT) {
        hash_index = new vl_hash_table(HASH_BUCKETS);
    } else if (index_type == RW_LOCK_HT) {
        hash_index = new rw_hash_table(HASH_BUCKETS);
    } else if (index_type == LOCK_FREE_HT) {
        hash_index = new lf_hash_table(HASH_BUCKETS);
    } else {
        std::cout << "incorrect index type, exiting !!!" << std::endl;
        return;
    }

    std::atomic<int> next_thread_id;
    {
        // Load
        auto starttime = std::chrono::system_clock::now();
        next_thread_id.store(0);
        auto func = [&]() {
            int thread_id = next_thread_id.fetch_add(1);
	          //sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set[thread_id]);
                
            uint64_t start_key = LOAD_SIZE / num_thread * (uint64_t)thread_id;
            uint64_t end_key = start_key + LOAD_SIZE / num_thread;
            bool ins_result;
            for (uint64_t i = start_key; i < end_key; i++) {
                // using key as the value to verify later
                ins_result = hash_index->insert(init_keys[i], init_keys[i]);
                assert(ins_result);
            }
        };
            
        std::vector<std::thread> thread_group;
        for (int i = 0; i < num_thread; i++) {
            thread_group.push_back(std::thread{func});
        }

        for (int i = 0; i < num_thread; i++) {
            thread_group[i].join();
        }

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::system_clock::now() - starttime);
        printf("Throughput: load, %f ,ops/us\n", (LOAD_SIZE * 1.0) / duration.count());
    }
    
    {
        // Run
        auto starttime = std::chrono::system_clock::now();
        next_thread_id.store(0);
        auto func = [&]() {
            int thread_id = next_thread_id.fetch_add(1);
            //sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set[thread_id]);
            uint64_t start_key = RUN_SIZE / num_thread * (uint64_t)thread_id;
            uint64_t end_key = start_key + RUN_SIZE / num_thread;
            bool ins_result;
            uint64_t read_result;
            for (uint64_t i = start_key; i < end_key; i++) {
                if (ops[i] == OP_INSERT) {
                    ins_result = hash_index->insert(keys[i], keys[i]);
                    assert(ins_result); 
                } else if (ops[i] == OP_READ) {
                    read_result = hash_index->lookup(keys[i]);
                    // this might not be true when multiple threads are used &
                    // key read is inserting during run phase and not load of
                    // the ycsb execution, check this!!
                    assert(read_result == keys[i]);
                }
            }
        };

        std::vector<std::thread> thread_group;
        for (int i = 0; i < num_thread; i++) {
            thread_group.push_back(std::thread{func});
        }

        for (int i = 0; i < num_thread; i++) {
            thread_group[i].join();
        }
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::system_clock::now() - starttime);
        printf("Throughput: run, %f ,ops/us\n", (RUN_SIZE * 1.0) / duration.count());
    }
}

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cout << "Usage: ./ycsb [hash table type] [ycsb workload type] [access pattern] [number of threads]\n";
        std::cout << "1. hash table type: vlht rwht lfht\n";
        std::cout << "2. ycsb workload type: a, b, c, d\n";
        std::cout << "3. access pattern: uniform, zipfian\n";
        std::cout << "4. number of threads (integer)\n";
        return 1;
    }

    printf("%s, workload%s, %s, threads %s\n", argv[1], argv[2], argv[3], argv[4]);

    int n_threads = atoi(argv[4]);
    int cpu_id;
    CPU_ZERO(&cpu_set[0]);
    for (int i = 0; i < n_threads; ++i) {
		    cpu_id = get_cpu_id();
		    CPU_SET(cpu_id, &cpu_set[0]);
    }

    int index_type;
    if (strcmp(argv[1], "vlht") == 0)
        index_type = VERSION_LOCK_HT;
    else if (strcmp(argv[1], "rwht") == 0)
        index_type = RW_LOCK_HT;
    else if (strcmp(argv[1], "lfht") == 0)
        index_type = LOCK_FREE_HT;
    else {
        fprintf(stderr, "Unknown index type: %s\n", argv[1]);
        exit(1);
    }

    int wl;
    if (strcmp(argv[2], "a") == 0) {
        wl = WORKLOAD_A;
    } else if (strcmp(argv[2], "b") == 0) {
        wl = WORKLOAD_B;
    } else if (strcmp(argv[2], "c") == 0) {
        wl = WORKLOAD_C;
    } else if (strcmp(argv[2], "d") == 0) {
        wl = WORKLOAD_D;
    } else {
        fprintf(stderr, "Unknown workload: %s\n", argv[2]);
        exit(1);
    }

    int ap;
    if (strcmp(argv[3], "uniform") == 0) {
        ap = UNIFORM;
    } else if (strcmp(argv[3], "zipfian") == 0) {
        ap = ZIPFIAN;
    } else {
        fprintf(stderr, "Unknown access pattern: %s\n", argv[3]);
        exit(1);
    }

    int num_thread = atoi(argv[4]);
    tbb::task_scheduler_init init(num_thread);

    std::vector<uint64_t> init_keys;
    std::vector<uint64_t> keys;
    std::vector<int> ops;

    init_keys.reserve(LOAD_SIZE);
    keys.reserve(RUN_SIZE);
    ops.reserve(RUN_SIZE);

    memset(&init_keys[0], 0x00, LOAD_SIZE * sizeof(uint64_t));
    memset(&keys[0], 0x00, RUN_SIZE * sizeof(uint64_t));
    memset(&ops[0], 0x00, RUN_SIZE * sizeof(int));

    ycsb_load_run_randint(index_type, wl, ap, num_thread, init_keys, keys, ops);
    return 0;
}

