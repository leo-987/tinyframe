#include <gtest/gtest.h>
#include <arpa/inet.h>

extern "C"
{
#include "inetaddr.h"
}

TEST(ExitDeathTest, InetaddrTest)
{
	ASSERT_EXIT(addr_create(NULL, 123), testing::ExitedWithCode(1),  "");
	ASSERT_EXIT(addr_create("192.", 123), testing::ExitedWithCode(1),  "");
	ASSERT_EXIT(addr_create("192.168", 123), testing::ExitedWithCode(1),  "");

	char buff[20];
	inet_address addr1 = addr_create("192.168.1.123", 2015);
	
	ASSERT_EQ(ntohs(addr1.addr.sin_port), 2015);
	ASSERT_STREQ(inet_ntop(AF_INET, &addr1.addr.sin_addr, buff, sizeof(buff)), "192.168.1.123");

	inet_address addr2 = addr_create("any", 2016);
	ASSERT_EQ(ntohs(addr2.addr.sin_port), 2016);
	ASSERT_EQ(ntohl(addr2.addr.sin_addr.s_addr), INADDR_ANY);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

