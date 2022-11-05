#include<iostream>
#include<stdlib.h>
#include<thread>
#include<atomic>
#include<vector>
#include"version_lock.hpp"

using namespace vl;
#define TEST_VL_API 0
#define TEST_VL 1
#define TEST_LF 2
#define TEST_RW 3

/* an instance of global counter and global version lock 
 * for testing purposes*/
uint64_t __g_counter;
version_lock vlock;
#define N_ITRS_WRITE 10
#define N_ITRS_READ 1
std::atomic<int> read_retry;

void test_version_lock_write(int tid, int itrs) 
{
	std::cout << "thread " << tid << " begin" << std::endl;
	for (int i = 0; i < itrs; ++i) {
#ifdef DEBUG
		std::cout << "itr= " << i << " begin counter= " << __g_counter << std::endl;
#endif
		vlock.write_lock();
		++__g_counter;
		vlock.write_unlock();
#ifdef DEBUG
		std::cout << "itr= " << i << " end counter= " << __g_counter << std::endl;
		std::cout << "======================\n";
#endif
	}
	std::cout << "thread " << tid << " done" << std::endl;
	return;
}

void test_version_lock_read(int tid, int itrs) 
{
	volatile uint64_t version;
	bool ret;
	
	std::cout << "thread " << tid << " begin" << std::endl;
	for (int i = 0; i < itrs; ++i) {
retry:
		version = vlock.read_lock();
#ifdef DEBUG
		std::cout << "thread= " << tid << " counter= " << __g_counter << std::endl;
#endif
		ret = vlock.read_unlock(version);
		if (!ret) {
			read_retry.fetch_add(1, std::memory_order_seq_cst);
			goto retry;
		}
	}
	std::cout << "thread " << tid << " done" << std::endl;
	return;
}

void test_version_lock_apis(int n_threads) 
{
	std::vector<std::thread> threads;

    std::cout << "======================\n";
	std::cout << "testing " << n_threads << " concurrent writers.." << std::endl;
	/* testing concurrent writers only*/
	for (int i = 0; i < n_threads; ++i) {
		threads.push_back(std::thread(test_version_lock_write, i, N_ITRS_WRITE));
	}
	for (auto &th: threads) {
		th.join();
	}
	std::cout << "global counter val= " << __g_counter << std::endl;
	std::cout << "read_retries= " << read_retry << std::endl;
	uint64_t expected_val = N_ITRS_WRITE * n_threads;
	assert(__g_counter == expected_val);

	threads.clear();
    std::cout << "======================\n";
	std::cout << "testing " << n_threads << " concurrent readers.." << std::endl;
	/* testing concurrent readers only*/
	for (int i = 0; i < n_threads; ++i) {
		threads.push_back(std::thread(test_version_lock_read, i, N_ITRS_READ));
	}
	for (auto &th: threads) {
		th.join();
	}
	std::cout << "global counter val= " << __g_counter << std::endl;
	std::cout << "read_retries= " << read_retry << std::endl;

    std::cout << "======================\n";

}

int main(int argc, char **argv)
{
	int n_threads, bench;

	if (argc < 3 || argc > 3) {
		std::cerr << " useage: ./test --threads  --bench_type" << std::endl;
		return -1;
	}
	/* arg1-- n_threads; arg2- bench type*/
	n_threads = atoi(argv[1]);
	bench = atoi(argv[2]);
	std::cout << "threads= " << n_threads << " bench= " << bench << std::endl;
	if (bench == TEST_VL_API)
		test_version_lock_apis(n_threads);
	return 0;
}
