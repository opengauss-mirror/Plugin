# ============================================================
# openGauss dolphin 插件构建环境 (aarch64 / openEuler 24.03)
# 最低要求 Docker 版本: 18.09.0
# ============================================================
#
# 【磁盘要求】(实测数据, 必须先检查再动手)
#   Docker 数据目录(默认 /var/lib/docker)所在分区需预留:
#     仅编译: 峰值 ~20G(镜像 7.2G + 容器编译产物 ~13G)
#     编译+UT: 峰值 ~27G(make check 临时数据再加 ~5G)
#   统一预留 30G(实测峰值 27G; 若需反复跑 UT 并留日志
#   则适当上浮)。检查: df -h /var/lib/docker
#   不够必须先清空间, 否则中途报
#   "No space left on device", 编译前功尽弃。
#   清理手段(root, 均不影响运行中的容器):
#     docker builder prune -f     # build cache
#     docker image prune -f       # dangling 镜像
#     docker container prune -f   # 已停止的容器
#   长期空间不足: 把 data-root 迁到大盘分区
#     (/etc/docker/daemon.json 写 {"data-root": "/路径"},
#      停 docker 后 rsync -aHAX 迁移再启动)
#
# 【注意点】
# 1. 老 Docker 不支持 BuildKit heredoc:
#    RUN <<'EOS' 会报 unknown instruction: SET,
#    本文件全用传统 RUN 写法规避
# 2. 老 seccomp 不认识 glibc 2.38 的 clone3:
#    容器内起线程一律失败(curl 解析域名就挂),
#    必须按【操作步骤-步骤二】根治
# 3. /etc/hosts 由 Docker 按容器挂载, 不跨 RUN 保留:
#    故每个联网 RUN 开头都重新注入三个 IP(辅助免 DNS)
#
# ============================================================
#
# 【操作步骤】
#
# 步骤一: docker 权限(一次性)
#   执行本文件的用户必须在 docker 组里, 或直接用 root
#   否则报: permission denied while trying to connect to
#           the Docker daemon socket
#   加组指令(以 test_user 为例):
#     sudo usermod -aG docker test_user
#     # 加组后必须重新登录 SSH 才生效(exit 重连)
#   验证: docker ps 不报权限错误即可
#
# 步骤二: 线程自检与修复(一次性, 按需, root, 任何机器通用)
#   先自检(镜像 tag 与下方 FROM 保持一致):
#     docker run --rm hub.oepkgs.net/openeuler/openeuler:24.03-lts \
#       python3 -c "import threading;t=threading.Thread(\
#       target=lambda:None);t.start();print('THREAD_OK')"
#   出 THREAD_OK → 本机不需要修, 直接跳到步骤四
#   失败 → 宿主机 Docker 的 seccomp 拦了线程, root 修复
#     (以下为 18.09 + 4.19 内核实测结论, 按序执行):
#     a. 写全放行 profile(直接用简版, 不要带 clone3 条目:
#        实测 18.09 加载含 clone3 的 profile 不生效,
#        容器仍被默认策略拦截; 4.19 内核本无 clone3,
#        glibc 自动回退老 clone, 全放行即可根治):
#          echo '{"defaultAction": "SCMP_ACT_ALLOW"}' \
#            > /etc/docker/seccomp.json
#     b. 用 systemd 旗标加载 profile(实测 18.09 不读取
#        daemon.json 里的 "seccomp-profile" 键, 必须改
#        ExecStart; 先看原始形态再改写):
#          systemctl cat docker | grep ExecStart
#          mkdir -p /etc/systemd/system/docker.service.d
#          cat > /etc/systemd/system/docker.service.d/seccomp.conf \
#            <<'EOF'
#          [Service]
#          ExecStart=
#          ExecStart=/usr/bin/dockerd \
#            --seccomp-profile=/etc/docker/seccomp.json \
#            $OPTIONS $DOCKER_STORAGE_OPTIONS \
#            $DOCKER_NETWORK_OPTIONS $INSECURE_REGISTRY
#          EOF
#          # 若重启 docker 后拉镜像变慢/失败, 说明本机
#          # OPTIONS 里有额外配置, 按上面看到的原始
#          # ExecStart 补全旗标
#     c. 清理旧尝试留在 daemon.json 里的 seccomp-profile
#        键(键与旗标并存 dockerd 会拒绝启动):
#          daemon.json 只含该键的: rm -f /etc/docker/daemon.json
#          还有其它配置的: vi 手工删该键所在行(注意上一行逗号)
#     d. systemctl daemon-reload && systemctl restart docker
#     e. 验证旗标真挂上了(输出须含 --seccomp-profile):
#          ps -ef | grep dockerd | grep -v grep
#     f. 重跑自检, 出 THREAD_OK 即修复完成
#     注: 全放行 = 容器内所有系统调用放行, 安全隔离变弱,
#         构建机可接受, 不要拿它跑不可信镜像
#
# 步骤三: 校验文件完整性(防传输损坏)
#   grep -n "$(printf '\x60')" Dockerfile    # 应无任何输出
#   md5sum Dockerfile         # 记录下来, 便于前后比对
#
# 步骤四: 编译(一条命令自足, 整段复制粘贴执行)
#   docker rm -f og-build 2>/dev/null; \
#   MIRROR=$(getent hosts mirrors.huaweicloud.com \
#     | awk '{print $1; exit}')
#   OBS=$(getent hosts opengauss.obs.cn-south-1.myhuaweicloud.com \
#     | awk '{print $1; exit}')
#   GIT=$(getent hosts gitcode.com | awk '{print $1; exit}')
#   docker build --network host \
#     --build-arg MIRROR_IP=$MIRROR \
#     --build-arg OBS_IP=$OBS \
#     --build-arg GITCODE_IP=$GIT \
#     -t og-dolphin-env . && \
#   docker run -d --name og-build --network host \
#     --user omm og-dolphin-env sleep infinity && \
#   docker exec og-build /opt/build.sh
#
#   编译日志: 容器内 /workspace/verify.log
#
# 步骤五: UT 测试(依赖步骤四成功)
#   docker exec og-build /opt/makecheck.sh
#
# 步骤六: 进容器调试
#   docker exec -it og-build /bin/bash
# ============================================================
ARG OE_TAG=24.03-lts
FROM hub.oepkgs.net/openeuler/openeuler:${OE_TAG}

ENV LANG=C.UTF-8 LC_ALL=C.UTF-8

ARG DNF_MIRROR=mirrors.huaweicloud.com/openeuler

# 步骤四经 --build-arg 传入的三个 IP:
# 在每个联网 RUN 开头写入 /etc/hosts 作辅助
# (让 git 等直接走 files 免 DNS; curl 能走通
# 靠的是步骤二的 seccomp 修复)
ARG MIRROR_IP=
ARG OBS_IP=
ARG GITCODE_IP=

# 前置层: 华为云单源 + git/curl
RUN H=/etc/hosts \
  && { [ -n "$MIRROR_IP" ] \
    && echo "$MIRROR_IP mirrors.huaweicloud.com" >> $H || true; } \
  && { [ -n "$OBS_IP" ] \
    && echo "$OBS_IP opengauss.obs.cn-south-1.myhuaweicloud.com" \
    >> $H || true; } \
  && { [ -n "$GITCODE_IP" ] \
    && echo "$GITCODE_IP gitcode.com" >> $H || true; } \
  && M="https://$DNF_MIRROR" \
  && R=/etc/yum.repos.d/openEuler.repo \
  && printf '[OS]\nname=OS\n' > $R \
  && printf 'baseurl=%s/openEuler-24.03-LTS/OS/aarch64/\n' "$M" >> $R \
  && printf 'enabled=1\ngpgcheck=0\n\n' >> $R \
  && printf '[everything]\nname=everything\n' >> $R \
  && printf 'baseurl=%s/openEuler-24.03-LTS/' "$M" >> $R \
  && printf 'everything/aarch64/\n' >> $R \
  && printf 'enabled=1\ngpgcheck=0\n\n' >> $R \
  && printf '[EPOL]\nname=EPOL\n' >> $R \
  && printf 'baseurl=%s/openEuler-24.03-LTS/EPOL/main/aarch64/\n' \
    "$M" >> $R \
  && printf 'enabled=1\ngpgcheck=0\n' >> $R \
  && dnf clean all \
  && dnf makecache \
  && dnf install -y git curl tar gzip findutils

# 并行层: dnf 依赖 ∥ binarylibs 877MB ∥ 双仓 clone
RUN set -e; RC=0; \
  H=/etc/hosts; \
  { [ -n "$MIRROR_IP" ] \
    && echo "$MIRROR_IP mirrors.huaweicloud.com" >> $H || true; }; \
  { [ -n "$OBS_IP" ] \
    && echo "$OBS_IP opengauss.obs.cn-south-1.myhuaweicloud.com" \
    >> $H || true; }; \
  { [ -n "$GITCODE_IP" ] \
    && echo "$GITCODE_IP gitcode.com" >> $H || true; }; \
  BINLIB_URL="https://opengauss.obs.cn-south-1.myhuaweicloud.com"; \
  BINLIB_URL="$BINLIB_URL/latest/binarylibs/gcc10.3"; \
  BINLIB_URL="$BINLIB_URL/openGauss-third_party_binarylibs"; \
  BINLIB_URL="${BINLIB_URL}_openEuler_2403_arm.tar.gz"; \
  PKGS="make cmake python3 python3-pip hostname diffutils which"; \
  PKGS="$PKGS procps-ng libaio-devel flex bison ncurses-devel"; \
  PKGS="$PKGS glibc-devel patch readline-devel libedit-devel"; \
  PKGS="$PKGS libxml2-devel lz4-devel numactl-devel unixODBC-devel"; \
  PKGS="$PKGS java-1.8.0-openjdk-devel openblas-devel binutils"; \
  PKGS="$PKGS openssl openssl-devel libnsl dkms"; \
  ( dnf install -y --disablerepo="update*" \
      --setopt="*.skip_if_unavailable=True" $PKGS; \
    command -v lsb_release >/dev/null 2>&1 \
      || dnf install -y openeuler-lsb || true; \
    if ! command -v lsb_release >/dev/null 2>&1; then \
      SRC=$(find /usr/lib /usr/libexec /usr/sbin /usr/share \
        -name lsb_release -type f 2>/dev/null | head -n1); \
      [ -n "$SRC" ] && ln -sf "$SRC" /usr/bin/lsb_release; \
    fi; \
    command -v lsb_release >/dev/null 2>&1 \
      || { echo "!! lsb_release not available"; exit 1; }; \
    dnf clean all; \
    useradd -m omm ) & DNF=$!; \
  curl -fsSL --retry 5 --retry-delay 2 \
    -o /tmp/binarylibs.tar.gz "$BINLIB_URL" & DL=$!; \
  mkdir -p /workspace; cd /workspace; \
  git clone --depth 1 --branch master \
    https://gitcode.com/opengauss/openGauss-server.git & P1=$!; \
  git clone --depth 1 --branch master \
    https://gitcode.com/opengauss/Plugin.git & P2=$!; \
  wait $DNF && echo "JOB dnf OK" \
    || { echo "JOB dnf FAILED"; RC=1; }; \
  wait $DL && echo "JOB binlib OK" \
    || { echo "JOB binlib FAILED"; RC=1; }; \
  wait $P1 && echo "JOB server OK" \
    || { echo "JOB server FAILED"; RC=1; }; \
  wait $P2 && echo "JOB plugin OK" \
    || { echo "JOB plugin FAILED"; RC=1; }; \
  [ "$RC" -eq 0 ]; \
  mkdir -p /binarylibs; \
  tar -xzf /tmp/binarylibs.tar.gz -C /binarylibs \
    --strip-components 1; \
  rm -f /tmp/binarylibs.tar.gz; \
  chown -R omm:omm /binarylibs; \
  rm -rf /workspace/openGauss-server/contrib/dolphin; \
  cp -r /workspace/Plugin/contrib/dolphin \
    /workspace/openGauss-server/contrib/dolphin; \
  chown -R omm:omm /workspace; \
  echo "PARALLEL_LAYER_DONE"

# 编译脚本(以 omm 运行)
RUN printf '#!/bin/bash\n' > /opt/build.sh \
  && printf 'set -o pipefail\n' >> /opt/build.sh \
  && printf 'exec > >(tee -a /workspace/verify.log) 2>&1\n' \
    >> /opt/build.sh \
  && printf 'cd /workspace/openGauss-server\n' >> /opt/build.sh \
  && printf 'sh build.sh -m release -3rd /binarylibs --cmake \\\n' \
    >> /opt/build.sh \
  && printf '  || { echo "!! build failed"; exit 1; }\n' \
    >> /opt/build.sh \
  && printf 'ls -lh mppdb_temp_install/bin/gaussdb \\\n' \
    >> /opt/build.sh \
  && printf '  mppdb_temp_install/lib/postgresql/dolphin.so\n' \
    >> /opt/build.sh \
  && chmod +x /opt/build.sh && chown omm:omm /opt/build.sh

# dolphin 回归测试脚本(以 omm 运行)
RUN printf '#!/bin/bash\n' > /opt/makecheck.sh \
  && printf 'set -o pipefail\n' >> /opt/makecheck.sh \
  && printf 'exec > >(tee -a /workspace/verify.log) 2>&1\n' \
    >> /opt/makecheck.sh \
  && printf 'SRV=/workspace/openGauss-server/mppdb_temp_install\n' \
    >> /opt/makecheck.sh \
  && printf 'export PATH=$SRV/bin:$PATH\n' >> /opt/makecheck.sh \
  && printf 'export LD_LIBRARY_PATH=$SRV/lib:$LD_LIBRARY_PATH\n' \
    >> /opt/makecheck.sh \
  && printf 'cd /workspace/openGauss-server/contrib/dolphin\n' \
    >> /opt/makecheck.sh \
  && printf 'make check p=38000 PART=A\n' >> /opt/makecheck.sh \
  && chmod +x /opt/makecheck.sh \
  && chown omm:omm /opt/makecheck.sh

WORKDIR /workspace
CMD ["/bin/bash"]
