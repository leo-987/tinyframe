## 概况
TinyFrame是一个基于Linux的小型网络服务器框架程序，采用epoll+非阻塞的多线程模型（one loop per thread），可以
处理网络和超时两类事件。线程数数目固定，可通过src/config.h头文件进行配置，只有一个线程负责accept新连接，其它线程处理请求。  

## 目录结构
src: 框架代码  
test: 测试代码  
example: 简单例程  

## 编译方法
cd src  
make生成静态库文件lib.a  
cd ../test  
make编译单元测试代码  
cd ../example  
make编译例程   

## 参考
Nginx  
Libevent  
Muduo  

## 压力测试
分别用TinyFrame，Muduo，Libevent编写服务器程序，代码在test/stress_xxx.c，Apache Bench（AB）程序作为客户端。
客户端发出请求，服务器响应字符串“test”，以此来测量服务器并发量。  

测试环境：  
1. 服务器配置：Intel(R) Core2 Duo CPU T6670 @ 2.2GHz 2.2GHz，2G RAM，虚拟机Ubuntu系统。  
2. 客户端配置：Pentium(R) Dual-Core CPU E6700 @ 3.2GHz 3.2GHz，2G RAM，Ubuntu系统。  
3. 两台机器处于同一快速以太网（100Mbps）中。  
4. 总请求数为100000，并发连接数从1到5000逐渐递增，测试指标为Requests per second。  

第一组测TinyFrame和Muduo使用双线程（一个负责监听新连接，一个负责处理请求），Libevent使用单线程。    
<pre><code>
Concurrency Level    TinyFrame    Muduo    Libevent  
    1                 1758.39	 1769.85    1724.46  
    5                 5957.29	 5974.21	5814.68  
    10                8961.47	 8972.27	8617.23  
    50                11325.49	 10675.03	10396.68  
    100               11170.79	 10137.41	10277  
    500               7140.82	 6877.62	9976.18  
    1000              7195.91	 7090.04	10003.78  
    5000              8041.07	 6180.22	6768.83  
</code></pre>
![Alt Text](https://github.com/oyld/tinyframe/raw/master/test/report/1.png)  


第二组测TinyFrame和Muduo都使用五个线程（一个负责监听新连接，四个负责处理请求）。  
<pre><code>
Concurrency Level    TinyFrame    Muduo    
    1                 1773.21	  1782.6  
    5                 5666.31	  5400.44  
    10                7454.84	  7196.96  
    50                11206.06	  11262.41  
    100               11716.09	  11602.08  
    500               11639.04	  11734.11  
    1000              11599.98	  11270.84  
    5000              9184.61	  9638.34  
</code></pre>
![Alt Text](https://github.com/oyld/tinyframe/raw/master/test/report/2.png)  
