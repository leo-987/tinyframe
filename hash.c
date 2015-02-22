#include "hash.h"

/* 散列表,参考<<数据结构与算法分析>> */

HashTable InitializeTable(int TableSize)
{
    HashTable ht;
    int i;

    ht = malloc(sizeof(struct HashTbl));
    if (ht == NULL)
        return -1;

    ht->TableSize = TableSize;
    ht->TheLists = calloc(TableSize, sizeof(Position));
    if (ht->TheLists == NULL)
        return -1;

    return ht;
}

// 散列函数
int Hash(ElementType Key, int TableSize)
{
    return Key % TableSize;
}

Position Find(HashTable H, ElementType Key)
{
    Position p;

    p = H->TheLists[Hash(Key, H->TableSize)];
    while (p != NULL && p->Element != Key)  // 这里只能对数值型key进行比较
        p = p->Next;

    return p;
}

// 键值不允许重复
void Insert(HashTable H, ElementType Key)
{
    Position *p;    // 如果不使用这个双重指针，那么需要计算多次散列函数
    Position tmp;

    if (Find(H, Key) == NULL)    // 元素不存在
    {   // 插入链表头部
        p = &(H->TheLists[Hash(Key, H->TableSize)]);
        tmp = malloc(sizeof(struct ListNode));
        tmp->Element = Key;
        tmp->Next = *p;
        *p = tmp;
    }
}

/*

int main(void)
{
    HashTable table;
    Position p;

    table = InitializeTable(100);

    Insert(table, 21);
    Insert(table, 24);
    Insert(table, 43);
    Insert(table, 35);

    p = Find(table, 43);
    if (p != NULL)
        printf("Find it!\n");
    else
        printf("Not found!\n");

    return 0;
}

*/
