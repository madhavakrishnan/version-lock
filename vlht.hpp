#pragma once
#include<shared_mutex>
#include"list.hpp"
#include"version_lock.hpp"

using namespace vl;
#define N_BUCKETS_DEF 19

class hash_table {
public:
	struct bucket {
		list *_list;
		version_lock lock;
	}____cacheline_aligned;

	hash_table();
	hash_table(uint64_t n_buckets);
	~hash_table();
	bool insert(uint64_t key, uint64_t val);
	uint64_t lookup(uint64_t key);
	bool remove(uint64_t key);
	inline uint64_t get_key_hash(uint64_t key);
	inline uint64_t get_bucket(uint64_t hash);
	int print();

private:
	uint64_t n_buckets;
	bucket *ht;
}____cacheline_aligned;

hash_table::hash_table() {
	size_t size = sizeof(bucket) * N_BUCKETS_DEF;
	this->n_buckets = N_BUCKETS_DEF;
	this->ht = (bucket *)aligned_alloc(DEFAULT_ALIGNMENT, size);
	if (!this->ht) {
		std::cerr << "Hash Table allocation failed" << std::endl;
		assert(this->ht);
	}
	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		ht[i]._list = new list;
	}
}

hash_table::hash_table(uint64_t n_buckets) {
	size_t size = sizeof(bucket) * n_buckets;
	this->n_buckets = n_buckets;
	this->ht = (bucket *)aligned_alloc(DEFAULT_ALIGNMENT, size);
	if (!this->ht) {
		std::cerr << "Hash Table allocation failed" << std::endl;
		assert(this->ht);
	}
	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		ht[i]._list = new list;
	}
}

hash_table::~hash_table() {
	free(ht);
}

inline uint64_t hash_table::get_bucket(uint64_t key) {

	if (key > this->n_buckets)
		key = key % this->n_buckets;
	return key;
}

inline uint64_t hash_table::get_key_hash(uint64_t key) {

	key += ~(key << 32);
	key ^= (key >> 22);
	key += ~(key << 13);
	key ^= (key >> 8);
	key += (key << 3);
	key ^= (key >> 15);
	key += ~(key << 27);
	key ^= (key >> 31);
	
	return get_bucket(key);
}

bool hash_table::insert(uint64_t key, uint64_t val) {

	uint64_t bkt;
	bool ret;
	
	bkt = get_key_hash(key);
	ht[bkt].lock.write_lock();
	ret = ht[bkt]._list->insert(key, val);
	if (!ret) {
		std::cerr << "hash insert falied for key " << key << std::endl;
		assert(ret);
	}
	ht[bkt].lock.write_unlock();
	return true;
}

uint64_t hash_table::lookup(uint64_t key) {
	
	uint64_t bkt, version, val;
	bool retry;

	bkt = get_key_hash(key);

loop:
	version = ht[bkt].lock.read_lock_no_wait();
	val = ht[bkt]._list->lookup(key);
	retry = ht[bkt].lock.read_unlock(version);
	if (!retry)
		goto loop;
	return val;
}

bool hash_table::remove(uint64_t key) {
	
	uint64_t bkt;
	bool ret;

	bkt = get_key_hash(key);
	ht[bkt].lock.write_lock();
	ret = ht[bkt]._list->remove(key);
	if (!ret) {
		std::cerr << "hash remove falied for key " << key << std::endl;
		assert(ret);
	}
	ht[bkt].lock.write_unlock();
	return true;
}

int hash_table::print() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		std::cout << "bucket # " << i << std::endl;
		n_elems += ht[i]._list->print_list();
	}
	return n_elems;
}







