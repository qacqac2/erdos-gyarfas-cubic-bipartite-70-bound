# EG70TRI1 static certificate format

This is a minimal cap-34 extension of the upstream `EG58TRI1` universal
triangle-root certificate idea.

Each orbit is a separate binary stream. The verifier independently reconstructs
the restricted-growth proposal schedule. Structural rejections consume no
certificate bytes because the verifier derives them itself.

For each structurally admissible proposal the stream contains exactly one of:

* `0x08` + 3 bytes: C8 rejection. The three bytes are distinct indices of old
  blocks witnessing the 4-block Berge cycle.
* `0x10` + 1 byte endpoint-pair code + 7 bytes: C16 rejection. The seven block
  indices witness a simple length-14 path in the old Levi graph between two
  points of the candidate block.
* `0x20`: expand this proposal recursively.

Header (60 bytes):

* 8 bytes magic `EG70TRI1`
* 1 byte side size
* 1 byte root orbit
* 1 byte flags (`3` = C8+C16)
* 1 reserved zero byte
* six little-endian uint64 counters: states, attempted, structural, C8, C16,
  completions.

The verifier rejects malformed witnesses, missing/trailing bytes, count
mismatches, and any branch that reaches a complete configuration.

The cap-34 generator differs from the upstream cap-29 generator in only two
material range changes:

1. supported side cap increases from 29 to 34;
2. the generator's C16 witness DFS widens `seen_points` and `seen_blocks` from
   32-bit to 64-bit masks, because a cap-34 partial structure may contain block
   indices 32 or 33.

The verifier itself does not use those bit masks; it checks the seven listed
block indices and eight induced path points directly.
