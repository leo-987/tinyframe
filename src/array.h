#ifndef __ARRAY_H__
#define __ARRAY_H__

/* 可变长数组,参考Nginx源码 */

typedef struct {
	/* 指向数组首地址 */
    void *elts;

	/* 已有元素个数 */
    unsigned int nelts;

	/* 每个元素字节大小 */
    size_t size;

	/* 容量 */
    unsigned int capacity;
}array;


array *array_create(unsigned int n, size_t size);
void array_free(array *a);
void *array_push(array *a);
void *array_push_n(array *a, unsigned int n);
int array_copy(array *from, char *to, int len);
void array_clear(array *a);

#endif
