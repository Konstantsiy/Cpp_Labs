#include <string.h>
#include <iostream>

using namespace std;

const int MAX_POOLS = 1000;
const long BUF_SIZE = 104888320;

static char ALLBUF[BUF_SIZE];             // ��������� ������
static unsigned long AVAIBLE = BUF_SIZE;  // ��������

static char* pools[MAX_POOLS];              // ����

static unsigned int POOLS_COUNT = 1;        // ���������� �����

static unsigned int pools_size[MAX_POOLS]; // ������� �����

static char* blocks[MAX_POOLS];  // �����
static unsigned int BLOCKS_COUNT = 0;  // ���������� ������������ ������
static unsigned int block_size[MAX_POOLS]; // ������� ������
static unsigned int RIGHT_BLOCK;  // ����� ������ ������� �����

#define NO_MEMORY 1
#define BLOCK_NOT_FOUND 2


void ars_alloc_init(void);   // �������������
char* ars_malloc(unsigned long Size);  // ���������
int ars_free(char*);                  // ������������
int ars_defrag(void);               // ��������������
void ars_alloc_stat();   // ����������


int main() {
	ars_alloc_init();
	bool flag = true;
	char* block = nullptr;
	cout << "1. malloc.\n2. free.\n3. defrag.\n4. get statistics.\n0. End.\n";
	while (flag) {
		cout << ">";
		switch (getchar()) {
		case '1': {
			int memorySize;
			cout << "Enter size of needed memory: ";
			cin >> memorySize;
			block = ars_malloc(memorySize);
			if (block == nullptr) {
				cout << "Error allocaction memory.\n";
				return -1;
			}
			break;
		}
		case '2': {
			ars_free(block);
			break;
		}
		case '3': {
			ars_defrag();
			break;
		}
		case '4': {
			ars_alloc_stat();
			break;
		}
		case '0': 
			flag = false;
			break;
		}
	}
	return 0;
}

//=========================================
void ars_alloc_init(void) {
	pools[0] = ALLBUF; // ������ ������� ����
	pools_size[0] = AVAIBLE;
}
//=========================================

char* ars_malloc(unsigned long Size) {
	unsigned long int i, k;
	char *p;

	// ���� ������� ������, ��� ����
	if (Size > AVAIBLE) {
		return nullptr;
	}

	p = 0;
	// ���� ������ ���������� ���
	for (i = 0; i < POOLS_COUNT; ++i)
		if (Size <= pools_size[i]) {
			p = pools[i]; // ���������� �����
			k = i;        // � �����
			break;
		}

	if (!p) {
		return nullptr;
	}

	blocks[BLOCKS_COUNT] = p;  // ����� ����� ����� ����
	block_size[BLOCKS_COUNT] = Size;
	++BLOCKS_COUNT;
	++RIGHT_BLOCK;
	pools[k] = (char*)(p + Size + 1);  // ������� ����� ������ ���� �� ����� �����
	pools_size[k] = pools_size[k] - Size;  // � ���������� ������

	AVAIBLE -= Size;  // ����� ����� ������
	return p;
}
//==========================================

int ars_free(char* block)
{
	unsigned int i, k;
	char* p = 0;
	// ���� ���� �� ������
	for (i = 0; i < RIGHT_BLOCK; ++i)
		if (block == blocks[i]) {
			p = blocks[i];
			k = i;
			break;
		}
	if (!p) {
		return BLOCK_NOT_FOUND;  // ���� �� ������
	}

	blocks[k] = 0;
	--BLOCKS_COUNT;
	pools[POOLS_COUNT] = block; // ���������� ���� � ���
	pools_size[POOLS_COUNT] = block_size[k];
	++POOLS_COUNT;
	AVAIBLE += block_size[k]; // ����� ����� ������

	return 0;
}
//==========================================

// ������� ������������������. ������� ��� ����� ����� � ���������
//  ������������ ���

int ars_defrag(void) {
	unsigned int i, k;
	char* p = ALLBUF;
	char* t, *tmp;

	for (i = 0; i < RIGHT_BLOCK; ++i)
	{
		t = blocks[i];
		if (t == ALLBUF) {
			p = (char*)(blocks[i] + block_size[i] + 1);
			continue;
		}
		tmp = p;
		for (k = 0, t = blocks[i]; k < block_size[i]; ++k) *p++ = *t++;
		blocks[i] = tmp;
	}

	POOLS_COUNT = 1;
	BLOCKS_COUNT = POOLS_COUNT;
	pools[0] = p;
	AVAIBLE = BUF_SIZE - (unsigned long)(p - ALLBUF + 1);
	pools_size[0] = AVAIBLE;
	RIGHT_BLOCK = 0;
	return 0;
}
//==========================

// ����� ����������
void ars_alloc_stat() {
	printf("\tAvaible memory: %ld of %ld bytes\n\tCount of blocks: %d\n", AVAIBLE, BUF_SIZE, BLOCKS_COUNT);
}