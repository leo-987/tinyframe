#include <gtest/gtest.h>

extern "C"
{
#include "hash.h"
}

class HashTableTest : public ::testing::Test {
protected:
	HashTableTest()
	{
		ht = hash_table_create(100);
		key1 = 1;
		value1 = 1;
		key2 = 11;
		value2 = 2;
		key3 = 101;
		value3 = 3;
		key4 = 2;
		value4 = 4;
	}
	
	virtual void SetUp()
	{
		hash_table_insert(ht, key1, &value1);
		hash_table_insert(ht, key2, &value2);
		hash_table_insert(ht, key3, &value3);
		hash_table_insert(ht, key4, &value4);
	}

	virtual void TearDown()
	{
		hash_table_free(ht);
	}
	
	hash_table *ht;
	int key1;
	int key2;
	int key3;
	int key4;
	int value1;
	int value2;
	int value3;
	int value4;
};

TEST_F(HashTableTest, HashTableFind)
{
	ASSERT_TRUE(hash_table_find(ht, key1));
	ASSERT_EQ(*(int *)hash_table_find(ht, key1)->value, value1);
	
	ASSERT_TRUE(hash_table_find(ht, key2));
	ASSERT_EQ(*(int *)hash_table_find(ht, key2)->value, value2);
	
	ASSERT_TRUE(hash_table_find(ht, key3));
	ASSERT_EQ(*(int *)hash_table_find(ht, key3)->value, value3);
	
	ASSERT_TRUE(hash_table_find(ht, key4));
	ASSERT_EQ(*(int *)hash_table_find(ht, key4)->value, value4);
	
	ASSERT_FALSE(hash_table_find(ht, 123));

	ASSERT_EQ(hash_table_remove(ht, key1), 0);
	ASSERT_EQ(hash_table_remove(ht, key2), 0);
	ASSERT_EQ(hash_table_remove(ht, 123), -1);

	ASSERT_FALSE(hash_table_find(ht, key1));
	ASSERT_FALSE(hash_table_find(ht, key2));

	hash_table_insert(ht, 123, &value4);
	
	ASSERT_TRUE(hash_table_find(ht, key3));
	ASSERT_EQ(*(int *)hash_table_find(ht, key3)->value, value3);
	
	ASSERT_TRUE(hash_table_find(ht, key4));
	ASSERT_EQ(*(int *)hash_table_find(ht, key4)->value, value4);
	
	ASSERT_TRUE(hash_table_find(ht, 123));
	ASSERT_EQ(*(int *)hash_table_find(ht, 123)->value, value4);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

