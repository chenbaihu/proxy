proxy
=====

A Socks5 proxy.

目前不支持以域名的方式连接

不支持身份验证

不支持UDP和BIND命令

编译须知：

请先安装glog

然后

cmake . -DCMAKE_BUILD_TYPE=Debug

make install
