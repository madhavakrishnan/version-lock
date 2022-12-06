bool vl_hash_table::insert(uint64_t key, uint64_t val) {

	uint64_t bkt;
	bool ret;
	
	bkt = get_key_hash(key);
	ht[bkt].lock.write_lock();
	ret = ht[bkt]._list->insert(key, val);
	if (!ret) {
	 std::cerr <<  " insert falied " << std::endl;
	}
	ht[bkt].lock.write_unlock();
	return true;
}

uint64_t vl_hash_table::lookup(uint64_t key) {
	
	uint64_t bkt, version, val;
	bool retry;

	bkt = get_key_hash(key);

loop:
	version = ht[bkt].lock.read_lock();
	val = ht[bkt]._list->lookup(key);
	retry = ht[bkt].lock.read_unlock(version);
	if (!retry)
		goto loop;
	return val;
}

