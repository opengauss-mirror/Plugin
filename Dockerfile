# ============================================================
# openGauss dolphin Plugin 仓测试使用流程 (aarch64)
# 镜像包: https://download-opengauss.osinfra.cn/archive_test/tools/og-dolphin-env-202609.tar.gz
# 镜像内置: openEuler 24.03 + 全部编译依赖 + omm + 预置脚本
#   (/opt/prepare.sh /opt/build.sh /opt/makecheck.sh
#    /opt/initdb.sh /opt/smoke-plugin.sh /opt/gsql.sh)
#   不含代码与 binarylibs, 由容器运行时拉取(合规要求)
#   Plugin 仓流程: 双仓(server + plugin)并行拉取 + 拷 dolphin 到 server/contrib/
#   脚本默认指向 master 分支，测试其他分支请修改对应配置
# 前提: aarch64 + 磁盘>=30GB + docker 权限/seccomp + 本机可访问 gitcode.com 与 opengauss OBS
# ============================================================
#
# # 注: 严格遵守以下 7 步指令操作。
# 【七条命令, 依次执行, 全程无 docker build】
#
# 1. 清理旧容器与旧镜像(执行指令耗时约 5 秒)
# docker rm -f og-plugin 2>/dev/null; docker rmi -f og-dolphin-env:202609 2>/dev/null; echo CLEAN_DONE
# # 全新机器上两条都报"不存在", 属正常, 忽略即可
#
# 2. docker 下载并导入预置环境镜像(执行指令耗时约 30 秒)
# wget -qO- https://download-opengauss.osinfra.cn/archive_test/tools/og-dolphin-env-202609.tar.gz | docker load
#
# 3. 启动容器(执行指令耗时约 3 秒)
# docker run -d --name og-plugin --network host --user omm og-dolphin-env:202609 sleep infinity
#
# 4. 获取代码及三方库(执行指令耗时约 60 秒)
# docker exec og-plugin /opt/prepare.sh
#
# 5. 代码编译(执行指令耗时约 400 秒)
# docker exec og-plugin /opt/build.sh
#
# 6. 执行 UT 测试(执行指令耗时约 120 秒)
# docker exec -e PART=S -e PORT=38000 og-plugin /opt/makecheck.sh
#
# 7. 冒烟测试(执行指令耗时约 90 秒)
# # 流程: 预加载 dolphin ---> 初始化实例 ---> 起库 ---> 建 B 库
# #   test_b ---> CREATE EXTENSION   ---> MySQL 兼容语法
# #   (auto_increment / show databases / SHOW TABLES) ---> 删 B 库
# #   (表随库删, 零残留) ---> 停库清理实例
# # 冒烟用独立临时目录 /workspace/smoke_node, 不碰开发实例
# # /workspace/node1(开发中的库/数据不受冒烟影响)
# docker exec -e PORT=5432 og-plugin /opt/smoke-plugin.sh
# # 看到 SMOKE_PLUGIN_OK(PORT=5432) 即通过
#
# ============================================================
# # 重新初始化并启动(删旧数据!): docker exec -e PORT=5432 og-plugin /opt/initdb.sh
# # 启动实例: docker exec og-plugin /opt/initdb.sh start
# # 停止实例: docker exec og-plugin /opt/initdb.sh stop
# # 直连数据库调试(默认进 postgres): docker exec -e PORT=5432 -it og-plugin /opt/gsql.sh
# ============================================================
# # 日志(非必要无需关注): docker exec og-plugin tail -f /workspace/verify.log
# # 调试(非必要无需关注): docker exec -it og-plugin /bin/bash
# ============================================================
# 下方 FROM 仅声明本镜像自身
ARG BASE_IMAGE=og-dolphin-env:202609
FROM ${BASE_IMAGE}

WORKDIR /workspace
CMD ["/bin/bash"]
