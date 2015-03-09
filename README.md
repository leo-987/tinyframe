## 概况
TinyFrame是一个基于Linux的小型网络服务器框架程序，采用epoll+非阻塞的单线程模型，可以处理网络和超时两类事件。  

## 目录结构
src: 框架代码  
test: 测试代码  

## 编译方法
cd src  
make生成静态库文件lib.a  
cd ../test  
make编译单元测试代码  
cd ../example  
make编译例程  

## 对象说明
event: 事件对象，封装了文件描述符和该事件就绪时需要调用的回调函数。  
connection: 代表一个已建立的TCP连接，封装了两端的地址和读写缓冲区。  
listener: 监听对象，负责监听socket，接收到新连接请求后负责建立connection对象。  
server: 服务器对象，一个server对应一个本地套接字地址，管理listener和多个具有相同服务端地址的connection。  
timer: 定时器对象，封装了超时时间和超时回调函数，可单次触发或重复触发。  
server_manager: 总管理对象，程序只能有一个该对象，负责管理所有event，server，timer对象。  

## 参考
Nginx  
Libevent  
Muduo  

## 压力测试
用TinyFrame编写Web服务器，代码在test/pressure.c，Apache Bench（AB）程序作为客户端。客户端发出请求，服务器响应字符串“test”，以此来测量服务器并发量。

测试环境：  
1. 服务器配置：Pentium(R) Dual-Core CPU E6700 @ 3.2GHz 3.2GHz，2G RAM，Ubuntu系统。  
2. 客户端配置：Intel(R) Core2 Duo CPU T6670 @ 2.2GHz 2.2GHz，2G RAM，虚拟机Ubuntu系统。  
3. 两台机器处于同一快速以太网（100Mbps）中。  

总请求数为100000，并发连接数从1到1000逐渐递增，测试结果如下：  
<pre><code>
Concurrency Level    Requests per second  
    1                       1667.90  
    10                      4592.89  
    50                      7230.34  
    100                     6617.45  
    200                     6440.19  
    400                     5002.37  
    600                     4572.24  
    800                     4195.10  
    1000                    5540.50  
</code></pre>

![Alt Text](https://github.com/oyld/tinyframe/raw/master/test/pic/pressure.png)