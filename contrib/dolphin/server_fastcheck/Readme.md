# 整体说明
为了验证dolphin插件对于openGauss-server原有用例的影响，将openGauss-server仓的fastcheck_single用例复制一份到此，在此文件夹下，通过以下命令执行。

```shell
make check p=xxx
```

测试会自动安装一个兼容性为B的数据库并执行相关用例。

# 用例维护方式
待补充