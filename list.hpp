#pragma once 
#include"node.hpp"

class list {
public:
	list();
	list(uint64_t key, uint64_t val);
	~list();
	inline node *alloc_node(uint64_t key, uint64_t val);
	inline node *get_list_head();
	inline node **get_list_head_ptr();
	
	/* thread unsafe functions for Version lock and RW lock*/
	bool insert(uint64_t key, uint64_t val);
	uint64_t lookup(uint64_t target_key);
	bool remove(uint64_t target_key);
	int print_list();
	int get_list_size();

	/* thread safe lock free varients*/
	bool lf_insert(uint64_t key, uint64_t val);
	uint64_t lf_lookup(uint64_t target_key);
	bool lf_remove(uint64_t target_key);
	inline node *get_list_head_atomic();
private:
	node *head;
};

list::list() {
	this->head = NULL;
}

list::list(uint64_t key, uint64_t val) {
	this->head = new node(key, val);
}

list::~list() {
	delete this->head;
}

inline node* list::alloc_node(uint64_t key, uint64_t val) {
	node *new_node = new node(key, val);
	return new_node;
}

inline node** list::get_list_head_ptr() {
	return &this->head;
}

inline node* list::get_list_head() {
	return this->head;
}

inline node* list::get_list_head_atomic() {
	return smp_atomic_load(&this->head);
}

/* this function inserts only at the head
 * it is not thread safe
 *
 * TODO: do we need update implmentation?
 * if so then we have to traverse the bucket chain
 * fully as we are simply adding to head*/
bool list::insert(uint64_t key, uint64_t val) {
	node **head = get_list_head_ptr();
	if (!head) {
		*head = alloc_node(key, val);
		return true;
	}
	node *new_node = alloc_node(key, val);
	if (!new_node) {
		std::cerr << "node alloc failed " << std::endl;
		return false;
	}
	new_node->next = *head;
	*head = new_node;
	return true;
}

/* this function is not thread safe*/
uint64_t list::lookup(uint64_t target_key) {
	node *head = get_list_head();
	if (!head)
		return UINT64_MAX;
	node *curr = head;
	while (curr) {
		if (curr->key == target_key)
			return curr->val;
		curr = curr->next;
	}
	return UINT64_MAX;
}

/* this function is not thread safe*/
bool list::remove(uint64_t target_key) {
	
	node **_head = get_list_head_ptr();
	node *head = *_head;

	if (!head)
		return false;

	if (head->key == target_key) {
		delete head;
		_head = NULL;
		return true;
	}
	node *curr = head->next;
	node *prev = head;
	while(curr) {
		if (curr->key == target_key) {
			prev->next = curr->next;
			delete curr;
			return true;
		}
		curr = curr->next;
	}
	return false;
}

int list::print_list() {
	node *head = get_list_head();
	node *curr = head;
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
	node *head = get_list_head();
	node *curr = head;
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

	volatile uint64_t tag;
	volatile node *curr_next;

	node *new_node = alloc_node(key, val);
	if (!new_node) {
		std::cerr << "node alloc failed " << std::endl;
		return false;
	}

cas_retry:
	node **_head = get_list_head_ptr();
	node *head = smp_atomic_load(_head);
	if (!head) {
		/* when head is NULL no need to check tag value*/
		if (smp_cas(_head, NULL, new_node))
			return true;
		/* execute a write memory barrier before CAS retry*/
		smp_wmb();
		goto cas_retry;

	}
	/* get the tag of head node*/
	tag = smp_atomic_load(&head->tag);
	curr_next = smp_atomic_load(&head->next);
	new_node->next = head;
	if (smp_cas16b(&head->tag, tag, curr_next, tag + 1, new_node))
		return true;
	/* execute a write memory barrier before CAS retry*/
	smp_wmb();
	goto cas_retry;
}

/* this function is thread safe-- lock-free*/
uint64_t list::lf_lookup(uint64_t target_key) {
	
	node *head = get_list_head_atomic();
	if (!head)
		return UINT64_MAX;
	node *curr = head;
	while (curr) {
		if (smp_atomic_load(&curr->key) == target_key)
			return curr->val;
		curr = smp_atomic_load(&curr->next);
	}
	return UINT64_MAX;
}























