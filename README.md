# Plugin

### 介绍
Plugin 仓主要承载 openGauss 的插件扩展能力，其中 dolphin 插件用于在 openGauss-server 基础上补充 B 库语法、函数、数据类型、类型转换等能力。

### docker 快速搭建运行环境及构建测试

> 前提：aarch64 机器，Docker ≥ 18.09.0，Docker 数据目录所在磁盘预留 ≥ 30G。
> 权限配置、seccomp 修复等一次性前置操作，见本仓库根目录 `Dockerfile` 头部注释【操作步骤】步骤一/步骤二。

> 注：仅进行 Docker 快速测试时，按本节步骤操作即可；贡献流程、开发须知和手动 check 用例说明可暂不阅读。首次搭建环境或遇到权限、seccomp 问题时，请同时查看 Dockerfile 头部注释。

1.  拉取本仓库，进入 Dockerfile 所在目录

2.  构建镜像并编译（整段复制执行，详见 Dockerfile 注释【操作步骤】步骤四）：

    ```bash
    docker rm -f og-build 2>/dev/null; \
    MIRROR=$(getent hosts mirrors.huaweicloud.com | awk '{print $1; exit}'); \
    OBS=$(getent hosts opengauss.obs.cn-south-1.myhuaweicloud.com | awk '{print $1; exit}'); \
    GIT=$(getent hosts gitcode.com | awk '{print $1; exit}'); \
    docker build --network host \
      --build-arg MIRROR_IP=$MIRROR \
      --build-arg OBS_IP=$OBS \
      --build-arg GITCODE_IP=$GIT \
      -t og-dolphin-env . && \
    docker run -d --name og-build --network host \
      --user omm og-dolphin-env sleep infinity && \
    docker exec og-build /opt/build.sh
    ```

    编译产物：容器内 `mppdb_temp_install/bin/gaussdb` 与
    `mppdb_temp_install/lib/postgresql/dolphin.so`；
    编译日志：容器内 `/workspace/verify.log`

3.  测试 UT 执行 S 组 check 用例即可（等价于 `make check p=38000 PART=S`）：

    ```bash
    docker exec og-build /opt/makecheck.sh
    ```
> 注意：Dockerfile 默认拉取 openGauss-server 与 Plugin 两个仓的 master
> 分支；若需验证本地修改，请自行调整 Dockerfile 中的仓库地址/分支，或
> 将本地 `contrib/dolphin` 拷入容器后重新编译。

### 参与贡献

1.  按照社区特性设计模板（ https://gitcode.com/opengauss/community/tree/master/design/template ）撰写特性设计说明书，并在plugin sig例会上评审通过
2.  代码编写/自测，提交代码review，修改检视意见
3.  如果需求涉及资料修改，同步提交资料PR（ https://gitcode.com/opengauss/docs ），dolphin相关的语法文档入口为 https://gitcode.com/opengauss/docs/tree/master/docs/zh/extension_reference ，对应官网页面： https://docs.opengauss.org/zh/docs/latest/docs/ExtensionReference/dolphin-Extension.html
4.  按照社区check-in模板（ https://gitcode.com/opengauss/community/tree/master/check-in/template ）撰写checkin说明书，并在plugin sig例会上评审通过
5.  代码合入
6.  按照社区QA测试报告模板（ https://gitcode.com/opengauss/QA/tree/master/Test_Delivery_Templates ）撰写openGauss XX版本XX特性测试报告模板.md，并在 QA sig例会上评审通过（QA sig例会请通过订阅QA SIG邮件列表获知信息： https://mailweb.opengauss.org/postorius/lists/qa.opengauss.org/ ）

### dolphin插件开发须知
1.  插件开发指南： https://opengauss.org/zh/blogs/ganyang/SQL%E5%BC%95%E6%93%8E%E6%8F%92%E4%BB%B6%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC.html
2.  修改如涉及文档，需要同步在docs仓提交文档修改，插件相关文档入口： https://gitcode.com/opengauss/docs/tree/master/docs/zh/extension_reference 。注意添加SQL语法时，需要增加必要的示例。
3.  新增/修改的代码需要使用宏 DOLPHIN 进行控制，方便后续回合openGauss-server仓代码时，区分哪些是插件修改的代码，哪些是内核修改的代码。修改的代码通过宏的IF/ELSE分支保留原始代码。主要控制 ```.h/.cpp``` 文件， ```.y``` 文件不太好使用宏控制，可以不处理。
4.  代码中涉及dolphin.b_compatibility_mode判断的地方，统一使用宏ENABLE_B_CMPT_MODE控制。
5.  涉及插件升级/回滚脚本修改的，应本地自验插件的升级/回滚流程，确保脚本正确，简单验证方式如下。 `2.0`，`3.0`是dolphin插件的版本，当前最新版本为 `3.0`，后续版本号升级的话，就分别改为 `3.0`，`4.0`，以此类推
```
alter system set upgrade_mode to 2;
select pg_sleep(2);
begin;
set isinplaceupgrade to on;
set dolphin.b_compatibility_mode = off;
alter extension dolphin update to '2.0';
alter extension dolphin update to '3.0';
reset dolphin.b_compatibility_mode;
abort;

alter system set upgrade_mode to 0;
```

### check用例编写规范

1. check用例默认使用的数据库为contrib_regression数据库，B兼容类型。编写用例时无需自己手动创建B类型数据库。
2. 建议通过schema的方式隔离不同用例间的结果影响。可参考现有用例的写法。
3. 单个用例执行时间不宜太长，建议不超过10s，超过的应当考虑优化用例或进行拆分。
4. 非必要不新增测试组，一个测试组可允许5~10个用例一起并行执行。
5. 对于SELECT语句强烈建议增加order by子句，保证SELECT语句查询结果稳定。

### dolphin 插件 check 用例执行

1. 拉取代码仓

   分别拉取 [openGauss-server仓](https://gitcode.com/opengauss/openGauss-server) 和 [Plugin仓](https://gitcode.com/opengauss/Plugin) 代码。

2. 准备 dolphin 插件目录

   将 Plugin 仓中 `contrib/dolphin` 目录复制到 openGauss-server 仓的 `contrib` 目录下，最终目录结构应为：

   ```text
   openGauss-server/contrib/dolphin
   ```

3. 编译 openGauss-server

   在 openGauss-server 仓根目录下编译项目代码，当前支持 `make` 和 `cmake` 两种编译方式，同时 release/debug/memcheck 版本均可以执行用例。为提高用例执行效率，推荐使用 cmake 编译 release 版本执行用例。

4. 执行 dolphin check 用例

   执行 Dolphin 的 S 组轻量 UT，对应 schedule 文件：

   ```text
   contrib/dolphin/parallel_schedule_dolphinS
   ```

   进入 `contrib/dolphin` 目录后执行：

   ```bash
   cd contrib/dolphin
   make check p=38000 PART=S
   ```

   其中，`p` 为数据库监听端口，可根据本地环境调整；工程项目执行时必须显式指定 `PART=S`，不要省略 `PART`。

5. 执行单个用例

   若只需执行 S 组中的单个用例，建议先备份 S schedule 文件：

   ```bash
   cp parallel_schedule_dolphinS parallel_schedule_dolphinS.bak
   ```

   然后修改 `parallel_schedule_dolphinS`，仅保留需要执行的用例，格式如下：

   ```text
   test: your_test_name
   ```

   修改完成后执行：

   ```bash
   make check p=38000 PART=S
   ```

   执行结束后，可根据需要恢复原 schedule 文件。
