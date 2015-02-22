#ifndef __HASH_H__
#define __HASH_H__

typedef int ElementType;

typedef struct ListNode *Position;
struct ListNode
{
    ElementType Element;
    Position Next;
};

typedef struct HashTbl *HashTable;
struct HashTbl
{
    int TableSize;
    Position *TheLists;  /* 指针数组,存放不同的桶 */
};


#endif

