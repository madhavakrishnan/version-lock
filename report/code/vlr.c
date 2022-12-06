inline uint64_t get_version_number() {return this->version_number;}

inline uint64_t version_lock::read_lock() {
	
	/* just return the current version number 
	 * no need of checking the lock status*/
	return get_version_number();
}

inline bool version_lock::read_unlock(uint64_t initial_version_number) {
	
	uint64_t current_version_number = get_version_number();
	
	/* if both version numbers are equal means no write has happened during the
	 * read operation. so read can return successful*/
	if (likely(initial_version_number == current_version_number))
		return true;
	
	/* if version numbers mismatch means that some writer has completed before
	 * the read operation*/
	return false;
}

