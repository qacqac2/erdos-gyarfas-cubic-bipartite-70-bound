# Third-Party Notices

## Tranquilli triangle-rooted search framework

Parts of the computational framework in this repository are based on
software from:

Julius Tranquilli,
*A 60-Vertex Lower Bound for Cubic Bipartite Counterexamples to the
Erdős–Gyárfás Conjecture* (2026).

Upstream repository:

```text
https://github.com/floor-licker/erdos-gyarfas-cubic-bipartite
```

Reference release:

```text
v1.0.0
```

Reference commit:

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

The upstream executable code is distributed under the MIT License.

Upstream copyright:

```text
Copyright (c) 2026 Julius Tranquilli
```

The MIT License permits use, copying, modification, publication and
redistribution subject to preservation of the copyright and permission
notice.

The exact relationship between the upstream search and the present
cap-34 computation is documented in:

```text
upstream/UPSTREAM_COMMIT.txt
upstream/CHANGES_FROM_UPSTREAM.md
```

In particular, the direct cap-34 DFS reproduction uses the upstream

```text
research/src/triangle_root_universal_search.cpp
```

with the compile-time parameter

```text
-DSIDE=34
```

The static-certificate generator and verifier distributed in this
repository are project-specific extensions. Any portions copied or
adapted from upstream MIT-licensed code remain subject to the upstream
MIT notice.

Third-party copyright is not transferred to the author of this
repository.