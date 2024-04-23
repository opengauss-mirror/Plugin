# Plugin

#### 介绍
{**以下是 Gitee 平台说明，您可以替换此简介**
Gitee 是 OSCHINA 推出的基于 Git 的代码托管平台（同时支持 SVN）。专为开发者提供稳定、高效、安全的云端软件开发协作平台
无论是个人、团队、或是企业，都能够用 Gitee 实现代码托管、项目管理、协作开发。企业项目请看 [https://gitee.com/enterprises](https://gitee.com/enterprises)}

#### 软件架构
软件架构说明


#### 安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明

1.  xxxx
2.  xxxx
3.  xxxx

#### 参与贡献

1.  按照社区特性设计模板（ https://gitee.com/opengauss/community/tree/master/design/template ）撰写特性设计说明书，并在plugin sig例会上评审通过
2.  代码编写/自测，提交代码review，修改检视意见
3.  如果需求涉及资料修改，同步提交资料PR（ https://gitee.com/opengauss/docs ），dolphin相关的语法文档入口为 https://gitee.com/opengauss/docs/blob/master/content/zh/docs/ExtensionReference/dolphin-Extension.md ，对应官网页面： https://docs.opengauss.org/zh/docs/latest/docs/ExtensionReference/dolphin-Extension.html
4.  按照社区check-in模板（ https://gitee.com/opengauss/community/tree/master/check-in/template ）撰写checkin说明书，并在plugin sig例会上评审通过
5.  代码合入
6.  按照社区QA测试报告模板（ https://gitee.com/opengauss/QA/tree/master/Test_Delivery_Templates ）撰写openGauss XX版本XX特性测试报告模板.md，并在 QA sig例会上评审通过（QA sig例会请通过订阅QA SIG邮件列表获知信息： https://mailweb.opengauss.org/postorius/lists/qa.opengauss.org/ ）

### dolphin插件开发须知
1.  插件开发指南： https://opengauss.org/zh/blogs/ganyang/SQL%E5%BC%95%E6%93%8E%E6%8F%92%E4%BB%B6%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC.html
2.  修改如涉及文档，需要同步在docs仓提交文档修改，插件相关文档入口： https://gitee.com/opengauss/docs/tree/master/content/zh/docs/ExtensionReference/dolphin-Extension.md 。注意添加SQL语法时，需要增加必要的示例。
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

#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
