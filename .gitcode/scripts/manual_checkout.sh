#!/usr/bin/env bash
set -euo pipefail

# CI scripts always come from opengauss/Plugin master, not the PR target branch.
repo="${GITCODE_REPO:-opengauss/Plugin}"
case "${repo}" in
  */*) ;;
  *)
    echo "Invalid repository: ${repo}" >&2
    exit 1
    ;;
esac

server="${ATOMGIT_SERVER_URL:-}"
if [[ -z "${server}" ]]; then
  server="${GITCODE_SERVER_URL:-}"
fi
if [[ -z "${server}" ]]; then
  server="https://gitcode.com"
fi
server="${server%/}"

dest="${PWD}/master-gitcode-scripts"
rm -rf "${dest}"
mkdir -p "${dest}"
dir="$(mktemp -d)"
cleanup() {
  rm -rf "${dir}"
}
trap cleanup EXIT

echo "Fetch CI scripts from ${server}/${repo}.git refs/heads/master"
(
  cd "${dir}"
  git init -q
  git remote add origin "${server}/${repo}.git"
  timeout 300 git fetch --depth=1 --force origin refs/heads/master
  git show "FETCH_HEAD:.gitcode/scripts/resolve_gitcode_env.sh" > "${dest}/resolve_gitcode_env.sh"
  git show "FETCH_HEAD:.gitcode/scripts/plugin_compile.sh" > "${dest}/plugin_compile.sh"
)
