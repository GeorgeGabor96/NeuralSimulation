#include "MemoryManagement.h"

#undef malloc
#undef calloc
#undef realloc
#undef free


typedef struct Node {
	char* desc;
	void* ptr;
	size_t size;
	struct Node* next;
	struct Node* prev;
} Node;


typedef struct List {
	Node* first;
	Node* last;
} List;


/********************************************************************************
* This will keep nodes, where every node has the info about one memory allocation
* When memory is freed, a node is removed from here
* 
* NOTE -- Don't care about performance here, used only to find memory leaks
********************************************************************************/
static List allocated_memory;


static inline Node* create_node(void* ptr, size_t size, char* desc) {
	Node* node = (Node*)malloc(sizeof(Node));
	if (node == NULL) return node;
	node->desc = desc;
	node->ptr = ptr;
	node->size = size;
	node->next = NULL;
	node->prev = NULL;
	return node;
}


static inline void add_node(Node* node) {
	if (allocated_memory.first == NULL) {
		allocated_memory.first = node;
		allocated_memory.last = node;
	}
	else {
		allocated_memory.last->next = node;
		node->prev = allocated_memory.last;
		allocated_memory.last = node;
	}
}

static inline void remove_node(Node* node) {
	if (node == allocated_memory.first && node == allocated_memory.last) {
		allocated_memory.first = NULL;
		allocated_memory.last = NULL;
	}
	else if (node == allocated_memory.first) {
		node->next->prev = NULL;
		allocated_memory.first = node->next;
	}
	else if (node == allocated_memory.last) {
		node->prev->next = NULL;
		allocated_memory.last = node->prev;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
}


static inline Node* find_node(void* ptr) {
	Node* iter = allocated_memory.first;
	while (iter != NULL && iter->ptr != ptr) iter = iter->next;
	return iter;
}


static inline bool update_node(void* old_ptr, void* new_ptr, size_t new_size, char* new_desc) {
	Node* iter = find_node(old_ptr);
	check(iter != NULL, "%p pointer does not have a node", old_ptr);

	iter->ptr = new_ptr;
	iter->size = new_size;
	iter->desc = new_desc;

	return TRUE;

error:
	return FALSE;
}


uint32_t memory_manage_memory_blocks() {
	Node* iter = allocated_memory.first;
	uint32_t memory_blocks = 0;

	while (iter != NULL) {
		memory_blocks++;
		iter = iter->next;
	}
	return memory_blocks;
}


size_t memory_manage_memory_size() {
	Node* iter = allocated_memory.first;
	size_t memory_size = 0;

	while (iter != NULL) {
		memory_size += iter->size;
		iter = iter->next;
	}
	return memory_size;
}


bool memory_manage_is_empty() {
	// if there is an element in the list then their is memory that is not deallocated
	if (allocated_memory.first != NULL) return FALSE;
	return TRUE;
}


void memory_manage_report() {
	Node* iter = NULL;
	size_t n_nodes = 0;
	size_t total_memory = 0;

	log_info("UNFREED MEMORY");
	for (iter = allocated_memory.first; iter != NULL; iter = iter->next) {
		printf("Node %llu - desc: %s ptr: %p size: %llu BYTES\n", n_nodes, iter->desc, iter->ptr, iter->size);
		n_nodes++;
		total_memory += iter->size;
	}
	log_info("Summary %llu nodes, %llu BYTES not freed", n_nodes, total_memory);
}


void* memory_manage_malloc(size_t size, char* desc) {
	void* ptr = malloc(size);
	check_memory(ptr);

	Node* node = create_node(ptr, size, desc);
	add_node(node);
	return ptr;

error:
	return NULL;
}


void* memory_manage_calloc(size_t nitems, size_t size, char* desc) {
	void* ptr = calloc(nitems, size);
	check_memory(ptr);

	Node* node = create_node(ptr, nitems * size, desc);
	add_node(node);
	return ptr;
error:
	return NULL;
}


void* memory_manage_realloc(void* ptr, size_t size, char* desc) {
	// try to realloc
	void* n_ptr = realloc(ptr, size);
	check_memory(ptr);

	update_node(ptr, n_ptr, size, desc);
	return n_ptr;

error:
	// realloc failed return old data
	return ptr;
}


void memory_manage_free(void* ptr) {
	// find node and remove it from list
	Node* node = find_node(ptr);
	check(node != NULL, "Node should exist for %p", ptr);

	remove_node(node);
	free(node);

error:
	return;
}