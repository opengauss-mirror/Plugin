#!/bin/sh

set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
output_dir=${1:-"$script_dir/mysql_sslcert"}
tmp_dir=$(mktemp -d "${TMPDIR:-/tmp}/dolphin_sslcert.XXXXXX")
openssl_cnf="$tmp_dir/openssl.cnf"
# Avoid loading openGauss-bundled libssl/libcrypto into the host openssl command.
openssl_ld_library_path=${OPENSSL_LD_LIBRARY_PATH:-}

if [ -n "${OPENSSL_BIN:-}" ]; then
    openssl_bin=$OPENSSL_BIN
elif ! openssl_bin=$(command -v openssl); then
    echo "openssl executable not found" >&2
    exit 1
fi

cleanup()
{
    rm -rf "$tmp_dir"
}

run_openssl()
{
    if ! env LD_LIBRARY_PATH="$openssl_ld_library_path" "$openssl_bin" "$@" \
        >"$tmp_dir/openssl.log" 2>&1; then
        cat "$tmp_dir/openssl.log" >&2
        return 1
    fi
}

trap cleanup EXIT HUP INT TERM

mkdir -p "$output_dir"

cat > "$openssl_cnf" <<'EOF'
[ req ]
distinguished_name = req_distinguished_name
prompt = no

[ req_distinguished_name ]

[ ca_ext ]
basicConstraints = critical, CA:TRUE
keyUsage = critical, keyCertSign, cRLSign
subjectKeyIdentifier = hash
authorityKeyIdentifier = keyid:always

[ server_ext ]
basicConstraints = critical, CA:FALSE
keyUsage = critical, digitalSignature, keyEncipherment
extendedKeyUsage = serverAuth
subjectKeyIdentifier = hash
authorityKeyIdentifier = keyid, issuer
subjectAltName = @server_alt_names

[ server_alt_names ]
DNS.1 = server
DNS.2 = localhost
IP.1 = 127.0.0.1
IP.2 = ::1

[ client_ext ]
basicConstraints = critical, CA:FALSE
keyUsage = critical, digitalSignature, keyEncipherment
extendedKeyUsage = clientAuth
subjectKeyIdentifier = hash
authorityKeyIdentifier = keyid, issuer
EOF

run_openssl genrsa -out "$tmp_dir/ca.key" 2048
run_openssl req -new -x509 -days 3650 -key "$tmp_dir/ca.key" -out "$tmp_dir/cacert.pem" \
    -subj "/C=CN/ST=Beijing/O=openGauss/OU=dolphin-test/CN=dolphin-test-root" -config "$openssl_cnf" \
    -extensions ca_ext -batch

run_openssl genrsa -out "$tmp_dir/server.key" 2048
run_openssl req -new -key "$tmp_dir/server.key" -out "$tmp_dir/server.csr" \
    -subj "/C=CN/ST=Beijing/O=openGauss/OU=dolphin-test/CN=server" -config "$openssl_cnf" -batch
run_openssl x509 -req -days 3650 -in "$tmp_dir/server.csr" -CA "$tmp_dir/cacert.pem" \
    -CAkey "$tmp_dir/ca.key" -CAcreateserial -out "$tmp_dir/server.crt" \
    -extfile "$openssl_cnf" -extensions server_ext

run_openssl genrsa -out "$tmp_dir/client.key" 2048
run_openssl req -new -key "$tmp_dir/client.key" -out "$tmp_dir/client.csr" \
    -subj "/C=CN/ST=Beijing/O=openGauss/OU=dolphin-test/CN=client" -config "$openssl_cnf" -batch
run_openssl x509 -req -days 3650 -in "$tmp_dir/client.csr" -CA "$tmp_dir/cacert.pem" \
    -CAkey "$tmp_dir/ca.key" -CAserial "$tmp_dir/cacert.srl" -out "$tmp_dir/client.crt" \
    -extfile "$openssl_cnf" -extensions client_ext

install -m 0644 "$tmp_dir/cacert.pem" "$output_dir/cacert.pem"
install -m 0644 "$tmp_dir/server.crt" "$output_dir/server.crt"
install -m 0600 "$tmp_dir/server.key" "$output_dir/server.key"
install -m 0644 "$tmp_dir/client.crt" "$output_dir/client.crt"
install -m 0600 "$tmp_dir/client.key" "$output_dir/client.key"
rm -f "$output_dir/client.key.pk8" "$output_dir/client.key.cipher" "$output_dir/client.key.rand" \
    "$output_dir/server.key.cipher" "$output_dir/server.key.rand"
