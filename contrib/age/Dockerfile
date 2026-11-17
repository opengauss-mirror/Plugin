#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


# Keep PostgreSQL 11, using a Debian release with available build dependencies.
FROM postgres:11.22-bookworm@sha256:af1f69f8c01df9b75f6a3594455c167a36490c4b8f263309c0d7a091320c7a70

# Use HTTP for bootstrap; ca-certificates is installed below. APT verifies signatures.
# Optional: --build-arg DEBIAN_MIRROR=http://mirrors.huaweicloud.com
# Optional: --build-arg PGDG_MIRROR=http://mirrors.aliyun.com/postgresql/repos/apt
ARG DEBIAN_MIRROR=http://deb.debian.org
ARG PGDG_MIRROR=http://apt.postgresql.org/pub/repos/apt
RUN sed -i "s|http://deb.debian.org|${DEBIAN_MIRROR%/}|g" /etc/apt/sources.list.d/debian.sources && \
  sed -i "s|http://apt.postgresql.org/pub/repos/apt|${PGDG_MIRROR%/}|g" /etc/apt/sources.list.d/pgdg.list && \
  apt-get update && \
  apt-get install --assume-yes --no-install-recommends --no-install-suggests \
  bison \
  build-essential \
  ca-certificates \
  flex \
  git \
  postgresql-plpython3-11 \
  postgresql-server-dev-11 && \
  rm -rf /var/lib/apt/lists/*

# Apache AGE 1.5.0 for PostgreSQL 11 (release/PG11/1.5.0).
RUN git init /age && \
  git -C /age remote add origin https://github.com/apache/age.git && \
  git -C /age fetch --depth 1 origin 95bf818d4d83be704bbd33be43d266b90d234e55 && \
  git -C /age checkout --detach FETCH_HEAD && \
  make -C /age install && \
  rm -rf /age

COPY docker-entrypoint-initdb.d/00-create-extension-age.sql /docker-entrypoint-initdb.d/00-create-extension-age.sql

CMD ["postgres", "-c", "shared_preload_libraries=age"]
