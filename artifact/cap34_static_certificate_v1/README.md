# Cap-34 triangle-rooted exhaustion artifact

## Scope

This artifact freezes evidence for one finite computational statement only:

> In each of the two normalized Berge-triangle restricted-growth search trees
> with point cap 34, after rejecting structural violations and proposals that
> create Berge 4- or Berge 8-cycles, there are zero completed symmetric
> configurations.

In Levi-graph language this is the computational exhaustion needed for the
`<=68`-vertex cubic-bipartite argument. This artifact by itself does **not**
prove the mathematical root-normalization, restricted-growth coverage theorem,
or the separate girth argument; those are mathematical proof obligations.
It makes no claim about graphs on 70 or more vertices.

## Upstream source anchor

The search schedule and cycle predicates were audited against Tranquilli's
immutable release `v1.0.0`, commit
`67adc92d31d5d1edfbd0a7b845dc232e31c412cd`.
See `UPSTREAM_SOURCE_IDENTITY.txt`.

Because this execution environment could not directly persist C++ raw downloads,
the local DFS source is a textual transcription rather than a byte-identical
Git checkout. That limitation is explicit. As a strong functional regression
check, compiling the local transcription at SIDE=29 reproduces the upstream
published orbit counts **and both exact decision-transcript SHA-256 hashes**.
See `logs/reproduction_v29.stdout`.

## Controlled cap-34 run

The SIDE=34 DFS returned exit code 0 and exactly:

* orbit 1: 23,143 states; 2,972,750 attempted; 180,011 structural;
  1,424,785 C8; 1,344,812 C16; 0 completions;
* orbit 2: 202,602 states; 25,227,209 attempted; 1,432,521 structural;
  15,207,779 C8; 8,384,308 C16; 0 completions;
* total: 225,745 states; 28,199,959 attempted; 1,612,532 structural;
  16,632,564 C8; 9,729,120 C16; 0 completions.

The run also freezes two new decision-transcript SHA-256 hashes; see
`EXPECTED_RESULTS.txt` and `logs/reproduction_v34.stdout`.

## Static certificates

`certificates/cap34_orbit1.cert.gz` and `cap34_orbit2.cert.gz` are compressed
static witness streams. Decompressed sizes are about 17.8 MB and 136.5 MB.

The checker `src/verify_triangle_certificate_v34.cpp` independently reconstructs
every proposal in the restricted-growth tree. It does **not** trust the searcher's
C8/C16 decisions. Every C8 rejection must supply three old block indices that
form a valid Berge 4-cycle witness. Every C16 rejection must supply seven distinct
old block indices forming a simple length-14 Levi path between two points of the
candidate block. Every accepted proposal is recursively expanded. Structural
rejections are recomputed by the checker itself.

Both static streams have been verified successfully and reproduce the exact DFS
counts with zero solutions.

## Quick verification

```sh
./VERIFY_ARTIFACT.sh
```

or manually:

```sh
c++ -O3 -std=c++17 src/verify_triangle_certificate_v34.cpp -o verify_cert
gzip -dc certificates/cap34_orbit1.cert.gz | ./verify_cert -
gzip -dc certificates/cap34_orbit2.cert.gz | ./verify_cert -
```

Expected final line for each stream includes `solutions 0`.

## Tamper checks

Three deliberate mutations of the orbit-1 stream were tested:

* flip one byte in the middle -> rejected (`invalid C8 witness`);
* truncate 17 bytes -> rejected (`unexpected end of certificate`);
* change the claimed state count in the header -> rejected (`header counts do
  not match the reconstructed proof`).

Logs are under `tamper_tests/`.

## Evidence status

The static certificate substantially reduces trust in the high-performance DFS:
the verifier needs only the mathematical proposal schedule plus local witness
checking. The remaining external proof obligations are the already-discussed
mathematical coverage/root-normalization and the `<70` girth reduction.
