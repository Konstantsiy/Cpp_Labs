#include <stdio.h>

#define MEMORY_SIZE 256

#define MEMORY_ALLOCATION_ERROR nullptr
#define MEMORY_CLEANING_ERROR 0

int memorySize = 256;
char virtualMemory[MEMORY_SIZE];

typedef struct memoryBlock {
	memoryBlock* next;
	int blockSize;
	bool isAvailable;
	int remainingBytes;
	void* memoryPointer;
	void* old;
};

memoryBlock memoryHead;

void __init() {
	memoryHead.next = NULL;
	memoryHead.blockSize = 0;
	memoryHead.isAvailable = 1;
	memoryHead.remainingBytes = MEMORY_SIZE;
	memoryHead.memoryPointer = virtualMemory;
	memoryHead.old = virtualMemory;
}

void __svap(void* destinationBlock, void* sourceBlock, int size) {
	for (int i = 0; i < size; i++) {
		((char*)destinationBlock)[i] = ((char*)sourceBlock)[i];
	}
}

void* __malloc(size_t size) {
	if (size < 0 || size > MEMORY_SIZE) {
		return MEMORY_ALLOCATION_ERROR;
	}
	memoryBlock* temp = &memoryHead;
	while (temp) {
		if (temp->isAvailable) {
			if (temp->remainingBytes > size) {
				memorySize -= size;
				temp->blockSize = size;
				temp->isAvailable = 0;
				temp->next = (temp)+sizeof(memoryBlock);

				memoryBlock* nextMemoryHead = temp->next;
				nextMemoryHead->isAvailable = 1;
				nextMemoryHead->remainingBytes = memorySize;
				nextMemoryHead->memoryPointer = (&(temp->memoryPointer) + size);
				nextMemoryHead->old = nextMemoryHead->memoryPointer;
				nextMemoryHead->next = NULL;
				return temp->memoryPointer;
			}
			return MEMORY_ALLOCATION_ERROR;
		}
		temp = temp->next;
	}
	return MEMORY_ALLOCATION_ERROR;
}

void __getInfo() {
	printf(" Available memory: %d\n", memorySize);
	memoryBlock *head = &memoryHead;
	while (head) {
		printf("%x\t%d\t%d\t%d\n", head->memoryPointer, head->blockSize, head->isAvailable, head->remainingBytes);
		head = head->next;
	}
}

void __free(void* pointer) {
	if (pointer != 0) {
		memoryBlock* temp = &memoryHead;

		while (temp->memoryPointer != pointer)  
			temp = temp->next;

		temp->isAvailable = 1;		           
		memorySize += temp->blockSize;	
		char *t = (char*)temp->memoryPointer;
		for (int i = 0; i < temp->blockSize; i++, t++)
			*t = 0;
	}
	else printf("Memory is not allocated to this pointer\n");
}

void __defrag() {
	memoryBlock *t, *temp = &memoryHead;

	while (temp) {
		if (temp->isAvailable) {
			t = temp->next;
			while (t && t->isAvailable)
				t = t->next;
			if (t) {
				__svap(temp->memoryPointer, t->memoryPointer, t->blockSize);
				temp->isAvailable = 0;
				temp->blockSize = t->blockSize;
				t->isAvailable = 1;
				t->remainingBytes = memorySize;
				temp->old = t->old;
				t->memoryPointer = &temp->memoryPointer + temp->blockSize;
				temp->next = t;
			}
		}
		temp = temp->next;
	}
}

int main() {
	__init();
	char *ptr1 = (char*)__malloc(10);
	char *ptr2 = (char*)__malloc(15);
	char *ptr3 = (char*)__malloc(9);
	char *ptr4 = (char*)__malloc(20);
	__getInfo();
	__free(ptr3);
	__getInfo();
	__defrag();
	__getInfo();
	return 0;
}
