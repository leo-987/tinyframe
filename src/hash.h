#ifndef __HASH_H__
#define __HASH_H__

/* 散列表,采用开链法解决冲突 */

typedef struct hash_node_t hash_node;

typedef struct hash_node_t{
	int key;
    void *value;
    hash_node *next;
}hash_node;

typedef struct hash_table_t{
    int size;				/* 桶个数 */
    hash_node **buckets;	/* 指针数组,存放不同的桶,桶不能动态增加 */
}hash_table;


hash_table *hash_table_create(unsigned int size);
int hash(int key, unsigned int size);
hash_node *hash_table_find(hash_table *ht, int key);
int hash_table_insert(hash_table *ht, int key, void *value);
int hash_table_remove(hash_table *ht, int key);
void hash_table_print(hash_table *ht);
void hash_table_free(hash_table *ht);

#endif

