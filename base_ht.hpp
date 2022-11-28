#pragma once
#include "list.hpp"

#define N_BUCKETS_DEF 1007

class hash_table {
public:
	hash_table(uint64_t n_bkts);
	virtual ~hash_table() { }

	virtual bool insert(uint64_t key, uint64_t val) = 0;
	virtual uint64_t lookup(uint64_t key) = 0;
	virtual bool remove(uint64_t key) = 0;

	inline uint64_t get_key_hash(uint64_t key);
	inline uint64_t get_bucket(uint64_t hash);
	
  virtual int print() = 0;
	virtual int get_node_count() = 0;

protected:
	uint64_t n_buckets;
}____cacheline_aligned;


hash_table::hash_table(uint64_t n_bkts) {
  this->n_buckets = n_bkts;
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


