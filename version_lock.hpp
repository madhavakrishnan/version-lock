#pragma once 
#include<cassert>
#include<cerrno>
#include"arch.h"

namespace vl {

#define RESET_LOCK 0
/* can only be odd-- 1, 3, 5, 7*/
#define SWITCH_LOCK_STATUS 1 

class version_lock {
public:
	version_lock();
	~version_lock();
	
	/* acquires lock, spins until lock is available*/
	void write_lock();  
	/* returns nothing; just unlocks*/
	void write_unlock(); 
	/* if lock is not available returns false immediately*/
	inline bool try_write_lock();

	/* returns VersionNumber upon success, spins until lock is 
	 * available if writer is present*/
	uint64_t read_lock(); 
	/* returns false if lock is not immediately available*/
	inline uint64_t try_read_lock(); 
	/* allows reader irrespective of on-going write*/
	inline uint64_t read_lock_no_wait(); 
	/* returns true if version number matches, application needs to retry if
	 * false is returned*/
	inline bool read_unlock(uint64_t local_version_number); 

private:
	volatile uint64_t version_number;
	inline uint64_t get_version_number() {return this->version_number;}
};

version_lock::version_lock() {
	this->version_number = 0;
}

version_lock::~version_lock() {
	this->version_number = UINT64_MAX;
}


void version_lock::write_lock() {

	volatile uint64_t version;
	volatile bool lock_status;

retry_lock_acquire:
	/* check if lock is already held
	 * even-- lock is free
	 * odd-- lock is held*/
	version = get_version_number();
	lock_status = (bool)version % 2;

	/* if lock is free acquire the lock and return*/
	if (!lock_status) {
		/* return if CAS succeeds-- meaning lock is acquired*/
		if (smp_cas(&this->version_number, version, version + SWITCH_LOCK_STATUS))	
			return;
	}
	/* TODO: FOR SUMIT: implement a exponential back off scheme here if busy
	 * wait leads to performance collpase in the evalauation*/
	goto retry_lock_acquire;

}

inline bool version_lock::try_write_lock() {
	
	/* check if lock is already held
	 * even-- lock is free
	 * odd-- lock is held*/
	uint64_t version = get_version_number();
	bool lock_status = (bool)version % 2;

	/* if lock is free acquire the lock and return*/
	if (!lock_status) {
		/* return if CAS succeeds-- meaning lock is acquired*/
		if (smp_cas(&this->version_number, version, version + SWITCH_LOCK_STATUS))	
			return true;
	}
	/* return false-- meaning lock is busy
	 * if lock_status or upon CAS failure*/
	return false;
}

/* lock acquired by using either of try_write_lock or write_lock can be unlocked
 * using the write_unlock API*/
inline void version_lock::write_unlock() {

	uint64_t version = get_version_number();

	/* RESET version_number to 0 to avoid overflow*/
	if (unlikely(version + 1 == UINT64_MAX - 1))
		smp_cas(&this->version_number, version, RESET_LOCK);
	/* no need of CAS, as it is not possible for 2 threads to call unlock on the
	 * same lock, so just atomically increament the version_number by 1*/
	smp_faa(&this->version_number, SWITCH_LOCK_STATUS);
	return;
}

uint64_t version_lock::read_lock() {

	volatile uint64_t version;
	volatile bool lock_status;

retry_lock_status:
	/* check if lock is already held by a writer
	 * even-- lock is free
	 * odd-- lock is held*/
	version = get_version_number();
	lock_status = (bool)version % 2;
	
	/* if lock is free, return the current version number*/
	if (!lock_status)
		return version;

	/* else perform busy wait*/
	/* TODO: FOR SUMIT: implement a exponential back off scheme here if busy
	 * wait leads to performance collpase in the evalauation*/
	goto retry_lock_status;
}

inline uint64_t version_lock::try_read_lock() {

	uint64_t version = get_version_number();
	bool lock_status = (bool)version % 2;
	
	/* if lock is free, return the current version number*/
	if (!lock_status)
		return version;

	/* else perform busy wait*/
	return UINT64_MAX;

}

inline uint64_t version_lock::read_lock_no_wait() {
	
	/* just return the current version number 
	 * no need of checking the lock status*/
	return get_version_number();
}

/* read lock acquired using try_read_lock, read_lock, and read_lock_no_wait can
 * all be unlocked using this API*/
inline bool version_lock::read_unlock(uint64_t initial_version_number) {
	
	uint64_t current_version_number = get_version_number();
	
	/*TODO: remove while running final evaluation*/
	assert(initial_version_number <= current_version_number);
	/* if both version numbers are equal means no write has happened during the
	 * read operation. so read can return successful*/
	if (likely(initial_version_number == current_version_number))
		return true;
	
	/* if version numbers mismatch means that some writer has completed before
	 * the read operation*/
	return false;

}






























}
