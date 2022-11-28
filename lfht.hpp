#pragma once
#include "base_ht.hpp"

class lf_hash_table : public hash_table {
public:
	struct bucket {
		list *_list;
	}____cacheline_aligned;

	lf_hash_table(uint64_t n_bkts = N_BUCKETS_DEF);
	~lf_hash_table();

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


lf_hash_table::lf_hash_table(uint64_t n_bkts) : hash_table(n_bkts) {
	
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

lf_hash_table::~lf_hash_table() {
	free(this->mem);
}

bool lf_hash_table::insert(uint64_t key, uint64_t val) {

	uint64_t bkt;
	bool ret;
	
	bkt = get_key_hash(key);
	ret = ht[bkt]._list->lf_insert(key, val);
	if (!ret) {
		std::cerr << "hash insert falied for key " << key << std::endl;
		assert(ret);
	}
	return true;
}

uint64_t lf_hash_table::lookup(uint64_t key) {
	
	uint64_t bkt, val;

	bkt = get_key_hash(key);
	val = ht[bkt]._list->lf_lookup_snapshot(key);
	//val = ht[bkt]._list->lf_lookup_linearizable(key);
	return val;
}

bool lf_hash_table::remove(uint64_t key) {
  // add impl
  return true;
}

int lf_hash_table::print() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		std::cout << "bucket # " << i << std::endl;
		n_elems += ht[i]._list->print_list();
	}
	return n_elems;
}

int lf_hash_table::get_node_count() {
	
	int n_elems = 0;

	for (uint64_t i = 0; i < this->n_buckets; ++i) {
		n_elems += ht[i]._list->get_list_size();
	}
	return n_elems;
}


