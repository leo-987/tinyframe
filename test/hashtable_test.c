#include <stdio.h>
#include <assert.h>

#include "hash.h"

void hash_table_test()
{
	hash_table *ht;
	int key1;
	int key2;
	int key3;
	int key4;
	int value1;
	int value2;
	int value3;
	int value4;
	
	ht = hash_table_create(100);
	key1 = 1;
	value1 = 1;
	key2 = 11;
	value2 = 2;
	key3 = 101;
	value3 = 3;
	key4 = 2;
	value4 = 4;
	
	hash_table_insert(ht, key1, &value1);
	hash_table_insert(ht, key2, &value2);
	hash_table_insert(ht, key3, &value3);
	hash_table_insert(ht, key4, &value4);

	assert(hash_table_find(ht, key1) != NULL);
	assert(*(int *)hash_table_find(ht, key1)->value == value1);
	
	assert(hash_table_find(ht, key2) != NULL);
	assert(*(int *)hash_table_find(ht, key2)->value == value2);
	
	assert(hash_table_find(ht, key3) != NULL);
	assert(*(int *)hash_table_find(ht, key3)->value == value3);
	
	assert(hash_table_find(ht, key4) != NULL);
	assert(*(int *)hash_table_find(ht, key4)->value == value4);
	
	assert(hash_table_find(ht, 123) == NULL);

	assert(hash_table_remove(ht, key1) == 0);
	assert(hash_table_remove(ht, key2) == 0);
	assert(hash_table_remove(ht, 123) == -1);

	assert(hash_table_find(ht, key1) == NULL);
	assert(hash_table_find(ht, key2) == NULL);

	hash_table_insert(ht, 123, &value4);
	
	assert(hash_table_find(ht, key3) != NULL);
	assert(*(int *)hash_table_find(ht, key3)->value == value3);
	
	assert(hash_table_find(ht, key4) != NULL);
	assert(*(int *)hash_table_find(ht, key4)->value == value4);
	
	assert(hash_table_find(ht, 123) != NULL);
	assert(*(int *)hash_table_find(ht, 123)->value == value4);
		
	hash_table_free(ht);
}

int main()
{
	hash_table_test();
	printf("hash table test OK!\n");
	return 0;
}

