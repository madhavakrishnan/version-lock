
/* acquires lock, spins until lock is available*/
void write_lock();  
/* if lock is not available returns false immediately*/
inline bool try_write_lock();
/* unlocks write_lock acquired by write_lock or try_write_lock */
void write_unlock(); 


/* allows the reader and returns the current version number*/ 
uint64_t read_lock(); 

/* returns VersionNumber upon success, spins until lock is 
 * available if writer is present*/
inline uint64_t read_lock_wait(); 

/* returns true if version number matches, application 
 * needs to retry if false is returned*/
inline bool read_unlock(uint64_t local_version_number); 
