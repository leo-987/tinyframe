#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Logging.h>

#include <string>

std::string response = "HTTP/1.1 200 0K\r\nConnection:keep-alive\r\nContent-length:4\r\n\r\ntest";

void onMessage(const muduo::net::TcpConnectionPtr& conn,
               muduo::net::Buffer* buf,
               muduo::Timestamp receiveTime)
{
	buf->retrieveAllAsString();
	conn->send(response);
}

int main(int argc, char* argv[])
{
	muduo::Logger::setLogLevel(muduo::Logger::FATAL);
  
	muduo::net::InetAddress listenAddr(2016);
	muduo::net::EventLoop loop;

	muduo::net::TcpServer server(&loop, listenAddr, "EchoServer");
	server.setMessageCallback(onMessage);
	if (argc > 1)
	{
		server.setThreadNum(atoi(argv[1]));
	}
	server.start();

	loop.loop();
}


