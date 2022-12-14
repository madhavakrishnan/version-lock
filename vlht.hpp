#pragma once
#include "base_ht.hpp"
#include "version_lock.hpp"

using namespace vl;

class vl_hash_table : public hash_table {
public:
	struct bucket {
		list *_list;
		version_lock lock;
	}____cacheline_aligned;

	vl_hash_table(uint64_t n_bkts = N_BUCKETS_DEF);
	~vl_hash_table();

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


vl_hash_table::vl_hash_table(uint64_t n_bkts) : hash_table(n_bkts) {
	
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

vl_hash_table::~vl_hash_table() {
	//free(ht);
	free(this->mem);
}

bool vl_hash_table::insert(uint64_t key, uint64_t val) {

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

uint64_t vl_hash_table::lookup(uint64_t key) {
	
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

bool vl_hash_table::remove(uint64_t key) {
	
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

int vl_hash_table::print() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		std::cout << "bucket # " << i << std::endl;
		n_elems += ht[i]._list->print_list();
	}
	return n_elems;
}

int vl_hash_table::get_node_count() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		n_elems += ht[i]._list->get_list_size();
	}
	return n_elems;
}


