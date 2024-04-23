<!-- 感谢您提交Pull Reqeust -->

<!-- 提交说明: 请按照以下的模板提交PR，下列内容均为必填项。如果未补充对应内容，不允许提交PR。 -->
【标题】(请简要描述下实现的内容)

【实现内容】:

【根因分析】:

【实现方案】:

【关联需求或issue】:

【开发自验报告】:
1. 请附上自验结果(内容或者截图)
2. 是否可以添加fastcheck测试用例，如是，请补充fastcheck用例
3. 是否涉及资料修改，如是，在docs仓库补充资料
4. 是否考虑支撑升级和在线扩容等扩展场景。涉及升级/回滚脚本修改的场景请按如下方式测试验证升级/回滚是否正常，并提供自验截图。注意 `2.0`， `3.0` 为dolphin插件版本，请根据实际情况选择。
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
5. 是否考虑异常场景/并发场景/前向兼容/性能场景
6. 是否对其他模块产生影响

【其他说明】: