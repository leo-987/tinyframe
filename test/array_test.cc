#include <gtest/gtest.h>

extern "C"
{
#include "array.h"
}

class ArrayTest : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		char *p;

		a = array_create(5, sizeof(char));
		p = (char *)array_push(a);
		*p = 'a';
		p = (char *)array_push(a);
		*p = 'b';
		p = (char *)array_push(a);
		*p = 'c';
		p = (char *)array_push(a);
		*p = 'd';
	}
	
	virtual void TearDown()
	{
		array_free(a);
	}

	array *a;
};

TEST_F(ArrayTest, ArrayFunc)
{
	ASSERT_EQ(a->size, sizeof(char));
	ASSERT_EQ(a->nelts, 4);
	ASSERT_EQ(a->capacity, 5);
	ASSERT_EQ(((char *)a->elts)[0], 'a');
	ASSERT_EQ(((char *)a->elts)[1], 'b');
	ASSERT_EQ(((char *)a->elts)[2], 'c');
	ASSERT_EQ(((char *)a->elts)[3], 'd');

	char *p = (char *)array_push_n(a, 4);
	p[0] = 'x';
	p[1] = 'y';
	p[2] = 'z';
	p[3] = 'w';

	ASSERT_EQ(a->nelts, 8);
	ASSERT_EQ(a->capacity, 10);
	ASSERT_EQ(((char *)a->elts)[0], 'a');
	ASSERT_EQ(((char *)a->elts)[1], 'b');
	ASSERT_EQ(((char *)a->elts)[2], 'c');
	ASSERT_EQ(((char *)a->elts)[3], 'd');
	ASSERT_EQ(((char *)a->elts)[4], 'x');
	ASSERT_EQ(((char *)a->elts)[5], 'y');
	ASSERT_EQ(((char *)a->elts)[6], 'z');
	ASSERT_EQ(((char *)a->elts)[7], 'w');
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


