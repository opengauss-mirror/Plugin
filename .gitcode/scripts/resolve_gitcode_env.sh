#!/usr/bin/env bash
set -euo pipefail

normalize_branch_name() {
  local branch="$1"
  branch="${branch//$'\r'/}"
  branch="${branch#refs/heads/}"
  printf '%s' "${branch}"
}

push_branch_from_ref() {
  local ref_name="$1"
  ref_name="${ref_name//$'\r'/}"
  case "${ref_name}" in
    refs/heads/*)
      printf '%s' "${ref_name#refs/heads/}"
      ;;
    refs/*|"")
      ;;
    *)
      printf '%s' "${ref_name}"
      ;;
  esac
}

parse_push_branch() {
  local file="$1"
  local value=""
  if command -v python3 >/dev/null 2>&1; then
    value="$(python3 - "${file}" <<'PY' || true
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    data = json.load(handle)
ref = data.get("ref") if isinstance(data, dict) else ""
prefix = "refs/heads/"
if isinstance(ref, str) and ref.startswith(prefix):
    print(ref[len(prefix):], end="")
PY
)"
  fi
  if [[ -n "${value}" ]]; then
    printf '%s' "${value}"
    return
  fi
  value="$(grep -oE '"ref"[[:space:]]*:[[:space:]]*"refs/heads/[^"]+"' "${file}" | head -n 1 | sed -n 's/.*"refs\/heads\/\([^"]*\)".*/\1/p' || true)"
  printf '%s' "${value}"
}

json_first_string() {
  local file="$1"
  local key="$2"
  sed -n "s/.*\"${key}\"[[:space:]]*:[[:space:]]*\"\\([^\"]*\\)\".*/\\1/p" "${file}" | head -n 1 || true
}

parse_event_target_branch() {
  local file="$1"
  local value=""
  if command -v python3 >/dev/null 2>&1; then
    value="$(python3 - "${file}" <<'PY' || true
import json
import sys

def text(value):
    if isinstance(value, str):
        value = value.strip()
        if value:
            return value
    return ""

def from_obj(obj):
    if not isinstance(obj, dict):
        return ""
    base = obj.get("base")
    if isinstance(base, dict):
        ref = text(base.get("ref"))
        if ref:
            return ref
    for key in ("target_branch", "base_ref"):
        ref = text(obj.get(key))
        if ref:
            return ref
    return ""

with open(sys.argv[1], encoding="utf-8") as handle:
    data = json.load(handle)

found = ""
if isinstance(data, dict):
    for key in ("pull_request", "merge_request", "object_attributes"):
        found = from_obj(data.get(key))
        if found:
            break
    if not found:
        found = from_obj(data)
print(found, end="")
PY
)"
  fi
  if [[ -n "${value}" ]]; then
    printf '%s' "${value}"
    return
  fi
  value="$(json_first_string "${file}" target_branch)"
  if [[ -z "${value}" ]]; then
    value="$(tr '\n' ' ' < "${file}" | sed -n 's/.*"base"[[:space:]]*:[[:space:]]*{[^}]*"ref"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n 1 || true)"
  fi
  if [[ -z "${value}" ]]; then
    value="$(json_first_string "${file}" base_ref)"
  fi
  printf '%s' "${value}"
}

event_name="${ATOMGIT_EVENT_NAME:-${GITCODE_EVENT_NAME:-}}"
event_file="${ATOMGIT_EVENT_PATH:-${GITCODE_EVENT_PATH:-}}"
pr_iid="${gitcodePullRequestIid:-${GITCODE_PR_IID:-}}"
target_branch="${gitcodeTargetBranch:-${GITCODE_TARGET_BRANCH:-${ATOMGIT_BASE_REF:-${GITCODE_BASE_REF:-}}}}"
after_sha="${gitcodeAfterCommitSha:-${GITCODE_AFTER_COMMIT_SHA:-${GITCODE_SHA:-}}}"
merge_ref="${gitcodeMergeRef:-${GITCODE_MERGE_REF:-}}"
target_branch="$(normalize_branch_name "${target_branch}")"

is_pull_request=0
case "${event_name}" in
  pull_request|pull_request_target|pull_request_comment|pr_comment)
    is_pull_request=1
    ;;
esac
if [[ -n "${pr_iid}" ]]; then
  is_pull_request=1
fi
if [[ "${is_pull_request}" -eq 0 && -n "${event_file}" && -f "${event_file}" ]] && grep -Eq '"pull_request"|"merge_request"' "${event_file}"; then
  is_pull_request=1
fi

if [[ "${is_pull_request}" -eq 1 && -n "${event_file}" && -f "${event_file}" ]]; then
  if [[ -z "${pr_iid}" ]]; then
    pr_iid="$(sed -n 's/.*"iid"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "${event_file}" | head -n 1 || true)"
  fi
  if [[ -z "${pr_iid}" ]]; then
    pr_iid="$(sed -n 's/.*"number"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "${event_file}" | head -n 1 || true)"
  fi
  if [[ -z "${target_branch}" ]]; then
    target_branch="$(normalize_branch_name "$(parse_event_target_branch "${event_file}")")"
  fi
fi

if [[ "${is_pull_request}" -eq 1 ]]; then
  if [[ -z "${target_branch}" ]]; then
    echo "ERROR: cannot determine the pull request target branch." >&2
    echo "Provide GITCODE_TARGET_BRANCH, ATOMGIT_BASE_REF, or pull_request.base.ref / target_branch in the event payload." >&2
    exit 1
  fi
elif [[ "${event_name}" == "push" ]]; then
  target_branch="$(push_branch_from_ref "${ATOMGIT_REF:-${GITCODE_REF:-}}")"
  if [[ -z "${target_branch}" && -n "${event_file}" && -f "${event_file}" ]]; then
    target_branch="$(parse_push_branch "${event_file}")"
  fi
  if [[ -z "${target_branch}" ]]; then
    echo "ERROR: cannot determine the pushed branch." >&2
    echo "Provide ATOMGIT_REF or GITCODE_REF as refs/heads/<branch>." >&2
    exit 1
  fi
else
  target_branch=master
fi

if [[ -n "${pr_iid}" && -z "${merge_ref}" ]]; then
  merge_ref="refs/merge-requests/${pr_iid}/merge"
fi

export GITCODE_PR_IID="${pr_iid}"
export GITCODE_TARGET_BRANCH="${target_branch}"
export GITCODE_AFTER_COMMIT_SHA="${after_sha}"
export GITCODE_MERGE_REF="${merge_ref}"

env_file="${ATOMGIT_ENV:-${GITCODE_ENV:-}}"
if [[ -n "${env_file}" ]]; then
  {
    echo "GITCODE_PR_IID=${GITCODE_PR_IID}"
    echo "GITCODE_TARGET_BRANCH=${GITCODE_TARGET_BRANCH}"
    echo "GITCODE_AFTER_COMMIT_SHA=${GITCODE_AFTER_COMMIT_SHA}"
    echo "GITCODE_MERGE_REF=${GITCODE_MERGE_REF}"
  } >> "${env_file}"
fi

echo "Resolved GitCode env:"
echo "  GITCODE_PR_IID=${GITCODE_PR_IID}"
echo "  GITCODE_TARGET_BRANCH=${GITCODE_TARGET_BRANCH}"
echo "  GITCODE_AFTER_COMMIT_SHA=${GITCODE_AFTER_COMMIT_SHA}"
echo "  GITCODE_MERGE_REF=${GITCODE_MERGE_REF}"
