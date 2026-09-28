#!/usr/bin/env bash
set -euo pipefail

repo="${GITCODE_REPO:-opengauss/Plugin}"
token="${PR_LABEL_TOKEN:-${ATOMGIT_TOKEN:-}}"
pr_iid="${GITCODE_PR_IID:-}"
action="${1:-}"

if [[ -z "${action}" ]]; then
  echo "Usage: $0 <running|success|failed>"
  exit 2
fi

if [[ -z "${pr_iid}" ]]; then
  echo "No pull request IID found; skip GitCode PR labels."
  exit 0
fi

if [[ -z "${token}" ]]; then
  echo "PR_LABEL_TOKEN is empty; skip GitCode PR labels."
  exit 0
fi

api_base="https://gitcode.com/api/v5/repos/${repo}/pulls/${pr_iid}/labels"

delete_label() {
  local label="$1"
  curl -fsS -X DELETE "${api_base}/${label}" \
    -H "Authorization: Bearer ${token}" \
    -H "Content-Type: application/json;charset=UTF-8" >/dev/null || true
}

add_label() {
  local label="$1"
  curl -fsS -X POST "${api_base}" \
    -H "Authorization: Bearer ${token}" \
    -H "Content-Type: application/json;charset=UTF-8" \
    -d "[\"${label}\"]" >/dev/null
}

case "${action}" in
  running)
    delete_label ci-pipeline-success
    delete_label ci-pipeline-failed
    add_label ci-pipeline-running
    ;;
  success)
    delete_label ci-pipeline-running
    delete_label ci-pipeline-failed
    add_label ci-pipeline-success
    ;;
  failed)
    delete_label ci-pipeline-running
    add_label ci-pipeline-failed
    ;;
  *)
    echo "Unknown label action: ${action}"
    exit 2
    ;;
esac
