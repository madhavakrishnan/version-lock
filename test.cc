#include<iostream>
#include<stdlib.h>
#include<thread>
#include<atomic>
#include<vector>
#include"version_lock.hpp"
#include"list.hpp"
#include"vlht.hpp"

using namespace vl;
#define TEST_VL_API 0
#define TEST_VL_LIST 1
#define TEST_VL_HT 2
#define TEST_LF 3
#define TEST_RW 4

/* an instance of global counter and global version lock 
 * for testing purposes*/
uint64_t __g_counter;
version_lock vlock;
#define TEST_READ_LOCK_NO_WAIT
#define N_ITRS_WRITE 1000000
#define N_ITRS_READ 1000000
std::atomic<int> read_retry;

/* parameters for testing list*/
#define LIST_INIT_SIZE 100
#define N_NODE_ADD 10000

void test_version_lock_write(int tid, int itrs) 
{
	std::cout << "writer thread " << tid << " begin" << std::endl;
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
	std::cout << "writer thread " << tid << " done" << std::endl;
	return;
}

void test_version_lock_read(int tid, int itrs) 
{
	volatile uint64_t version;
	bool ret;
	
	std::cout << "reader thread " << tid << " begin" << std::endl;
	for (int i = 0; i < itrs; ++i) {
retry:
#ifdef TEST_READ_LOCK_NO_WAIT
		version = vlock.read_lock_no_wait();
#else

		version = vlock.read_lock();
#endif

#ifdef DEBUG
		std::cout << "thread= " << tid << " counter= " << __g_counter << std::endl;
#endif
		ret = vlock.read_unlock(version);
		if (!ret) {
			read_retry.fetch_add(1, std::memory_order_seq_cst);
			goto retry;
		}
	}
	std::cout << "reader thread " << tid << " done" << std::endl;
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

	threads.clear();
    std::cout << "======================\n";
	std::cout << "testing " << 2 * n_threads << " concurrent readers and writers.." 
		<< std::endl;
	/* testing concurrent readers and writers*/
	for (int i = 0; i < 2*n_threads; ++i) {
		if (i < n_threads)
			threads.push_back(std::thread(test_version_lock_write, i, N_ITRS_WRITE));
		else 
			threads.push_back(std::thread(test_version_lock_read, i, N_ITRS_READ));

	}
	for (auto &th: threads) {
		th.join();
	}
	std::cout << "global counter val= " << __g_counter << std::endl;
	std::cout << "read_retries= " << read_retry << std::endl;

    std::cout << "======================\n";

}

void vl_list_insert(list *_list, int count, uint64_t start_key, 
		std::vector<uint64_t> *keys, int tid) {
	
	std::cout << "writer thread " << tid << " begin" << std::endl;
	for (int i = 0; i < count; ++i) {
		vlock.write_lock(); 
		_list->insert(start_key + i, start_key + i);
		keys->push_back(start_key + i);
		vlock.write_unlock();
	}
	std::cout << "writer thread " << tid << " end" << std::endl;

}

void vl_list_lookup(list *_list, int start, int end, 
		std::vector<uint64_t> keys, int tid)
{
	uint64_t version, val;
	bool ret;

#ifdef DEBUG
	std::cout << "reader thread " << tid << ": " << start << "-" << end  << std::endl;
#endif

	for (int i = start; i < end; ++i) {
retry:
#ifdef TEST_READ_LOCK_NO_WAIT
		version = vlock.read_lock_no_wait();
#else
		version = vlock.read_lock();
#endif
		val = _list->lookup(keys[i]);
		assert(keys[i] == val);
		ret = vlock.read_unlock(version);
		if (!ret) {
			read_retry.fetch_add(1, std::memory_order_seq_cst);
			goto retry;
		}
	}
	std::cout << "reader thread " << tid << " end" << std::endl;
	return;

}

void test_version_lock_ht(int n_threads) 
{
	std::vector<std::thread> threads;
	std::vector<uint64_t> keys;
	hash_table ht;
	uint64_t key;
	uint64_t _val;

	/* init hash table*/
	std::cout << "Initializing Hash Table with " << LIST_INIT_SIZE 
		<< " elements.." << std::endl;
	for (uint64_t i = 0; i < LIST_INIT_SIZE; ++i) {
		key = (i + 1) * (i + 100);
		ht.insert(key, key);
		keys.push_back(key);
	}

	std::cout << "Hash table after init " << std::endl;
	int ret = ht.print();
	for (uint64_t i = 0; i < keys.size(); ++i) {
		_val = ht.lookup(keys[i]);
		if (keys[i] != _val) {
			std::cout << "key= " << keys[i] << " val= " << _val << std::endl;
			assert(keys[i] == _val);
		}
	}

	std::cout << "==================\n";
	std::cout << " Hash Table has " << ret << " nodes" << std::endl;


}

void test_version_lock_list(int n_threads) 
{
	std::vector<std::thread> threads;
	list _list;
	uint64_t start_key;
	std::vector<uint64_t> keys;
	int start, end;

	/* init list*/	
	for (uint64_t i = 0; i < LIST_INIT_SIZE; ++i) {
		_list.insert(i + 100, i + 100);
		keys.push_back(i + 100);
	}
	std::cout << "List after init" << std::endl;
	_list.print_list();
	std::cout << "===========\n";

	/* testing list with concurrent writers only*/
	std::cout << "inserting to list with " << n_threads << " threads" << std::endl;
	for (int i = 0; i < n_threads; ++i) {
		start_key = (i + 1) * 1000;
		threads.push_back(std::thread(vl_list_insert, &_list, N_NODE_ADD, 
					start_key, &keys, i));
	}
	for (auto &th:threads) {
		th.join();
	}
#ifdef DEBUG
	std::cout << "List after concurrent " <<  keys.size() << " inserts" << std::endl;
	_list.print_list();
#endif

	threads.clear();

	/* testing list with concurrent readers only*/
	std::cout << "reading list with " << n_threads << " threads" << std::endl;
	for (int i = 0; i < n_threads; ++i) {
		start = (keys.size()/n_threads) * i;
		end = start + (keys.size()/n_threads);
		threads.push_back(std::thread(vl_list_lookup, &_list, start, 
					end, keys, i));
	}
	for (auto &th:threads) {
		th.join();
	}
	std::cout << "read_retries= " << read_retry << std::endl;
	std::cout << "===========\n";

	threads.clear();
	std::cout << "testing " << 2 * n_threads << " concurrent readers and writers.." 
		<< std::endl;
	/* testing concurrent readers and writers*/
	for (int i = 0; i < 2*n_threads; ++i) {
		if (i < n_threads) {
			start = (keys.size()/n_threads) * i;
			end = start + (keys.size()/n_threads);
			threads.push_back(std::thread(vl_list_lookup, &_list, start, 
					end, keys, i));
		}
		else {
			start_key = i * 10000;
			threads.push_back(std::thread(vl_list_insert, &_list, N_NODE_ADD/10, 
					start_key, &keys, i));
		}

	}
	for (auto &th: threads) {
		th.join();
	}
	std::cout << "read_retries= " << read_retry << std::endl;
    std::cout << "======================\n";
	return;

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
	if (bench == TEST_VL_LIST)
		test_version_lock_list(n_threads);
	if (bench == TEST_VL_HT)
		test_version_lock_ht(n_threads);
	return 0;
}
