## 概况
TinyFrame是一个基于Linux的小型网络服务器框架程序，采用epoll+非阻塞的单线程模型，可以处理网络和超时两类事件。  

## 目录结构
src: 框架代码  
test: 测试代码  

## 编译方法
cd src  
make生成静态库文件lib.a  
cd ../test  
make编译测试代码  

## 对象说明
event: 事件对象，封装了文件描述符和该事件就绪时需要调用的回调函数。  
connection: 代表一个已建立的TCP连接，封装了两端的地址和读写缓冲区。  
listener: 监听对象，负责监听socket，接收到新连接请求后负责建立connection对象。  
server: 服务器对象，一个server对应一个本地套接字地址对应，管理listener和多个具有相同服务端地址的connection。  
timer: 定时器对象，封装了超时时间和超时回调函数，可单次触发或重复触发。  
server_manager: 总管理对象，程序只能有一个该对象，负责管理所有event，server，timer对象。  

## 参考
Nginx  
Libevent  
Muduo  
