#include "MemoryManagement.h"

#undef malloc
#undef calloc
#undef realloc
#undef free

/********************************************************************************
* This will keep nodes in Hash Table, where the key is the memory adress of the 
* allocation. Every node has info about one memory allocation and when memory
* is freed a node is removed from the Hash Table
********************************************************************************/

/********************************************************************************
* General information about a memory allocation
* Will be stored in lists
********************************************************************************/
typedef struct Node {
	void* ptr;
	char* desc;
	size_t size;
	struct Node* next;
	struct Node* prev;
} Node;


static inline Node* create_node(void* ptr, size_t size, char* desc) {
	Node* node = (Node*)malloc(sizeof(Node));
	if (node == NULL) return node;
	node->ptr = ptr;
	node->desc = desc;
	node->size = size;
	node->next = NULL;
	node->prev = NULL;
	return node;
}


/********************************************************************************
* List of Nodes, are used as buckets for the hash table
********************************************************************************/
typedef struct List {
	Node* first;
	Node* last;
} List;

static inline void list_add(List* list, Node* node) {
	if (list->first == NULL) {
		list->first = node;
		list->last = node;
	}
	else {
		list->last->next = node;
		node->prev = list->last;
		list->last = node;
	}
}

static inline Node* list_remove_first(List* list) {
	Node* node = NULL;
	if (list->first != NULL) {
		node = list->first;
		if (node == list->last) {
			list->first = NULL;
			list->last = NULL;
		}
		else list->first = node->next;
	}
	return node;
}

static inline Node* list_find(List* list, void* ptr) {
	Node* iter = list->first;
	while (iter != NULL && iter->ptr != ptr) iter = iter->next;
	return iter;
}


static inline void list_remove(List* list, Node* node) {
	if (node == list->first && node == list->last) {
		list->first = NULL;
		list->last = NULL;
	}
	else if (node == list->first) {
		node->next->prev = NULL;
		list->first = node->next;
	}
	else if (node == list->last) {
		node->prev->next = NULL;
		list->last = node->prev;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
}


/********************************************************************************
* Hash table with linked lists
* It grows when the length / n_entries >= 0.8
********************************************************************************/
typedef struct HashTable {
	List* entries;
	size_t n_entries;
	size_t length;
} HashTable;
#define TABLE_INITIAL_LENGTH 997

static inline HashTable* hash_table_create(size_t n_entries);
static inline void hash_table_resize(HashTable* table, size_t new_size);
static inline void hash_table_add(HashTable* table, Node* node);
static inline Node* hash_table_remove(HashTable* table, void* key);


static inline BOOL is_prime(size_t n) {
	size_t i = 0;
	for (i = 2; i < n / 2; ++i) {
		if (n % i == 0) return FALSE;
	}
	return TRUE;
}

static inline size_t next_prime(size_t n) {
	if (is_prime(n) == TRUE) return n;
	size_t radius = 1;
	size_t number = 0;
	while (TRUE) {
		number = n + radius;
		if (is_prime(number) == TRUE) return number;
		number = n - radius;
		if (is_prime(number) == TRUE) return number;
		radius++;
	}
	return n;
}

static inline size_t hash_f(HashTable* table, void* address_value) {
	return (size_t)address_value % table->n_entries;
}

static inline HashTable* hash_table_create(size_t n_entries) {
	HashTable* table = (HashTable*)malloc(sizeof(HashTable));
	check_memory(table);

	table->entries = (List*)calloc(n_entries, sizeof(List));
	check_memory(table->entries);

	table->n_entries= n_entries;
	table->length = 0;

	return table;
ERROR
	if (table != NULL) {
		free(table);
	}
	return NULL;
}

static inline void hash_table_resize(HashTable* table, size_t new_size) {
	size_t i = 0;
	List* list = NULL;
	Node* node = NULL;
	size_t hash = 0;
	HashTable* new_table = hash_table_create(new_size);

	// move the nodes from @table to @new_table
	for (i = 0; i < table->n_entries; ++i) {
		list = &(table->entries[i]);
		
		// take all elements from the list
		while ((node = list_remove_first(list)) != NULL) {
			node->next = NULL;
			node->prev = NULL;
			hash_table_add(new_table, node);
		}
	}

	// overwrite table
	free(table->entries);	// table->entries contains only empty lists
	table->entries = new_table->entries;
	table->length = new_table->length;
	table->n_entries = new_table->n_entries;
	free(new_table); // no need for this anymore
}

static inline void hash_table_add(HashTable* table, Node* node) {
	// resize when 80% is filled (nr_elements / nr_entries)
	if ((float)table->length / (float)table->n_entries >= 0.8f) {
		hash_table_resize(table, next_prime(table->n_entries * 2));
	}
	size_t idx = hash_f(table, node->ptr);
	list_add(&(table->entries[idx]), node);
	(table->length)++;
}

static inline Node* hash_table_remove(HashTable* table, void* key) {
	size_t hash = hash_f(table, key);
	List* entry = &(table->entries[hash]);
	Node* node = list_find(entry, key);
	check(node != NULL, "NO node with key %p", key);
	list_remove(entry, node);
	(table->length)--;
	return node;
ERROR
	return NULL;
}


/********************************************************************************
* Keep a global Hash Table, we only need one for the whose process
* This will be freed automatically at the end of the program
********************************************************************************/
HashTable* memory_table = NULL;


/********************************************************************************
* Module Functionality
********************************************************************************/
void* memory_manage_malloc(size_t size, char* desc) {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);

	void* ptr = malloc(size);
	check_memory(ptr);

	Node* node = create_node(ptr, size, desc);
	hash_table_add(memory_table, node);
	return ptr;
ERROR
	return NULL;
}


void* memory_manage_calloc(size_t nitems, size_t size, char* desc) {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);
	void* ptr = calloc(nitems, size);
	check_memory(ptr);

	Node* node = create_node(ptr, nitems * size, desc);
	hash_table_add(memory_table, node);
	return ptr;
ERROR
	return NULL;
}

void* memory_manage_realloc(void* ptr, size_t size, char* desc) {
	check(memory_table != NULL, null_argument("memory_tabel"));
	// try to realloc
	void* n_ptr = realloc(ptr, size);
	check_memory(ptr);

	// remove node because its key may change
	Node* node = hash_table_remove(memory_table, ptr);
	check(node != NULL, "%p pointer does not have a node", ptr);

	// update node state
	node->ptr = n_ptr;
	node->size = size;
	node->desc = desc;
	node->prev = NULL;
	node->next = NULL;

	hash_table_add(memory_table, node);
	return n_ptr;
ERROR
	// realloc failed return old data
	return ptr;
}

void memory_manage_free(void* ptr) {
	check(memory_table != NULL, null_argument("memory_tabel"));
	// find node and remove it from list
	Node* node = hash_table_remove(memory_table, ptr);
	check(node != NULL, "Node should exist for %p", ptr);

	free(node->ptr);
	free(node);
ERROR
	return;
}

size_t memory_manage_memory_blocks() {
	if (memory_table == NULL) {
		memory_table = hash_table_create(TABLE_INITIAL_LENGTH);
	}
	size_t i = 0;
	size_t n_blocks = 0;
	List* list = NULL;
	Node* node = NULL;

	for (i = 0; i < memory_table->n_entries; ++i) {
		list = &(memory_table->entries[i]);
		node = list->first;

		while (node != NULL) {
			n_blocks++;
			node = node->next;
		}
	}
	return n_blocks;
}

size_t memory_manage_memory_size() {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);
	size_t i = 0;
	size_t mem_size = 0;
	List* list = NULL;
	Node* node = NULL;

	for (i = 0; i < memory_table->n_entries; ++i) {
		list = &(memory_table->entries[i]);
		node = list->first;

		while (node != NULL) {
			mem_size += node->size;
			node = node->next;
		}
	}
	return mem_size;
}

BOOL memory_manage_is_empty() {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);
	if (memory_table->length == 0) return TRUE;
	return FALSE;
}

void memory_manage_report() {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);
	size_t i = 0;
	size_t mem_size = 0;
	size_t n_nodes = 0;
	List* list = NULL;
	Node* node = NULL;

	log_info("UNFREED MEMORY");
	for (i = 0; i < memory_table->n_entries; ++i) {
		list = &(memory_table->entries[i]);
		node = list->first;

		while (node != NULL) {
			printf("Node %llu - desc: %s ptr: %p size: %llu BYTES\n", n_nodes, node->desc, node->ptr, node->size);
			n_nodes++;
			mem_size += node->size;
			node = node->next;
		}
	}

	check(n_nodes == memory_table->length, "@n_nodes != @memory_table->length");
	log_info("Summary %llu nodes, %llu BYTES not freed", n_nodes, mem_size);
ERROR
	return;
}


void memory_manage_show_inner_state(BOOL show_entries, BOOL show_empty) {
	if (memory_table == NULL) memory_table = hash_table_create(TABLE_INITIAL_LENGTH);

	size_t i = 0;
	List* list = NULL;
	Node* node = NULL;
	size_t n_lists = 0;

	log_info("INNER STATE");
	for (i = 0; i < memory_table->n_entries; ++i) {
		
		list = &(memory_table->entries[i]);
		node = list->first;
		
		if (show_entries == TRUE && (show_empty == TRUE || list->first != NULL)) printf("[%4llu] ", i);
		if (list->first != NULL) n_lists++;

		if (show_entries == TRUE) {
			while (node != NULL) {
				printf("-> %p ", node->ptr);
				node = node->next;
			}
		}
		if (show_entries == TRUE && (show_empty == TRUE || list->first != NULL)) printf("\n");
	}
	printf("TABLE LENGTH: %llu\nTABLE ENTRIES: %llu\nAverage List Length %f\n", memory_table->length, memory_table->n_entries, (float)memory_table->length / n_lists);
}