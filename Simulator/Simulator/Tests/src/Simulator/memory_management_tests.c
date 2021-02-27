#include "memory_management_tests.h"
#include "utils/MemoryManagement.h"


TestStatus memory_management_general_test() {
	TestStatus status = TEST_FAILED;

	// should not have any memory registered
	size_t memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == 0, "@memory_blocks is %llu, not 0", memory_blocks);
	size_t memory_size = memory_manage_memory_size();
	assert(memory_size == 0, "@memory_size is %llu, not 0", memory_size);
	
	// do 1000 mallocs 
	uint32_t malloc_allocations = 50;
	size_t memory_block_size = 1000;
	uint32_t i = 0;
	void* ptrs[1000] = { 0 };
	for (i = 0; i < malloc_allocations; ++i) {
		ptrs[i] = malloc(memory_block_size, "malloc_allocation");
	}

	// check status of memory
	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == malloc_allocations, "@memory_blocks is %llu, not %u", memory_blocks, malloc_allocations);
	memory_size = memory_manage_memory_size();
	assert(memory_size == memory_block_size * malloc_allocations, "@memory_size is %llu, not %llu", memory_size, memory_block_size * malloc_allocations);

	// clean memory
	for (i = 0; i < malloc_allocations; ++i) {
		free(ptrs[i]);
	}

	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == 0, "@memory_blocks is %llu, not 0", memory_blocks);
	memory_size = memory_manage_memory_size();
	assert(memory_size == 0, "@memory_size is %llu, not 0", memory_size);

	// do 1000 callocs
	uint32_t calloc_allocations = 1000;
	void* ptrs2[1000] = { 0 };
	for (i = 0; i < calloc_allocations; i++) {
		ptrs2[i] = calloc(1, memory_block_size, "calloc_allocation");
	}
	// check status of memory
	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == calloc_allocations, "@memory_blocks is %llu, not %u", memory_blocks, calloc_allocations);
	memory_size = memory_manage_memory_size();
	assert(memory_size == memory_block_size * calloc_allocations, "@memory_size is %llu, not %llu", memory_size, memory_block_size * calloc_allocations);

	// realloc all of them with double size
	for (i = 0; i < calloc_allocations; i++) {
		ptrs2[i] = realloc(ptrs2[i], memory_block_size * 2, "realloc_allocation");
	}

	// check status of memory
	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == calloc_allocations, "@memory_blocks is %llu, not %u", memory_blocks, calloc_allocations);
	memory_size = memory_manage_memory_size();
	assert(memory_size == 2 * memory_block_size * calloc_allocations, "@memory_size is %llu, not %llu", memory_size, 2 * memory_block_size * calloc_allocations);

	memory_manage_show_inner_state(FALSE, FALSE);

	// clean memory
	for (i = 0; i < calloc_allocations; ++i) {
		free(ptrs2[i]);
	}

	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == 0, "@memory_blocks is %llu, not 0", memory_blocks);
	memory_size = memory_manage_memory_size();
	assert(memory_size == 0, "@memory_size is %llu, not 0", memory_size);

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus memory_manage_report_test() {
	TestStatus status = TEST_FAILED;
	
	void* ptr1 = malloc(100000, "malloc_simple");
	void* ptr2 = calloc(3, 300000, "calloc_simple");
	BOOL is_empty = memory_manage_is_empty();
	assert(is_empty == FALSE, "should have memory");
	size_t memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == 2, "@memory_blocks is %llu, not 2", memory_blocks);
	size_t memory_size = memory_manage_memory_size();
	size_t real_memory_size = 100000 + 3 * 300000;
	assert(memory_size == real_memory_size, "@memory_size is %llu, not %llu", memory_size, real_memory_size);

	memory_manage_report();

	free(ptr1);
	free(ptr2);
	memory_blocks = memory_manage_memory_blocks();
	assert(memory_blocks == 0, "@memory_blocks is %llu, not 0", memory_blocks);
	memory_size = memory_manage_memory_size();
	assert(memory_size == 0, "@memory_size is %llu, not 0", memory_size);

	status = TEST_SUCCESS;

error:
	return status;
}
