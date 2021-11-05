# db_b_parser

## 编译
1. 下载代码，将db_b_parser源码拷贝到openGauss源码contrib目录下。
2. 编译完openGauss后，单独编译插件：make && make install。
## 使用
1. 连接数据库，通过SQL命令加载插件。通过回显判断是否创建成功。
```sql
create extension db_b_parser;
```
2. 设置GUC参数启动对应解析器。
```sql
set enable_csutom_parser = on;
```
3. 测试兼容语句。
```
select rand();
```
4. 约束。

不支持带插件升级。
## 备注
数据库重启后需要重新加载插件。否则重启后插件无法功能。当前仅支持配置文件设置重启的方式加载。修改postgresql.conf的参数实现功能。
```sql
shared_preload_libraries = 'db_b_parser' 
```