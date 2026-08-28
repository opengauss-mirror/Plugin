#!/usr/bin/env bash
set -euo pipefail

workspace="$PWD"
repo="${ATOMGIT_REPOSITORY:-}"
if [[ -z "${repo}" ]]; then
  repo="${GITCODE_REPOSITORY:-}"
fi
if [[ -z "${repo}" || "${repo}" != */* ]]; then
  repo="${GITCODE_REPO:-opengauss/Plugin}"
fi

server="${ATOMGIT_SERVER_URL:-}"
if [[ -z "${server}" ]]; then
  server="${GITCODE_SERVER_URL:-}"
fi
if [[ -z "${server}" ]]; then
  server="https://gitcode.com"
fi

repo_url="${server%/}/${repo}.git"
ref="${ATOMGIT_REF:-}"
if [[ -z "${ref}" ]]; then
  ref="${GITCODE_REF:-}"
fi
if [[ -z "${ref}" ]]; then
  ref="refs/heads/master"
fi

sha="${ATOMGIT_SHA:-}"
if [[ -z "${sha}" ]]; then
  sha="${GITCODE_SHA:-}"
fi
if [[ -z "${sha}" ]]; then
  sha="FETCH_HEAD"
fi

echo "Manual checkout ${repo_url} ${ref} ${sha}"

mkdir -p "${workspace}"
cd "${workspace}"
if [[ ! -d .git ]]; then
  git init
fi
if git remote | grep -qx origin; then
  git remote set-url origin "${repo_url}"
else
  git remote add origin "${repo_url}"
fi
git fetch --depth=1 origin "${ref}"
git checkout -f "${sha}" || git checkout -f FETCH_HEAD
