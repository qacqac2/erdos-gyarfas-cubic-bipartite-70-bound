#!/usr/bin/env bash
set -euo pipefail
sha256sum -c MANIFEST.sha256
mkdir -p build
c++ -O3 -std=c++17 src/verify_triangle_certificate_v34.cpp -o build/verify_cert_v34
gzip -dc certificates/cap34_orbit1.cert.gz | ./build/verify_cert_v34 -
gzip -dc certificates/cap34_orbit2.cert.gz | ./build/verify_cert_v34 -
echo 'ARTIFACT VERIFIED'
