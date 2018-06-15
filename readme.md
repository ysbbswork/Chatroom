# Chartroom介绍

一个跨平台的聊天服务器Demo，用户可以通过任意的Telnet接入chatroomserver，接入聊天室，简单的用户管理和telnet协议的实现，使用select()实现IO复用，模块分离，便于扩展。

整个server使用泛型编程，Telnet是作为协议独立实现，你也可以实现自己的协议。

预留了Handlers接口类，可以按模板实现自己的处理器来实现个性化的处理接入信息。

聊天室只是一个简单的Demo，由SCLogon和SCChat两个定制的Handler实现。

# 详细介绍组成

## 每个接入都是一个connecton

由简易封装的socket派生出datasocket和listensocket，再进一步完善datasocket生成加强版的connection，

之后所有工作都是在面向connection在做工作，比如如何解析connection 传来的数据包即模板的translate，比如给connection添加删除处理器，处理器在接收到协议解析出来的command之后做什么回应，给connection回什么语句，就是各个connectionHandler做的事情。

可以将每个Connection作为一个接入用户处理，receive的数据利用protocol翻译出command语句交由Handler去处理。

![](http://ww1.sinaimg.cn/large/005HFdfGgy1fsc8fk2drsj30pt06taa4.jpg)



# Manger&Handlers

![](http://ww1.sinaimg.cn/large/005HFdfGgy1fsc8fse0kqj30hb08bt8t.jpg)



而完全实例化的协议（如Telnet）就是一个完整的Handler，而且这个作为协议的handler是m_protocol，固定在所有的connection里（`protocol m_protocol;`），每次connection receive的数据都要通过m_protocol这个实例化的handler进行翻译处理。

并且将翻译出来的每一句（即每条command）立刻发送给connection当前的handler进行handle()处理：

```
p_conn.Handler()->Handle(string(m_buffer, m_buffersize));
```

## Connection&Handlers

连接和连接处理器之间通过协议交互：

![](http://ww1.sinaimg.cn/large/005HFdfGgy1fsc8goiuobj30rn080t8t.jpg)

而在我们的Telnet命令只有string这一种，所以就简化成：

![](http://ww1.sinaimg.cn/large/005HFdfGgy1fsc8gu2gxgj30rn0800st.jpg)