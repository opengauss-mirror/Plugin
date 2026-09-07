#!/usr/bin/env bash

set -euo pipefail

action=${1:?missing setup action}
fixture_source_directory=${2:?missing fixture source directory}
gsql_binary=${3:?missing gsql binary}
server_port=${4:?missing server port}

data_directory=$(
    "$gsql_binary" -X -A -t -p "$server_port" -d postgres \
        -c "SHOW data_directory"
)

if [[ -z "$data_directory" || "$data_directory" == "/" ]]; then
    echo "invalid database data directory: '$data_directory'" >&2
    exit 1
fi

fixture_directory="$data_directory/age_load"
backup_directory="$data_directory/.age_load.regression-backup"
fixture_marker="$fixture_directory/.age-regression-fixture"

restore_existing_directory() {
    if [[ -e "$fixture_directory" ]]; then
        if [[ ! -f "$fixture_marker" ]]; then
            echo "refusing to remove unmarked AGE loader fixture directory" >&2
            exit 1
        fi
        rm -rf -- "$fixture_directory"
    fi

    if [[ -e "$backup_directory" ]]; then
        mv -- "$backup_directory" "$fixture_directory"
    fi
}

case "$action" in
    setup)
        if [[ -e "$backup_directory" ]]; then
            restore_existing_directory
        fi

        if [[ -e "$fixture_directory" ]]; then
            mv -- "$fixture_directory" "$backup_directory"
        fi

        staging_directory="$data_directory/.age_load.regression-staging.$$"
        restore_on_failure() {
            rm -rf -- "$staging_directory"
            if [[ ! -e "$fixture_directory" && -e "$backup_directory" ]]; then
                mv -- "$backup_directory" "$fixture_directory"
            fi
        }
        trap restore_on_failure EXIT

        mkdir -- "$staging_directory"
        cp -R -- "$fixture_source_directory"/. "$staging_directory"/
        touch -- "$staging_directory/.age-regression-fixture"
        ln -s -- ../postgresql.conf "$staging_directory/escaped.csv"
        mv -- "$staging_directory" "$fixture_directory"
        trap - EXIT
        ;;
    cleanup)
        restore_existing_directory
        ;;
    *)
        echo "unknown setup action: '$action'" >&2
        exit 1
        ;;
esac
