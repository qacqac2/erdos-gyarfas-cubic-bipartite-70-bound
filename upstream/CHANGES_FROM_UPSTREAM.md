# Changes from upstream

The reference upstream source is commit

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

from Julius Tranquilli's repository.

## Direct DFS reproduction

For the direct cap-34 DFS reproduction, no search-logic modification was made.

The upstream source

```text
research/src/triangle_root_universal_search.cpp
```

was compiled with the compile-time parameter

```text
-DSIDE=34
```

instead of its original cap-29 setting.

The resulting counters and decision-transcript hashes exactly matched the independently obtained cap-34 values recorded in this repository.

## Static certificate extension

The static-certificate generator and verifier in this repository are project-specific additions.

They extend the triangle-rooted restricted-growth framework by recording independently checkable witnesses for every rejected proposal and by reconstructing the complete proposal schedule during verification.

These certificate components are not claimed to be part of the upstream release.

Their source and certificate format are included in the artifact so that their behavior can be inspected and independently tested.