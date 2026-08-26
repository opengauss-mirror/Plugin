#!/usr/bin/env bash
set -euo pipefail

echo "gitcodePullRequestIid: ${GITCODE_PR_IID:-}"
echo "gitcodeAfterCommitSha: ${GITCODE_AFTER_COMMIT_SHA:-}"
echo "gitcodeRef: refs/merge-requests/${GITCODE_PR_IID:-}/merge"
echo "gitcodeMergeRef: ${GITCODE_MERGE_REF:-}"

git config --global core.compression 0

gitcodeTargetBranch="${GITCODE_TARGET_BRANCH:-master}"
if [[ -z "${gitcodeTargetBranch}" ]]; then
  gitcodeTargetBranch=master
fi
echo "gitcodeTargetBranch: ${gitcodeTargetBranch}"

plugin_repo="${PLUGIN_REPO:-https://gitcode.com/opengauss/Plugin.git}"
server_repo="${SERVER_REPO:-https://gitcode.com/opengauss/openGauss-server.git}"
WORKSPACE="${WORKSPACE_ROOT:?WORKSPACE_ROOT is required}"
private_build_script="${PRIVATE_BUILD_SCRIPT:-/home/PrivateBuild_tools/Private_Main_Plugin.sh}"

export JAVA_HOME="${JAVA_HOME:-/usr/local/jdk-17}"
export PATH="${JAVA_HOME}/bin:${PATH}"
export WORKSPACE
export gitcodeTargetBranch

java -version

download_source_from_gitcode() {
  local repo="$1"
  local branch="$2"
  local target_dir="$3"
  local a=0
  local flag=0

  echo "download source [${repo}], branch [${branch}]"

  while [[ "${a}" -lt 3 ]]; do
    echo "${a}"
    rm -rf "${WORKSPACE:?}/${target_dir}"
    if timeout 5m git clone "${repo}" -b "${branch}" "${WORKSPACE}/${target_dir}"; then
      flag=1
      break
    fi
    a=$((a + 1))
    sleep 10
  done

  if [[ "${flag}" == 0 ]]; then
    echo "clone ${target_dir} failed!"
    exit 1
  fi
}

download_source() {
  cd "${WORKSPACE}"
  download_source_from_gitcode "${server_repo}" "${gitcodeTargetBranch}" openGauss

  echo "server: current commit:"
  cd "${WORKSPACE}/openGauss"
  git show
  sed -i "s#@if test -d contrib/timescaledb#@if test -d contrib/1234213#g" GNUmakefile.in

  cd "${WORKSPACE}"
  download_source_from_gitcode "${plugin_repo}" "${gitcodeTargetBranch}" Plugin
}

merge_source_code() {
  cd "${WORKSPACE}/Plugin"

  git config --global user.email "${GIT_AUTHOR_EMAIL:-gitcode-actions@users.noreply.gitcode.com}"
  git config --global user.name "${GIT_AUTHOR_NAME:-gitcode-actions}"
  git rev-parse --is-inside-work-tree
  git config remote.origin.url "${plugin_repo}"

  if [[ -n "${GITCODE_PR_IID:-}" ]]; then
    git fetch origin "refs/merge-requests/${GITCODE_PR_IID}/head:refs/merge-requests/${GITCODE_PR_IID}/head"
    git merge --no-verify "refs/merge-requests/${GITCODE_PR_IID}/head" --no-edit
  else
    echo "No GITCODE_PR_IID found; skip PR head merge."
  fi
}

download_binarylibs() {
  cd "${WORKSPACE}"
  rm -rf openGauss-third_party_binarylibs*

  echo "downing binarylibs--------------------"
  local binarylibs_name_centos_x86="openGauss-third_party_binarylibs_Centos7.6_x86_64"
  local binarylibs_name_openeuler_x86="openGauss-third_party_binarylibs_openEuler_x86_64"
  local binarylibs_name_openeuler_arm="openGauss-third_party_binarylibs_openEuler_arm"
  local os_name
  local cpu_arc
  local binarylibs_file=""

  os_name="$(source /etc/os-release && echo "${ID}")"
  cpu_arc="$(uname -p)"

  if [[ "${os_name}" == "centos" && "${cpu_arc}" == "x86_64" ]]; then
    binarylibs_file="${binarylibs_name_centos_x86}"
  elif [[ "${os_name}" == "euleros" && "${cpu_arc}" == "aarch64" ]]; then
    binarylibs_file="${binarylibs_name_openeuler_arm}"
  elif [[ "${os_name}" == "openEuler" && "${cpu_arc}" == "aarch64" ]]; then
    binarylibs_file="${binarylibs_name_openeuler_arm}"
  elif [[ "${os_name}" == "openEuler" && "${cpu_arc}" == "x86_64" ]]; then
    binarylibs_file="${binarylibs_name_openeuler_x86}"
  elif [[ "${os_name}" == "asianux" && "${cpu_arc}" == "x86_64" ]]; then
    binarylibs_file="${binarylibs_name_openeuler_x86}"
  elif [[ "${os_name}" == "asianux" && "${cpu_arc}" == "aarch64" ]]; then
    binarylibs_file="${binarylibs_name_openeuler_arm}"
  else
    echo "Not support this platfrom: ${os_name}_${cpu_arc}"
    exit 1
  fi

  if [[ "${gitcodeTargetBranch}" == "master" && -z "${binarylibs_file}" ]]; then
    echo "Not found binarylibs of platfrom: ${os_name}_${cpu_arc}"
    exit 1
  fi

  echo "Build openGauss user third-party_binarylibs: ${binarylibs_file}"

  set -e
  if [[ "${gitcodeTargetBranch}" == "2.0.0" ]]; then
    wget https://opengauss.obs.cn-south-1.myhuaweicloud.com/2.0.0/openGauss-third_party_binarylibs.tar.gz -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "3.0.0" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/3.0.0/binarylibs/${binarylibs_file}-3.0.3.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "5.0.0" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/5.0.0/binarylibs/${binarylibs_file}.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "5.1.0" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/5.1.0/binarylibs/gcc10.3/${binarylibs_file}.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "6.0.0" || "${gitcodeTargetBranch}" == "6.0.5" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/6.0.0/binarylibs/gcc10.3/${binarylibs_file}.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "master" || "${gitcodeTargetBranch}" == "dev" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/latest/binarylibs/gcc10.3/${binarylibs_file}.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  elif [[ "${gitcodeTargetBranch}" == "7.0.0-RC3" ]]; then
    wget "https://opengauss.obs.cn-south-1.myhuaweicloud.com/latest/binarylibs/gcc10.3/${binarylibs_file}.tar.gz" -O "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -q
  else
    echo "ERROR: ${gitcodeTargetBranch} branch not found"
    exit 1
  fi
  set +e

  mkdir -p "${WORKSPACE}/openGauss-third_party_binarylibs"
  tar -zxf "${WORKSPACE}/openGauss-third_party_binarylibs.tar.gz" -C openGauss-third_party_binarylibs --strip-components 1
}

plugin_compile() {
  cd "${WORKSPACE}/openGauss/build/script"
  chmod +x build_opengauss.sh

  if [[ "${gitcodeTargetBranch}" == "2.0.0" ]]; then
    sed -i 's/make -sj 8/make -sj20 /g' mpp_package.sh
  fi

  chmod -R 755 /usr1
  chmod -R 755 /home/PrivateBuild_tools
  sh "${private_build_script}" 0 "${gitcodeTargetBranch}"
}

main() {
  download_source
  merge_source_code
  download_binarylibs
  plugin_compile
}

main
