#pragma once 
#include"node.hpp"

class list {
public:
	list();
	//list(uint64_t key, uint64_t val);
	~list();
	inline node *alloc_node(uint64_t key, uint64_t val);
	//inline node *get_list_head();
	//inline node **get_list_head_ptr();
	//inline node *get_list_head_atomic();
	
	/* thread unsafe functions for Version lock and RW lock*/
	bool insert(uint64_t key, uint64_t val);
	uint64_t lookup(uint64_t target_key);
	bool remove(uint64_t target_key);
	int print_list();
	int get_list_size();

	/* thread safe lock free varients*/
	bool lf_insert(uint64_t key, uint64_t val);
  // 2 implementation of lock-free lookups : snapshot & linearizable
	uint64_t lf_lookup_snapshot(uint64_t target_key);
	uint64_t lf_lookup_linearizable(uint64_t target_key);
	bool lf_remove(uint64_t target_key);
private:
  // this is the sentinel node
	node *head;
};

list::list() {
  // allocate a sentinel node
	this->head = new node;
}

/*list::list(uint64_t key, uint64_t val) {
	this->head = new node(key, val);
}*/

list::~list() {
  // TODO:: remove items, if any
	delete this->head;
}

inline node* list::alloc_node(uint64_t key, uint64_t val) {
	node *new_node = new node(key, val);
	return new_node;
}

/*inline node** list::get_list_head_ptr() {
	return &this->head;
}

inline node* list::get_list_head() {
	return this->head;
}

inline node* list::get_list_head_atomic() {
	return smp_atomic_load(&this->head);
}*/

/* this function inserts only at the head
 * it is not thread safe
 *
 * TODO: do we need update implmentation?
 * if so then we have to traverse the bucket chain
 * fully as we are simply adding to head*/
bool list::insert(uint64_t key, uint64_t val) {
  // insert doesn't check if the key is already present. That functionality
  // will be a part of upsert (update + insert)
	node *new_node = alloc_node(key, val);
	if (!new_node) {
		std::cerr << "node alloc failed " << std::endl;
		return false;
	}
	new_node->next = head->next;
  head->next = new_node;
	return true;
}

/* this function is not thread safe*/
uint64_t list::lookup(uint64_t target_key) {
	volatile node *curr = head->next;
	while (curr) {
		if (curr->key == target_key)
			return curr->val;
		curr = curr->next;
	}
	return UINT64_MAX;
}

/* this function is not thread safe*/
bool list::remove(uint64_t target_key) {
	volatile node *prev = head, *curr = head->next;
	while(curr) {
		if (curr->key == target_key) {
			prev->next = curr->next;
			delete curr;
			return true;
		}
    prev = curr;
		curr = curr->next;
	}
	return false;
}

int list::print_list() {
	volatile node *curr = head->next;
	int _node = 0;
	while (curr) {
		std::cout<< " node-"<< _node << ": " << curr->key << "\t" << curr->val << std::endl;
		std::cout<<"========================\n";
		curr = curr->next;
		++_node;
	}
	return _node;
}

int list::get_list_size() {
	volatile node *curr = head->next;
	int _node = 0;
	while (curr) {
		curr = curr->next;
		++_node;
	}
	return _node;
}


/* this function inserts only at the head
 * lock-free varient*/
bool list::lf_insert(uint64_t key, uint64_t val) {

	volatile uint64_t curr_tag;
	volatile node *curr_next;

	node *new_node = alloc_node(key, val);
	if (!new_node) {
		std::cerr << "node alloc failed " << std::endl;
		return false;
	}

cas_retry:
	curr_tag = head->tag;
	curr_next = head->next;
	new_node->next = curr_next;
	if (smp_cas16b(&head->tag, curr_tag, curr_next, curr_tag + 1, new_node)) {
		return true;
  }
	/* execute a write memory barrier before CAS retry*/
	smp_wmb();
	goto cas_retry;
}

/* this function is thread safe-- lock-free*/
uint64_t list::lf_lookup_snapshot(uint64_t target_key) {
	volatile node *curr = head->next;
	while (curr) {
		if (curr->key == target_key)
			return curr->val;
		curr = curr->next;
	}
	return UINT64_MAX;
}

uint64_t list::lf_lookup_linearizable(uint64_t target_key) {
  // tag before starting traversal
  volatile uint64_t init_tag = head->tag;
  do {
    volatile node *curr = head->next;
    while (curr) {
      if (curr->key == target_key)
        return curr->val;
      curr = curr->next;
    }
  } while (head->tag != init_tag);
  
	return UINT64_MAX;
}


