#include "MemoryManagement.h"

// add the define from header, this code should only be added 
#undef malloc
#undef calloc
#undef realloc
#undef free

// TODO: Do we need a allocated description, are constant strings enough?


typedef struct Node {
	char* desc; // description
	void* ptr; // pointer value
	size_t size; // how many bytes
	Node* next;
} Node;

typedef struct List {
	Node* first;
	Node* last;
} List;

/*
* This will kept nodes, where every node has the info about one memory allocation
* When memory is freed, a node is removed from here 
* NOTE -- Don't care about performance here, used only for debug
*/
static List allocated_memory;


static inline Node* create_node(void* ptr, size_t size, char* desc) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->desc = desc;
	node->next = NULL;
	node->ptr = ptr;
	node->size = size;
}


static inline void add_node(Node* node) {
	if (allocated_memory.first == NULL) {
		allocated_memory.first = node;
		allocated_memory.last = node;
	}
	else {
		allocated_memory.last->next = node;
		allocated_memory.last = node->next;
	}
}


static inline Node* find_node(void* ptr) {
	Node* iter = allocated_memory.first;
	while (iter->ptr != ptr && iter != NULL) iter = iter->next;
	return iter;
}


static inline bool update_node(void* old_ptr, void* new_ptr, size_t new_size, char* new_desc) {
	Node* iter = find_node(old_ptr);
	check(iter != NULL, "%p pointer does not have a node", old_ptr);

	// update information
	iter->ptr = new_ptr;
	iter->size = new_size;
	iter->desc = new_desc;

	return TRUE;

error:
	return FALSE;
}


bool memory_manage_is_empty() {
	// if there is an element in the list then their is memory that is not deallocated
	if (allocated_memory.first != NULL) return FALSE;
	return TRUE;
}


void memory_manage_report();


void* memory_manage_malloc(size_t size, char* desc) {
	// allocate the memory
	void* ptr = malloc(size);
	check_memory(ptr);

	// save the information
	Node* node = create_node(ptr, size, desc);
	add_node(node);

error:
	return NULL;
}


void* memory_manage_calloc(size_t nitems, size_t size, char* desc) {
	// allocate the memory
	void* ptr = calloc(nitems, size);
	check_memory(ptr);

	// save the information
	Node* node = create_node(ptr, size, desc);
	add_node(node);
	
error:
	return ptr;
}


void* memory_manage_realloc(void* ptr, size_t size, char* desc) {
	// try to realloc
	void* n_ptr = realloc(ptr, size);
	check_memory(ptr);

	// update the node with the new information
	update_node(ptr, n_ptr, size, desc);

error:
	// realloc failed return old data
	return ptr;
}


void memory_manage_free(void* ptr) {
	Node* node = find_node(ptr);
	check(node != NULL, "Node should exist for %p", ptr);

	// need previous

error:
	return;
}