# **orafce编译说明文档**

+ 下载openGauss社区master分支的最新代码 (本文档撰写的最新代码版本是master分支,时间:2022-06-15)

  URL如下

  `
  https://gitee.com/opengauss/openGauss-server.git
  `

+ 下载opengauss社区master分支源码，假设源码文件名为openGauss-server，并设计环境变量CODE_BASE
``` bash
  export CODE_BASE='绝对路径'/openGauss-server
```

+ 编译、并安装opengauss，并生成安装目录，此处假设安装目录为OpenGauss_install（绝对路径）。

+ 设置环境变量

  ``` bash
  export PATH=OpenGauss_install/bin:$PATH
  ```

+ 下载适配openGauss的orafce代码，代码文件名为orafce

+ 进入插件源码文件orafce里，执行make && make install，当显示

  ``` bash
  install-sh -c -m 755  orafce.so
  install-sh -c -m 644 ./orafce.control
  install-sh -c -m 644 ./orafce--3.17.sql
  install-sh -c -m 644 ./README.asciidoc ./COPYRIGHT.orafce ./INSTALL.orafce
  ```
  等字样则表示编译成功，且已经安装完毕

__注意：如果编译有报错，则应停留在编译阶段报错信息处。__

# **在数据库中安装orafce插件**

+ 在openGauss安装目录OpenGauss_install/bin下，初始化数据库目录(gs_initdb)，并启动数据库服务(gs_ctl)

+ 用gsql工具进入客户端，执行sql
  ```sql
  CREATE EXTENSION orafce;
  ```
  当返回`CREATE EXTENSION`，则表示安装成功。

+ 在客户端执行\dn，返回如图显示，则表示orafce的schema和function已经安装完毕。此图仅做参考

  ![模式展示](./INSTALL-picture/orafce%E6%A8%A1%E5%BC%8F%E5%B1%95%E7%A4%BA.png)

