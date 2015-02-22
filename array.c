#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "debug.h"

/* 初始化一个已存在的动态数组 */
static int array_init(array *array, unsigned int n, size_t size)
{
    array->nelts = 0;
    array->size = size;
    array->capacity= n;
    array->elts = malloc(n * size);
    if (array->elts == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
        return -1;
    }
    return 0;
}

/* 创建动态数组,大小为n*size字节 */
array *array_create(unsigned int n, size_t size)
{
    array *a = malloc(sizeof(array));
    if (a == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
    }

    if (array_init(a, n, size) < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
    }

    return a;
}

/* 销毁动态数组,与ngx_array_create配对使用 */
void array_free(array *a)
{
    free(a->elts);
	free(a);
}

/* 添加一个元素,返回新元素地址 */
void *array_push(array *a)
{
    void        *elt, *new;
    size_t       size;

    if (a->nelts == a->capacity)
	{
		/* 扩容 */
        size = a->size * a->capacity;

        new = malloc(2 * size);
        if (new == NULL)
		{
			debug_ret("file: %s, line: %d", __FILE__, __LINE__);
            return NULL;
        }

        memcpy(new, a->elts, size);
		free(a->elts);
        a->elts = new;
        a->capacity *= 2;
    }

    elt = (unsigned char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}

/* 添加n个新元素,返回第一个新元素的地址 */
void *array_push_n(array *a, unsigned int n)
{
    void        *elt, *new;
    unsigned int   nalloc;

    if (a->nelts + n > a->capacity)
	{
        /* 扩容 */
        nalloc = 2 * ((n >= a->capacity) ? n : a->capacity);

        new = malloc(nalloc * a->size);
        if (new == NULL)
		{
			debug_ret("file: %s, line: %d", __FILE__, __LINE__);
            return NULL;
        }

        memcpy(new, a->elts, a->nelts * a->size);
		free(a->elts);
        a->elts = new;
        a->capacity = nalloc;
    }

    elt = (unsigned char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}

/* 将from中的载荷复制到数组to中,不自动在末尾追加'\0' */
int array_copy(array *from, char *to, int len)
{
	int n;
	if (from->nelts * from->size < len)
		n = from->nelts * from->size;
	else
		n = len;
	
	memcpy(to, from->elts, n);
	
	/* 输入buffer清空 */
	from->nelts = 0;
	return n;
}

