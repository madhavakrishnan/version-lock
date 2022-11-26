#pragma once 
#include<cstdint>
#include<cstdlib>
#include"arch.h"

//#define DEFAULT_ALIGNMENT 16
#define DEFAULT_ALIGNMENT 64

struct node {
	uint64_t key;
	uint64_t val;
	volatile uint64_t tag; /* will be used only for lock free implementation*/
	volatile node *next;

	/* constructor for dummy node*/
	node() {
		key = UINT64_MAX;
		val = UINT64_MAX;
		tag = 0;
		next = NULL;
	}
	/* constructor with init node*/
	node(uint64_t _key, uint64_t _val) {
		key = _key;
		val = _val;
		tag = 0;
		next = NULL;
	}

	void *operator new(size_t size) {
		
		/* needs to be 16b aligned for CAS16b*/
		void *ptr = aligned_alloc(DEFAULT_ALIGNMENT, size);
		return ptr;
	}

	void operator delete(void *ptr) {
		free(ptr);
	}
//} ____ptr_aligned;
} ____cacheline_aligned;

