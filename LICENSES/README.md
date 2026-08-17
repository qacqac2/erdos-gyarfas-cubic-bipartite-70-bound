# License map

Copyright © 2026 Yihan Zhuge.

This repository uses separate licenses for software, machine-readable
research data, documentation, and the manuscript because these materials
have different reuse requirements.

## Code

Project-authored executable source code, scripts, build files, and
verification software are licensed under the MIT License:

```text
LICENSES/LICENSE-CODE
```

This generally includes project-authored material under:

```text
artifact/**/src/
artifact/**/scripts/
artifact/**/tamper_tests/
```

and other executable build or verification files, except where a file
contains or is derived from third-party code.

Files derived from the upstream repository retain the upstream copyright
and license notices. See:

```text
THIRD_PARTY_NOTICES.md
```

## Research data

Project-authored machine-readable certificate streams, result tables,
retained machine outputs, and reproducibility logs are released under
CC0 1.0 Universal:

```text
LICENSES/LICENSE-DATA
```

This generally includes project-authored material under:

```text
artifact/**/certificates/
artifact/**/logs/
upstream/reproduction-cap34/
```

unless otherwise stated.

## Documentation

Project-authored Markdown documentation outside the manuscript is
licensed under Creative Commons Attribution 4.0 International
(CC BY 4.0):

```text
LICENSES/LICENSE-DOCUMENTATION
```

This includes, unless otherwise stated:

```text
README.md
AUDIT_HISTORY.md
proof/
upstream/*.md
artifact/**/README.md
artifact/**/CERTIFICATE_FORMAT.md
```

## Paper

The manuscript and its source remain copyright © 2026 Yihan Zhuge,
all rights reserved unless a later publication or archival record states
otherwise:

```text
LICENSES/LICENSE-PAPER
```

This includes:

```text
paper/
```

## Third-party material

Third-party files retain their original copyright and license terms.

In particular, portions of the search framework derive from the
reproducibility artifact accompanying Julius Tranquilli's work on the
60-vertex cubic-bipartite lower bound.

The relevant upstream repository is:

```text
https://github.com/floor-licker/erdos-gyarfas-cubic-bipartite
```

The reference upstream commit is:

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

See `THIRD_PARTY_NOTICES.md` for details.

Nothing in this repository relicenses third-party material beyond the
rights granted by its original license.