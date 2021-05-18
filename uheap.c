#include <inc/lib.h>


// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
#define heap_size (USER_HEAP_MAX - USER_HEAP_START)/4096
int first_created = 0;
struct block_info
{
	int satus;
	int no_of_blocks;
};
struct bestfit_info
{
	int free_blocks;
	int index;

};
struct block_info block [heap_size] = {};
uint32 va;
void* malloc(uint32 size)
{
	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side malloc]
	// Write your code here, remove the panic and write your code
	int blocks_to_allocate = ROUNDUP(size,4096)/4096;
	if(first_created == 0)
	{
		for(int i =0;i<heap_size;i++)
		{
			block[i].satus = 0;
			block[i].no_of_blocks = 0;
		}
		first_created = 1;
	}
	if(size > heap_size*4096)
	{
		return NULL;
	}
	else{
	int method = sys_isUHeapPlacementStrategyFIRSTFIT() ;
	//1) FIRST FIT strategy
	if(method == 1)
		{

			int allocated = 0;
			int i = 0;
			int count = 0;
			while(allocated == 0 && i < heap_size-blocks_to_allocate)
			{
				if(block[i].satus == 0)
				{
					int temp_ind = i;
					for(int j = 0;j<blocks_to_allocate;j++)
					{
						//cprintf("test = %d \t %d  status = %d \n",test,temp_ind,block[j].satus);
						if(block[temp_ind].satus == 1)
						{
							//cprintf("break test");
							count = 0;
							i = temp_ind;
							break;
						}else
						{
							//cprintf("test = %d \t count =%d \n",test,count);
							count++;
						}
						temp_ind++;
					}

					//cprintf("count = %d \t blocks to allocat = %d \n",count,blocks_to_allocate);
					if(count == blocks_to_allocate)
					{
						int temp_ind = i;
						for(int j = 0;j<blocks_to_allocate;j++)
						{
							block[temp_ind].satus = 1;
							block[temp_ind].no_of_blocks = blocks_to_allocate - j;
							//cprintf("no_of_blocks = %d \t blocks_to_allocate = %d \n",block[temp_ind].no_of_blocks,blocks_to_allocate);
							temp_ind++;
						}

						allocated = 1;
						break;
					}
				}
				i++;
			}
			//cprintf("size = %d \t heapsize = %d \n",size,heap_size*4096);
			if(allocated == 0)
			{
				//cprintf("null value va = %x \n",va);
				return NULL;
			}else
			{
				va = (4096 * i) + USER_HEAP_START;
				//cprintf("test3 %x \n",va);
				sys_allocateMem(va,size);

			}
		}
	//2) BEST FIT strategy
		else
		{
			int test = 0;
			struct bestfit_info min;
			min.free_blocks = heap_size+1;
			int count = 0;
			int temp = 0;
			int allocated = 0;
			for(int i = 0;i<heap_size-blocks_to_allocate;i++)
			{
				count = 0;
				int j = i;
				while(j<heap_size && block[j].satus == 0)
				{
					count++;
					j++;
				}
				if(blocks_to_allocate<=count)
				{
					//cprintf("count = %d \n",count);
					if(count < min.free_blocks)
					{
						min.free_blocks = count;
						min.index = i;
						allocated = 1;
					}
				}
				i = j;
			}
			//cprintf("count = %d min ind = %d test = %d \n",count,min.index,test);
			if(allocated == 1)
			{
				int temp_ind = min.index;
				//cprintf("blocks to allocat = %d \n",blocks_to_allocate);
				for(int i = 0;i<blocks_to_allocate;i++)
				{
					block[temp_ind].satus = 1;
					block[temp_ind].no_of_blocks = blocks_to_allocate - i;
					temp_ind++;
				}
				va = (4096 * min.index) + USER_HEAP_START;
				sys_allocateMem(va,size);
			}else
			{
				return NULL;
			}
		}
	}
	//refer to the documentation for details

	//This function should find the space of the required range by either:



	//cprintf("%x \n",va);
	//cprintf("va = %x \n",va);
	return (void*) va;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side free]
	// Write your code here, remove the panic and write your code
	int index = ((uint32) virtual_address-USER_HEAP_START)/4096;
	int number = block[index].no_of_blocks;
	int temp = index;
	uint32 size = block[temp].no_of_blocks*PAGE_SIZE;
	//cprintf("size = %d \t number = %d \n",size,number);
	for(int i =0;i<number;i++)
	{
		block[temp].satus = 0;
		//block[temp].no_of_blocks = 0;
		temp++;
	}
	//cprintf("virtual address = %d \t size = %d \n",virtual_address,size);
	sys_freeMem((uint32) virtual_address,size);
	//you should get the size of the given allocation using its address
	//refer to the documentation for details
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//
//===============
// [1] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}


//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
