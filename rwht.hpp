#pragma once
#include <mutex>
#include <shared_mutex>
#include "base_ht.hpp"

class rw_hash_table : public hash_table {
public:
  typedef std::shared_mutex rw_lock;
  typedef std::unique_lock<rw_lock> write_lock;
  typedef std::shared_lock<rw_lock> read_lock;

	struct bucket {
		list *_list;
		rw_lock lock;
	}____cacheline_aligned;

	rw_hash_table(uint64_t n_bkts = N_BUCKETS_DEF);
	~rw_hash_table();

  // pure virtual fns from base class
	bool insert(uint64_t key, uint64_t val);
	uint64_t lookup(uint64_t key);
	bool remove(uint64_t key);
  int print();
	int get_node_count();

private:
  void *mem;
	bucket *ht;
}____cacheline_aligned;


rw_hash_table::rw_hash_table(uint64_t n_bkts) : hash_table(n_bkts) {

  size_t size = sizeof(bucket) * this->n_buckets;
	this->mem = aligned_alloc(DEFAULT_ALIGNMENT, size);
  this->ht = new (this->mem) bucket[this->n_buckets];
	if (!this->ht) {
		std::cerr << "Hash Table allocation failed" << std::endl;
		assert(this->ht);
	}
	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		ht[i]._list = new list;
	}
}

rw_hash_table::~rw_hash_table() {
  free(this->mem);
}

bool rw_hash_table::insert(uint64_t key, uint64_t val) {

	uint64_t bkt;
	bool ret;
	
	bkt = get_key_hash(key);
  write_lock w_lock(ht[bkt].lock);
	ret = ht[bkt]._list->insert(key, val);
	if (!ret) {
		std::cerr << "hash insert falied for key " << key << std::endl;
		assert(ret);
	}
	return true;
}

uint64_t rw_hash_table::lookup(uint64_t key) {
	
	uint64_t bkt, val;

	bkt = get_key_hash(key);
  read_lock r_lock(ht[bkt].lock);
	val = ht[bkt]._list->lookup(key);
	return val;
}

bool rw_hash_table::remove(uint64_t key) {
	
	uint64_t bkt;
	bool ret;

	bkt = get_key_hash(key);
  write_lock w_lock(ht[bkt].lock);
	ret = ht[bkt]._list->remove(key);
	if (!ret) {
		std::cerr << "hash remove falied for key " << key << std::endl;
		assert(ret);
	}
	return true;
}

int rw_hash_table::print() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		std::cout << "bucket # " << i << std::endl;
		n_elems += ht[i]._list->print_list();
	}
	return n_elems;
}

int rw_hash_table::get_node_count() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		n_elems += ht[i]._list->get_list_size();
	}
	return n_elems;
}


