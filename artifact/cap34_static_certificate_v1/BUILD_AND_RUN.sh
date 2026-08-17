#!/usr/bin/env bash
set -euo pipefail
mkdir -p build

# DFS reproduction from the supplied local transcription.
c++ -O3 -std=c++17 -DSIDE=34 src/triangle_root_universal_search.cpp -o build/triangle_dfs_v34
./build/triangle_dfs_v34 | tee build/reproduction_v34.stdout

# Static certificate verifier. It does not run C8/C16 search oracles; instead,
# it reconstructs every proposal and checks explicit C8/C16 witnesses.
c++ -O3 -std=c++17 src/verify_triangle_certificate_v34.cpp -o build/verify_cert_v34

gzip -dc certificates/cap34_orbit1.cert.gz | ./build/verify_cert_v34 - | tee build/cap34_orbit1.verify.log
gzip -dc certificates/cap34_orbit2.cert.gz | ./build/verify_cert_v34 - | tee build/cap34_orbit2.verify.log
