# AGE parser fuzzing

This integration fuzzes the AGE Go driver's lexer and parser using the current
repository sources. It covers scalar values, nested collections, graph values,
escapes and malformed input. It does not require a running database or building
openGauss-server.

Only the Go parser is covered; this is not coverage of Dolphin, the server, or
the entire Plugin repository. Inputs are bounded to 4096 bytes. Expected syntax
diagnostics are silenced, while panics and crashes remain failures. No CI jobs
or Git hooks are added.

## Run locally

With Go 1.18 or later, from the repository root:

```bash
cd contrib/age/drivers/golang
go test -mod=mod ./parser -run '^$' -fuzz '^FuzzAgeParser$' -fuzztime=60s -parallel=2
```

Go saves failing inputs under `parser/testdata/fuzz/FuzzAgeParser`. Replay them
with `go test -mod=mod ./parser -run FuzzAgeParser`. The Go version build tags keep
the new test out of builds using the module's original Go 1.16 toolchain.

## ClusterFuzzLite

Follow the [official build integration guide](https://google.github.io/clusterfuzzlite/build-integration/).
With Docker and a checkout of `google/oss-fuzz`, run from the OSS-Fuzz directory:

```bash
python3 infra/helper.py build_image --external /absolute/path/to/Plugin
python3 infra/helper.py build_fuzzers --external /absolute/path/to/Plugin --sanitizer address  # Select AddressSanitizer (ASan).
python3 infra/helper.py run_fuzzer --external /absolute/path/to/Plugin age_parser_fuzzer -- -max_total_time=60 -max_len=4096 -timeout=10
```

The builder image is pinned by digest and targets Linux x86-64. The build uses
the official native Go-to-libFuzzer adapter and produces `age_parser_fuzzer`.
Generated corpora, build outputs and crash artifacts belong outside the source
tree, except for deliberately retained regression inputs.
