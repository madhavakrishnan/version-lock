void pmdk_overlapping_allocation(nvm_heap *heap) {
    void *p[1024], *free;
    int i;

    /* Make the NVM heap full of 64-byte objects */
    for (i = 0; true; i = ++i % 1024) {
        if ( !(p[i] = nvm_malloc(64)) )
      	    break;
    }

    /* Free an arbitrary object but before freeing
     * the object, corrupt the size in its allocation
     * header to larger number. It will make the PMDK
     * allocator corrupt its allocation bitmap. */
    free = p[i/2];
    *(uint64_t *)(free - 16) = 1088; /* Corrupt header!!! */ @\label{f:pmdkbug:corruption0}@
    nvm_free(free);

    /* Since only one object is freed, the NVM heap
     * should be able to allocate only one 64-byte object.
     * But due to the allocation bitmap corruption
     * in the previous step, 9 objects will be allocated.
     * Unfortunately, 8 out of 9 will be already allocated
     * objects so it will cause user data corruption. */
    for (i = 0; true; i = ++i % 1024) {
        if ( !(p[i] = nvm_malloc(64)) ) @\label{f:pmdkbug:overlap}@
	          break;
        assert(p[i] == free); /* This will fail!!! */ @\label{f:pmdkbug:assert0}@
    }
}
