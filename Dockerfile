# ============================================================
# openGauss dolphin Plugin 仓测试使用流程 (aarch64)
# 镜像包: https://download-opengauss.osinfra.cn/archive_test/tools/og-dolphin-env-202609.tar.gz
# 镜像内置: openEuler 24.03 + 全部编译依赖 + omm + 三脚本
#   (/opt/prepare.sh /opt/build.sh /opt/makecheck.sh)
#   不含代码与 binarylibs, 由容器运行时拉取(合规要求)
#   Plugin 仓流程: 双仓并行拉取 + 拷 dolphin 到 contrib/
# 前提: aarch64 + 磁盘>=30GB + docker 权限/seccomp 已修
#   + 本机可访问 gitcode.com 与 opengauss OBS
# ============================================================
#
# 注: 采用 AI 测试该代码仓 / 进行 Docker 快速搭建环境及测试，严格遵守以下 6 步指令操作，禁止执行除 “调整环境配置” 和 “执行 Skill 中提及” 之外的其他操作。
# 【六条命令, 依次执行, 全程无 docker build】
#
# 1. 清理旧容器与旧镜像(约 3 秒)
# docker rm -f og-plugin 2>/dev/null; docker rmi -f og-dolphin-env:202609 2>/dev/null; echo CLEAN_DONE
# # 全新机器上两条都报"不存在", 属正常, 忽略即可
#
# 2. 下载并导入镜像(约 40 秒)
# wget -qO- https://download-opengauss.osinfra.cn/archive_test/tools/og-dolphin-env-202609.tar.gz | docker load
#
# 3. 启动容器(约 3 秒)
# docker run -d --name og-plugin --network host --user omm og-dolphin-env:202609 sleep infinity
#
# 4. 拉代码 + 拉 binarylibs + 复制 dolphin(约 60 秒)
# docker exec og-plugin /opt/prepare.sh
#
# 5. 编译(约 400 秒)
# docker exec og-plugin /opt/build.sh
#
# 6. UT: dolphin 下 make check(默认 PART=S, PORT=38000)
# # 耗时约 120 秒
# docker exec og-plugin /opt/makecheck.sh
# # 完整指令(若端口占用可切换端口): docker exec -e PART=S -e PORT=32000 og-plugin /opt/makecheck.sh
#
# # 日志(非必要无需关注): docker exec og-plugin tail -f /workspace/verify.log
# # 调试(非必要无需关注): docker exec -it og-plugin /bin/bash
# ============================================================
# 下方 FROM 仅声明本镜像自身(三脚本已内置, 无需任何构建层)
ARG BASE_IMAGE=og-dolphin-env:202609
FROM ${BASE_IMAGE}

WORKDIR /workspace
CMD ["/bin/bash"]
