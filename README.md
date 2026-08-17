# A 70-Vertex Lower Bound for Cubic Bipartite Counterexamples to the Erdős–Gyárfás Conjecture

This repository contains the proof materials and computational artifact for the following computer-assisted result.

> **Theorem.** Every simple cubic bipartite graph on at most 68 vertices contains a cycle of length $4$, $8$, or $16$.

Equivalently,

> **Corollary.** Any cubic bipartite counterexample to the Erdős–Gyárfás conjecture has at least 70 vertices.

**Scope.** No claim is made here about graphs on 70 or more vertices.

---

## Status

The mathematical reduction, restricted-growth coverage argument, and the correctness of the $C_8$ and $C_{16}$ pruning oracles have been separately audited.

The cap-34 exhaustive search has also been encoded as a static certificate. The certificate has been verified in a fresh Ubuntu 24.04 / GCC 13.3.0 environment.

The two rooted searches certify:

| Root orbit | States | Attempted proposals | Structural rejections | $C_8$ rejections | $C_{16}$ rejections | Completions |
|---|---:|---:|---:|---:|---:|---:|
| 1 | 23,143 | 2,972,750 | 180,011 | 1,424,785 | 1,344,812 | **0** |
| 2 | 202,602 | 25,227,209 | 1,432,521 | 15,207,779 | 8,384,308 | **0** |
| **Total** | **225,745** | **28,199,959** | **1,612,532** | **16,632,564** | **9,729,120** | **0** |

The remaining provenance caveat is described in [Reproducibility and provenance](#reproducibility-and-provenance).

**This work has not yet been peer reviewed or published.**

---

## Background

The Erdős–Gyárfás conjecture asks whether every finite simple graph of minimum degree at least three contains a cycle whose length is a power of two.

For cubic bipartite graphs, Julius Tranquilli proved that every such graph on at most 58 vertices contains a $C_4$, $C_8$, or $C_{16}$, giving a 60-vertex lower bound for a counterexample.

This repository extends the same triangle-rooted restricted-growth framework from a point cap of $29$ to a point cap of $34$.

The corresponding Levi graph has twice as many vertices, so cap $34$ covers cubic bipartite graphs on at most

$$
2\cdot 34=68
$$

vertices.

---

## Proof outline

The argument has four logically separate parts.

### 1. Incidence reduction

Let $G=(X,Y;E)$ be a connected cubic bipartite graph.

Since

$$
3|X|=|E|=3|Y|,
$$

we have $|X|=|Y|=v$.

Taking the neighborhoods of vertices in one bipartition class gives a symmetric $v_3$-incidence system whose Levi graph is $G$.

If $G$ is $C_4$-free, the incidence system is linear.

In a linear incidence system, a Berge cycle of length $k$ corresponds to a simple cycle of length $2k$ in the Levi graph. Thus

$$
C_6 \leftrightarrow \text{Berge triangle},
$$

$$
C_8 \leftrightarrow \text{Berge 4-cycle},
$$

and

$$
C_{16} \leftrightarrow \text{Berge 8-cycle}.
$$

### 2. A $C_6$ is forced below 70 vertices

Suppose a connected cubic bipartite graph $H$ has fewer than 70 vertices and contains no $C_4$ or $C_8$.

If $H$ also contained no $C_6$, then, since $H$ is bipartite, its girth would be at least 10.

If the girth were exactly 10, this would contradict the theorem of O'Keefe and Wong that

$$
f(3,10)=70.
$$

If the girth were at least 12, the edge-rooted cubic Moore bound would give at least

$$
2(1+2+4+8+16+32)=126
$$

vertices.

Therefore every relevant connected component below 70 vertices contains a $C_6$, hence the associated configuration contains a Berge triangle.

### 3. Triangle-rooted restricted-growth coverage

After normalizing a Berge triangle, there are two root orbits.

The first is

$$
\{0,1,3\},\quad
\{1,2,4\},\quad
\{0,2,5\},\quad
\{0,4,6\},
$$

and the second is

$$
\{0,1,3\},\quad
\{1,2,4\},\quad
\{0,2,5\},\quad
\{0,6,7\}.
$$

The search always processes the least introduced point whose degree is below three.

The coverage proof maintains three invariants:

1. every target block whose least point is smaller than the current point has already been inserted;
2. the inserted target blocks whose least point is the current point form a lexicographic initial segment;
3. new point labels are introduced in first-occurrence order.

Every next target block falls into exactly one of three cases:

- both remaining points are already introduced;
- one remaining point is new;
- both remaining points are new.

The generator proposes all three possibilities under the restricted-growth labeling rule.

The degree, repeated-pair, and lexicographic filters cannot reject the target branch. Connectedness guarantees that all target points are eventually introduced.

The same proof works for any point cap $M$. The number $29$ in the original proof is only a cap on available labels. For a target with $v\le M$, no label outside

$$
0,\ldots,M-1
$$

is ever required.

The block-count cutoff is also safe: a symmetric $v_3$-configuration has exactly $v$ blocks, and the implementation tests for completion before applying the block-cap cutoff.

### 4. Cycle-oracle correctness

For a proposed new block $t$:

- `creates_C8(t)` returns true exactly when the new block closes a genuine Berge 4-cycle, equivalently a simple $C_8$ in the Levi graph;
- `creates_C16(t)` searches for a length-14 simple path in the old Levi graph between two members of $t$. Adding the two incidence edges through the new block then produces a simple $C_{16}$.

Thus

$$
\texttt{creates\_C8(t)}
\iff
\text{adding }t\text{ creates a new }C_8,
$$

and

$$
\texttt{creates\_C16(t)}
\iff
\text{adding }t\text{ creates a new }C_{16}.
$$

For the coverage proof, only the soundness direction is required:

$$
\texttt{oracle=true}
\Longrightarrow
\text{a forbidden cycle really exists}.
$$

The exhaustive cap-34 certificate then closes both rooted search trees with zero completions.

---

## Static certificate

The theorem-supporting exhaustion is encoded as two static certificate streams, one for each normalized triangle root.

The verifier does **not** simply trust the output of the original DFS search.

Instead, it reconstructs the restricted-growth proposal schedule and checks that every proposal is accounted for.

A proposal is either:

- rejected for a directly checkable structural reason;
- rejected with a supplied $C_8$ witness;
- rejected with a supplied $C_{16}$ witness; or
- accepted and expanded into its child state.

The verifier therefore checks both:

1. that every recorded rejection has a valid witness; and
2. that no branch of the prescribed search tree is silently omitted.

Both certificate streams terminate with

```text
solutions 0
```

---

## Quick verification

The verified artifact is located at

```text
artifact/cap34_static_certificate_v1/
```

On a Linux environment with a C++17 compiler, `gzip`, and `sha256sum`, run:

```bash
cd artifact/cap34_static_certificate_v1
bash VERIFY_ARTIFACT.sh
```

The final output should include:

```text
VERIFIED EG70TRI1-UNIVERSAL side=34 orbit=1
states 23143
attempted 2972750
structural 180011
C8 1424785
C16 1344812
expanded 23142
solutions 0

VERIFIED EG70TRI1-UNIVERSAL side=34 orbit=2
states 202602
attempted 25227209
structural 1432521
C8 15207779
C16 8384308
expanded 202601
solutions 0

ARTIFACT VERIFIED
```

The artifact has been verified under:

```text
Ubuntu 24.04
GCC / g++ 13.3.0
WSL2
```

---

## Artifact contents

```text
artifact/cap34_static_certificate_v1/
├── README.md
├── VERIFY_ARTIFACT.sh
├── BUILD_AND_RUN.sh
├── MANIFEST.sha256
├── EXPECTED_RESULTS.txt
├── UPSTREAM_SOURCE_IDENTITY.txt
├── CERTIFICATE_FORMAT.md
├── ENVIRONMENT.txt
├── src/
├── certificates/
├── logs/
├── binaries/
├── build/
└── tamper_tests/
```

The artifact contains:

- the cap-34 DFS source used for reproduction;
- the certificate generator;
- an independent certificate verifier;
- both compressed certificate streams;
- cap-29 regression data;
- cap-34 execution logs;
- transcript hashes;
- tamper tests;
- environment metadata;
- a SHA-256 manifest.

---

## Upstream source identity

The search framework is based on the reproducibility artifact accompanying:

> Julius Tranquilli,  
> *A 60-Vertex Lower Bound for Cubic Bipartite Counterexamples to the Erdős–Gyárfás Conjecture*,  
> arXiv:2608.02675 (2026).

Upstream repository:

```text
https://github.com/floor-licker/erdos-gyarfas-cubic-bipartite
```

Reference release:

```text
v1.0.0
```

Immutable upstream commit:

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

The upstream SHA-256 of

```text
research/src/triangle_root_universal_search.cpp
```

is

```text
e778648bc3d5b0a19c860bec1371a542c464652693c2cbc4c2cd5ef05f314f41
```

The upstream completeness note

```text
research/docs/completeness_argument.md
```

has SHA-256

```text
b1b2b185c9f2ac16ea7861b351b6ca734565eee790b161837eb79de91bb633ff
```

---

## Reproducibility and provenance

The upstream search source is anchored to the immutable commit

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

of the upstream repository.

A fresh Git clone was checked out directly at this commit. The working tree was clean, the upstream SHA-256 manifest verification succeeded, and the relevant source hashes matched the published manifest.

The triangle-rooted DFS was then compiled directly from this fresh upstream checkout with

```text
-DSIDE=34
```

using GCC 13.3.0 on Ubuntu 24.04.

The run reproduced exactly the cap-34 counters:

| Root orbit | States | Attempted | Structural | $C_8$ | $C_{16}$ | Completions |
|---|---:|---:|---:|---:|---:|---:|
| 1 | 23,143 | 2,972,750 | 180,011 | 1,424,785 | 1,344,812 | 0 |
| 2 | 202,602 | 25,227,209 | 1,432,521 | 15,207,779 | 8,384,308 | 0 |

and the decision-transcript hashes

```text
orbit 1:
db548be2919a7f8a8b28c5ded67aa622779dd3e22192ee8a2a1e065f8c07b4c5

orbit 2:
c2e03a0f97ea3c649a5feaf4d4145b229da6f56aa31b8ebbdb8b769b18848607
```

Thus the cap-34 DFS result has been independently reproduced directly from the immutable upstream source.

The static-certificate generator and verifier included in this repository are project-specific additions rather than upstream components. Their relationship to the upstream search is documented separately, and the resulting certificates are independently checkable without trusting the original DFS execution.

## Research and audit history

This project was developed as an exploratory AI-assisted mathematics experiment.

During adversarial review, several earlier claims or formulations were found to be incorrect or insufficiently justified and were removed or repaired. In particular:

- an earlier attempt incorrectly treated three known 70-vertex girth-10 cubic graphs as a complete classification;
- the present theorem was therefore deliberately restricted to graphs on at most 68 vertices;
- the cap-$M$ coverage argument was rewritten as a self-contained proof;
- the block-count cutoff and completion order were checked explicitly;
- the $C_8$ and $C_{16}$ pruning predicates were separated from the abstract coverage proof and verified independently;
- source references were changed from the mutable `main` branch to an immutable upstream commit;
- the cap-34 exhaustion was converted into a static certificate and checked independently.

A fuller audit history will be maintained separately in `AUDIT_HISTORY.md`.

---

## Repository structure

The intended repository structure is:

```text
.
├── README.md
├── LICENSE
├── CITATION.cff
├── AUDIT_HISTORY.md
│
├── paper/
│   ├── paper.tex
│   ├── paper.pdf
│   └── references.bib
│
├── proof/
│   ├── coverage_proposition.md
│   └── oracle_correctness.md
│
├── artifact/
│   └── cap34_static_certificate_v1/
│
└── upstream/
    ├── UPSTREAM_COMMIT.txt
    └── CHANGES_FROM_UPSTREAM.md
```

Some of these files are still being prepared.

---

## AI assistance disclosure

Generative AI tools were used extensively during:

- exploratory reasoning;
- literature and source review;
- code interpretation;
- proof auditing;
- adversarial review;
- artifact design;
- manuscript drafting.

AI output is **not** used as an evidentiary substitute for the mathematical proof or computational certificate.

All theorem-supporting claims are intended to be reducible to:

- explicit mathematical arguments;
- fixed source code;
- reproducible computations;
- static certificate files; and
- independently executable verification procedures.

Responsibility for the final mathematical claims and any publication lies with the human author(s).

---

## References

1. Julius Tranquilli,  
   *A 60-Vertex Lower Bound for Cubic Bipartite Counterexamples to the Erdős–Gyárfás Conjecture*,  
   arXiv:2608.02675, 2026.

2. M. O'Keefe and P. K. Wong,  
   *A Smallest Graph of Girth 10 and Valency 3*,  
   Journal of Combinatorial Theory, Series B **29** (1980), 91–105.

3. P. Erdős,  
   *Some Old and New Problems in Various Branches of Combinatorics*,  
   Discrete Mathematics **165–166** (1997), 227–231.

---

## Citation

A formal citation will be added once the manuscript and archival release are finalized.

For now, please cite this repository by its exact Git commit or release tag rather than the moving `main` branch.

---

## License

A software/documentation license has not yet been finalized.

Please do not assume reuse rights beyond those granted by the upstream sources until a license is added to this repository.