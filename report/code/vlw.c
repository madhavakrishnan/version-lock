inline uint64_t get_version_number() {return this->version_number;}

void version_lock::write_lock() {

	volatile uint64_t version;
	volatile bool lock_status;

retry_lock_acquire:
	/* check if lock is already held
	 * even-- lock is free
	 * odd-- lock is held*/
	version = get_version_number();
	lock_status = version % 2;

	/* if lock is free acquire the lock and return*/
	if (!lock_status) {
		/* return if CAS succeeds-- meaning lock is acquired*/
		if (smp_cas(&this->version_number, version, version + SWITCH_LOCK_STATUS))	
			return;
	}

	goto retry_lock_acquire;

}

inline void version_lock::write_unlock() {

	uint64_t version = get_version_number();

	/* RESET version_number to 0 to avoid overflow*/
	if (unlikely(version + 1 == UINT64_MAX - 1))
		smp_cas(&this->version_number, version, RESET_LOCK);

	smp_faa(&this->version_number, SWITCH_LOCK_STATUS);
	return;
}

