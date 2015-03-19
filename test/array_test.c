#include <stdio.h>
#include <assert.h>

#include "array.h"

void array_test()
{
	char *p;
	array *a;
	
	a = array_create(5, sizeof(char));
	p = (char *)array_push(a);
	*p = 'a';
	p = (char *)array_push(a);
	*p = 'b';
	p = (char *)array_push(a);
	*p = 'c';
	p = (char *)array_push(a);
	*p = 'd';

	assert(a->size == sizeof(char));
	assert(a->nelts == 4);
	assert(a->capacity == 5);
	assert(((char *)a->elts)[0] == 'a');
	assert(((char *)a->elts)[1] == 'b');
	assert(((char *)a->elts)[2] == 'c');
	assert(((char *)a->elts)[3] == 'd');

	p = (char *)array_push_n(a, 4);
	p[0] = 'x';
	p[1] = 'y';
	p[2] = 'z';
	p[3] = 'w';

	assert(a->nelts == 8);
	assert(a->capacity == 10);
	assert(((char *)a->elts)[0] == 'a');
	assert(((char *)a->elts)[1] == 'b');
	assert(((char *)a->elts)[2] == 'c');
	assert(((char *)a->elts)[3] == 'd');
	assert(((char *)a->elts)[4] == 'x');
	assert(((char *)a->elts)[5] == 'y');
	assert(((char *)a->elts)[6] == 'z');
	assert(((char *)a->elts)[7] == 'w');

	array_free(a);
}

int main()
{
	array_test();
	printf("array test OK!\n");
	return 0;
}


