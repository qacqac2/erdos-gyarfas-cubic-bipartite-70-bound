# Audit History

## Purpose

This document records the major mathematical, computational, and
reproducibility issues examined during the development of the
70-vertex lower bound for cubic bipartite counterexamples to the
Erd?s每Gy芍rf芍s conjecture.

Its purpose is not to provide an alternative proof of the main theorem.
The proof itself is contained in the manuscript and in the files under

```text
proof/
```

Instead, this file records:

- issues raised during adversarial review;
- claims that were corrected or withdrawn;
- proof gaps that were subsequently closed;
- computational and provenance checks;
- the evidence used to resolve each issue.

The guiding principle throughout the project was to distinguish:

- mathematical proof;
- computational observation;
- implementation correctness;
- reproducibility evidence;
- external theorem dependencies.

A resolved audit issue is not deleted from this history.

---

# 1. Current theorem

The final theorem supported by this project is:

> **Theorem.** Every simple cubic bipartite graph on at most 68 vertices
> contains a cycle of length 4, 8, or 16.

Equivalently:

> **Corollary.** Any cubic bipartite counterexample to the
> Erd?s每Gy芍rf芍s conjecture has at least 70 vertices.

No claim is made about graphs on 70 or more vertices.

---

# 2. Final proof architecture

The accepted proof is divided into four logically separate layers.

1. **Structural reduction**
   - cubic bipartite graph ↙ symmetric \(v_3\)-configuration;
   - \(C_4\)-free ↙ linearity;
   - below 70 vertices, exclusion of \(C_4\) and \(C_8\) forces a
     \(C_6\);
   - \(C_6\) becomes a Berge triangle.

2. **Restricted-growth coverage**
   - every relevant connected target configuration occurs on a branch
     of one of two normalized triangle-rooted search trees.

3. **Safe cycle pruning**
   - the \(C_8\) and \(C_{16}\) pruning predicates do not falsely reject
     a valid target branch.

4. **Certified finite exhaustion**
   - static certificate streams account for every proposal in both
     cap-34 search trees;
   - both verified trees contain zero completed configurations.

The separation of these four claims was an explicit outcome of the
audit process.

---

# 3. Early 70-vertex cage classification error

## Issue

An early line of attack attempted to reason about the 70-vertex
girth-10 cubic graphs using the fact that three examples were known.

This was incorrectly interpreted as if the literature classified all
70-vertex cubic girth-10 graphs.

## Audit finding

The original paper of O'Keefe and Wong does **not** prove such a
classification.

It establishes the minimum order

\[
f(3,10)=70,
\]

constructs a 70-vertex example, and explicitly discusses the existence
of several known graphs while leaving open whether further examples
exist.

Therefore the inference

> there are exactly three 70-vertex cubic girth-10 graphs

was invalid.

## Resolution

The entire argument depending on a classification of order-70 cages
was abandoned.

The theorem was deliberately restricted to

\[
|V|\le 68.
\]

The project now uses O'Keefe每Wong only for the statement

\[
f(3,10)=70.
\]

No property of the individual 70-vertex cages is used.

## Status

**Resolved by withdrawal of the invalid route.**

---

# 4. Scope correction: no claim at order 70

## Issue

The structural argument forcing a \(C_6\) works strictly below
70 vertices.

At order 70, cubic graphs of girth 10 exist.

## Resolution

The manuscript explicitly states:

> No claim is made about graphs on 70 or more vertices.

The result is formulated as:

\[
|V(G)|\le 68,
\]

or equivalently a lower bound of 70 for a cubic bipartite
counterexample.

No extrapolation to the 70-vertex case is made.

## Status

**Resolved and explicitly documented.**

---

# 5. Moore-bound collision argument

## Issue

During development, the edge-rooted Moore-bound argument was at one
stage described with an incorrect cycle-length expression.

A later manuscript draft also contained the inequality

\[
a+b+1\le 10
\]

for cross-tree collisions with

\[
a,b\le 5.
\]

This is false when \(a=b=5\), since then

\[
a+b+1=11.
\]

The same draft also did not explicitly distinguish collisions within
one rooted tree from collisions between the two rooted trees.

## Resolution

The final argument treats the two cases separately.

### Same-side collision

Two distinct nonbacktracking root paths of lengths

\[
a,b\le 5
\]

ending at the same vertex contain a cycle of length at most

\[
a+b\le 10.
\]

### Cross-side collision

If a vertex at depth \(a\le5\) from one endpoint of the central edge
coincides with a vertex at depth \(b\le5\) from the other endpoint,
the two root paths together with the central edge contain a cycle of
length at most

\[
a+b+1\le 11.
\]

Both contradict girth at least 12.

Therefore the two depth-5 trees are internally collision-free and
mutually disjoint, yielding

\[
2(1+2+4+8+16+32)=126
\]

distinct vertices.

## Effect on theorem

The erroneous inequality did not invalidate the theorem, because the
correct bound \(11<12\) is still sufficient.

Nevertheless it was a genuine mathematical error in the draft and is
recorded here as such.

## Status

**Resolved in manuscript v0.2.**

---

# 6. External dependence on O'Keefe每Wong

## Issue

The proof below 70 vertices depends on the theorem

\[
f(3,10)=70.
\]

An audit distinguished two different claims:

1. the project uses the O'Keefe每Wong theorem correctly;
2. the complete 1980 proof has been independently re-proved within this
   project.

Only the first claim is made.

## Resolution

The original paper was inspected and the statement used by this
project was confirmed.

The project does not claim to reproduce the full minimum-cage proof.

## Status

**Correctly used external theorem dependency.**

---

# 7. Two normalized triangle roots

## Issue

The triangle-rooted search is complete only if every relevant Berge
triangle can be normalized to one of the two initial states.

## Resolution

After normalizing the triangle blocks to

\[
\{0,1,3\},\qquad
\{1,2,4\},\qquad
\{0,2,5\},
\]

linearity prevents the third block through point 0 from reusing

\[
1,2,3,5.
\]

The only previously introduced point that may be reused is 4.

Under first-occurrence labeling, the third block through 0 is therefore
either

\[
\{0,4,6\}
\]

or

\[
\{0,6,7\}.
\]

Thus there are exactly two normalized root types relevant to the
coverage proof.

## Status

**Resolved by direct proof.**

---

# 8. Restricted-growth coverage concern

## Issue

The most serious early mathematical concern was that the
restricted-growth generator might omit a valid isomorphism class when
the point cap was increased from 29 to 34.

Matching search counts between implementations would not by itself
resolve this concern, because two implementations could share the same
coverage specification error.

## Resolution

A cap-independent coverage proposition was written and audited.

For a connected linear symmetric \(v_3\)-configuration with

\[
v\le M,
\]

containing a Berge triangle and no Berge 4- or 8-cycle, the proof
constructs a target branch by maintaining:

\[
I_1:
\]

all target blocks whose least point is smaller than the current point
have been inserted;

\[
I_2:
\]

the inserted target blocks whose least point equals the current point
form a lexicographic initial segment;

\[
I_3:
\]

new labels are assigned in first-occurrence order.

The next target block necessarily has one of the forms

\[
\{p,q,r\},
\qquad
\{p,q,m\},
\qquad
\{p,m,m+1\},
\]

and the generator proposes all three forms.

Degree, repeated-pair, lexicographic, and sound cycle pruning cannot
remove this target branch.

Connectedness ensures that all target points are eventually introduced.

## Status

**Resolved by mathematical proof.**

Primary file:

```text
proof/coverage_proposition.md
```

---

# 9. Base case of the lexicographic invariant

## Issue

An adversarial review noted that the coverage proof initially stated
that the invariants held at the normalized root ※by construction§
without explicitly proving the nontrivial part of \(I_2\) for point 1.

## Resolution

The final proof now observes that the root already contains

\[
\{1,2,4\}.
\]

The remaining block through point 1 cannot reuse

\[
0,3
\]

because of \(\{0,1,3\}\), and cannot reuse

\[
2,4
\]

because of \(\{1,2,4\}\).

Thus if the remaining block is

\[
\{1,q,r\},
\]

then

\[
q\ge5,
\]

so

\[
(2,4)<_{\mathrm{lex}}(q,r).
\]

Hence the inserted blocks with least point 1 form a lexicographic
initial segment.

## Status

**Resolved in the final coverage proof.**

---

# 10. Point-cap and block-cap off-by-one concern

## Issue

A possible failure mode was that a nominal cap \(M\) search could
actually cover only configurations with fewer than \(M\) points or
blocks.

## Resolution

The coverage proof handles the two caps separately.

### Point cap

A target with

\[
v\le M
\]

uses first-occurrence labels only

\[
0,\ldots,v-1,
\]

so label \(M\) is never required.

### Block cap

A symmetric \(v_3\)-configuration has exactly \(v\) blocks.

If

\[
v<M,
\]

completion occurs before the block cap.

If

\[
v=M,
\]

the \(M\)-th target block produces the complete configuration.

The implementation checks completion before applying the block-count
cutoff.

## Status

**Resolved by proof and implementation-order inspection.**

---

# 11. Connectedness and unseen points

## Issue

A subtle concern was whether the search could saturate all currently
introduced points while some portion of a connected target
configuration remained unintroduced.

## Resolution

Suppose the introduced target point set \(S\) were a nonempty proper
subset and every point of \(S\) were saturated.

Then every target block incident with \(S\) would already have been
inserted.

Inserting a block introduces all of its points, so every such block
would lie entirely in \(S\).

Thus the incidences supported on \(S\) would form a nonempty proper
union of connected components of the target Levi graph, contradicting
connectedness.

## Status

**Resolved by proof.**

---

# 12. C8 pruning predicate

## Issue

The abstract coverage theorem is invalid if the implementation can
falsely reject a valid target block.

The first pruning predicate therefore required a separate correctness
proof.

## Resolution

For a structurally admissible candidate \(t\), the implementation
enumerates three distinct old blocks and four pairwise distinct
intersection points.

A successful witness produces

\[
T,p_0,B_i,p_1,B_j,p_2,B_h,p_3,T,
\]

a genuine simple \(C_8\).

Conversely, every new simple \(C_8\) containing the new block vertex
has exactly this form and is found by the enumeration.

Therefore

\[
\texttt{creates\_C8}(t)
\iff
\text{adding }t\text{ creates a new }C_8.
\]

## Status

**Resolved by mathematical/source-level proof.**

Primary file:

```text
proof/oracle_correctness.md
```

---

# 13. Exact-path semantics

## Issue

The \(C_{16}\) predicate depends on a recursive path routine.

Potential failure modes included:

- finding walks instead of simple paths;
- allowing repeated block vertices;
- accepting the target too early;
- parity or depth errors.

## Resolution

The visited structure contains both point and block vertices.

A successful recursive branch therefore contains no repeated Levi
vertex.

Success occurs only when the target is reached at exactly depth 14.

Reaching the target earlier is rejected.

Thus:

\[
\texttt{exact\_path}(x,y)
\iff
\]

there exists a simple path of exactly 14 old Levi edges between
\(x\) and \(y\).

## Status

**Resolved by proof and source inspection.**

---

# 14. C16 pruning predicate

## Issue

The relationship between a 14-edge old Levi path and a new
\(C_{16}\) required an exact proof.

## Resolution

If two candidate points \(x,y\) are joined by a simple old path of
length 14, then adding the new block vertex \(T\) and edges

\[
Tx,\ Ty
\]

closes the path into a simple \(C_{16}\).

Conversely, removing \(T\) from any new simple \(C_{16}\) leaves an old
simple path of length 14 between the two candidate points used by the
cycle.

Thus

\[
\texttt{creates\_C16}(t)
\iff
\text{adding }t\text{ creates a new }C_{16}.
\]

## Status

**Resolved by proof.**

---

# 15. Fresh-point boundary case in the C16 proof

## Issue

A later full-paper review noticed that the compressed manuscript
stated the exact-path lemma only for old point vertices, while the
subsequent C16 lemma quantified over arbitrary structurally admissible
candidate blocks, which may contain fresh points.

## Resolution

A fresh point has no incident block in the old Levi graph and therefore
cannot be an endpoint of a positive-length old path.

Likewise, if a new \(C_{16}\) used the edge from the new block vertex
to a fresh point, deleting the new block vertex would leave a
length-14 old path starting at an isolated fresh point, which is
impossible.

Hence the two candidate points used by a new \(C_{16}\) must both be
old, and the exact-path lemma applies.

## Status

**Resolved in manuscript v0.2.**

---

# 16. Third candidate point on a C16 path

## Issue

One possible concern was that the third point of a candidate block
might occur internally on the old length-14 path used to certify a
\(C_{16}\).

## Resolution

This does not invalidate the cycle.

The third incidence edge from the new block vertex then forms a chord
of the simple \(C_{16}\).

A simple cycle need not be induced.

## Status

**Resolved.**

---

# 17. Independent reimplementation and cap-29 regression

## Issue

Before accepting the cap-34 extension, the reconstructed search needed
a strong regression test against the published cap-29 computation.

## Resolution

An independent implementation reproduced the published cap-29 search
counts exactly.

A generic Levi-path implementation of the cycle tests also reproduced
the same cap-29 counts.

This supplied strong implementation-level cross-checking.

However, matching counts were explicitly **not** treated as a proof of
coverage; mathematical coverage was established separately.

## Status

**Resolved as computational cross-checking.**

---

# 18. Cap-34 exhaustive-search results

The cap-34 search returned:

## Root 1

```text
states      23143
attempted   2972750
structural  180011
C8          1424785
C16         1344812
completions 0
```

## Root 2

```text
states      202602
attempted   25227209
structural  1432521
C8          15207779
C16         8384308
completions 0
```

## Combined

```text
states      225745
attempted   28199959
structural  1612532
C8          16632564
C16         9729120
completions 0
```

The bookkeeping identity

\[
\text{attempted}
=
\text{structural}
+
C_8
+
C_{16}
+
\text{expanded}
\]

holds for each root, with

\[
\text{expanded}=\text{states}-1.
\]

These identities were treated only as consistency checks, not as proof
of exhaustion.

---

# 19. Upstream source provenance

## Issue

An early provenance limitation was that the cap-34 run had not yet
been performed directly from a fresh byte-identical Git checkout of
the upstream repository.

## Resolution

The upstream repository was freshly cloned in a Linux filesystem and
checked out at the immutable commit

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

The working tree was clean.

The upstream manifest verification succeeded.

The relevant search source had SHA-256

```text
e778648bc3d5b0a19c860bec1371a542c464652693c2cbc4c2cd5ef05f314f41
```

The completeness note had SHA-256

```text
b1b2b185c9f2ac16ea7861b351b6ca734565eee790b161837eb79de91bb633ff
```

The fixed source was compiled directly with

```text
-DSIDE=34
```

and rerun.

The cap-34 counters were reproduced exactly.

## Status

**Resolved by fresh official checkout reproduction.**

---

# 20. Decision-transcript hashes

The direct cap-34 reproduction also reproduced the exact transcript
hashes:

```text
orbit 1:
db548be2919a7f8a8b28c5ded67aa622779dd3e22192ee8a2a1e065f8c07b4c5

orbit 2:
c2e03a0f97ea3c649a5feaf4d4145b229da6f56aa31b8ebbdb8b769b18848607
```

These provide a strong fingerprint of the branch-decision sequence.

They are supplementary evidence and are not used as a substitute for
the static certificate.

---

# 21. Static finite-exhaustion certificate

## Issue

A depth-first search reporting zero solutions does not prove by itself
that every branch was generated and explored.

The remaining high-level risk after coverage and oracle correctness
were established was therefore:

> Was the finite cap-34 search tree actually exhausted?

## Resolution

Static certificate streams were generated for the two normalized roots.

The verifier independently reconstructs the expected proposal schedule
from each verified state.

Every expected proposal must be accounted for as exactly one of:

1. structural rejection;
2. \(C_8\) rejection with explicit witness;
3. \(C_{16}\) rejection with explicit witness;
4. accepted child followed recursively.

The certificate does not supply an arbitrary candidate list.

Therefore a silently omitted branch causes stream/verifier
desynchronization and verification failure.

## Status

**Resolved by certificate construction and verification.**

Primary file:

```text
proof/finite_exhaustion_certificate.md
```

---

# 22. Certificate rejection witnesses

## Structural rejection

The verifier recomputes the degree or repeated-pair violation directly.

## C8 rejection

The certificate supplies three old block identifiers.

The verifier reconstructs four intersections and checks the resulting
simple \(C_8\).

## C16 rejection

The certificate supplies the candidate endpoint pair and seven old
block identifiers.

The verifier reconstructs a simple old Levi path of length 14 and
checks that the candidate block closes it to a \(C_{16}\).

Thus the verifier does not merely trust a Boolean rejection decision
from the original DFS.

---

# 23. Independent certificate verification

The certificate artifact was independently verified in a fresh
environment:

```text
Ubuntu 24.04
g++ 13.3.0
WSL2
```

The verification process:

1. checked the artifact SHA-256 manifest;
2. rebuilt the verifier;
3. verified both certificate streams;
4. reproduced the exact expected counters;
5. obtained zero solutions for both roots;
6. terminated with:

```text
ARTIFACT VERIFIED
```

## Status

**Resolved by fresh-environment independent execution.**

---

# 24. Certificate and artifact hashes

The packaged artifact has SHA-256:

```text
32558c65679736fb2ff75b67b2d75a85a2b7b91e6dde5d15b06c3a9b59f0d7de
```

The compressed certificate hashes are:

```text
orbit 1:
dbc3b405bff9cadbaf5687e2eb8b6ba033255fc99d23b4bf7f49efc4d6e8b79a

orbit 2:
f8cd411dc6b26b5a703df811ad53c055d1e7ea75dbca2704a52bf0bbea911bd0
```

These hashes identify the exact artifacts that were verified.

Hash identity is a provenance mechanism and is not itself a proof of
correctness.

---

# 25. Tamper tests

The verifier was tested against deliberately modified certificate data.

Tested modifications included:

- flipping a certificate byte;
- truncating the certificate stream;
- modifying header count data.

These modifications were rejected.

One altered witness produced an explicit invalid-C8-witness failure.

Tamper tests are empirical robustness checks and are not substitutes
for the logical certificate argument.

## Status

**Passed.**

---

# 26. Original MITM concern

## Issue

An early adversarial review treated the unpublished widening of a
meet-in-the-middle implementation beyond its original 64-bit range as
a potentially fatal reproducibility issue.

## Resolution

The MITM computation was removed from the theorem's essential evidence
chain.

The theorem is supported by:

- the triangle-rooted DFS;
- the mathematical coverage proof;
- oracle correctness;
- the static certificate and verifier.

No unpublished 128-bit MITM modification is needed for the final
theorem.

## Status

**Resolved by removing the route from the trusted proof chain.**

---

# 27. Main-theorem dependency ordering

## Issue

One manuscript draft invoked the coverage proposition before explicitly
establishing the soundness hypothesis of its cycle predicates.

The required lemmas appeared immediately afterward, so this was a
presentation-order issue rather than a missing argument.

## Resolution

The final theorem proof now states first that the C8 and C16 oracle
lemmas establish the soundness hypothesis.

Only then is the restricted-growth coverage proposition invoked.

## Status

**Resolved in manuscript v0.2.**

---

# 28. Block-cap definition in the manuscript

## Issue

The compressed manuscript initially introduced a point cap \(M\), but
the later coverage proof referred to a block cap \(M\) without
formally introducing it first.

## Resolution

The final manuscript explicitly states before the coverage proposition
that the implementation has:

- a point-label cap \(M\);
- a block-count cap \(M\);
- completion testing before the block-count cutoff.

## Status

**Resolved in manuscript v0.2.**

---

# 29. Abstract wording

## Issue

An early abstract described the work as extending the frontier ※to 70
vertices§.

Although mathematically defensible as a lower-bound statement, this
could be read as claiming that order 70 itself had been classified.

## Resolution

The final wording is:

> We raise the lower bound for a cubic bipartite counterexample from
> 60 to 70 by proving that every simple cubic bipartite graph on at
> most 68 vertices contains a \(C_4\), \(C_8\), or \(C_{16}\).

The manuscript also explicitly states that no claim is made at order
70 or above.

## Status

**Resolved in manuscript v0.2.**

---

# 30. Related-work audit

## Issue

The first manuscript draft cited only three papers and therefore gave
too little context for the broader history of the conjecture.

## Resolution

The literature review was expanded to include:

- general unavoidable-cycle results;
- sufficiently-large-average-degree results;
- \(P_k\)-free special classes;
- cubic planar and claw-free classes;
- cubic computational history;
- cubic power-cycle spectrum results;
- recent diameter-two results.

In particular, the final related-work discussion includes:

- Verstra?te;
- Liu--Montgomery;
- Gao--Shan;
- Hu--Shen;
- Hegde--Sandeep--Shashank;
- Heckman--Krakovski;
- Nowbandegani et al.;
- Markstr?m;
- Bensmail;
- Carr.

Markstr?m's bibliographic entry was cross-checked against Bensmail's
published reference list:

```text
Congressus Numerantium 171 (2004), 179--192.
```

Bensmail's original paper was also inspected directly for the cubic
power-cycle claims used in the manuscript.

## Status

**Resolved for manuscript v0.2.**

---

# 31. Final full-manuscript adversarial review

The v0.1 manuscript received a formal-style adversarial review.

The reviewer found:

- no defect invalidating the main theorem;
- one genuine local Moore-bound error;
- one minor fresh-point proof gap;
- several presentation/self-containment issues.

Recommendation:

```text
Minor Revision
```

All identified items were repaired in v0.2.

The revised v0.2 manuscript was then reviewed again and passed without
a remaining theorem-level objection.

## Status

**Internal proof review passed.**

---

# 32. Trust boundary

The project does not claim proof-assistant-level formal verification.

The remaining conventional trusted computing base includes:

- the certificate verifier source;
- the C++ compiler and implementation;
- the operating system;
- the hardware used during verification;
- SHA-256 for artifact identity.

The finite exhaustion is therefore described as:

> static, independently executable, certificate-checked exhaustive
> computation,

not as a fully formally verified proof from first principles.

---

# 33. AI assistance

Generative AI tools were used extensively during:

- exploratory mathematical reasoning;
- literature search and source review;
- code interpretation;
- construction of independent checks;
- adversarial proof review;
- artifact and certificate design;
- manuscript drafting.

AI output is not treated as mathematical evidence.

All theorem-supporting claims are intended to reduce to:

- explicit mathematical proofs;
- external published theorems with identified citations;
- immutable source code;
- reproducible computation;
- static certificate data;
- independently executable verification.

AI systems are not listed as authors.

Responsibility for the released claims and artifact remains with the
human author.

---

# 34. Final internal status

As of manuscript v0.2:

| Component | Status |
|---|---|
| Graph-to-incidence reduction | Verified |
| Treatment of disconnected graphs | Verified |
| O'Keefe--Wong theorem usage | Verified external dependency |
| Forced \(C_6\) below 70 | Verified |
| Two triangle roots | Verified |
| Restricted-growth coverage | Verified |
| Lexicographic base case | Verified |
| Point/block cap safety | Verified |
| Connectedness argument | Verified |
| C8 oracle correctness | Verified |
| Exact-path semantics | Verified |
| C16 oracle correctness | Verified |
| Fresh-point boundary case | Verified |
| Official upstream source identity | Verified |
| Fresh official cap-34 reproduction | Verified |
| Exact cap-34 counters | Verified |
| Decision-transcript hashes | Verified |
| Static certificate construction | Completed |
| Static verifier execution | Verified |
| Independent fresh-environment verification | Verified |
| Tamper tests | Passed |
| Moore-bound manuscript correction | Completed |
| Related-work expansion | Completed |
| Full manuscript internal review | Passed |
| Formal public archival DOI | Pending release |
| Final TeX source | Pending publication preparation |

---

# 35. Release policy

The theorem-supporting artifact should be released as a fixed version
rather than referenced only through a moving `main` branch.

The intended final release process is:

```text
clean public GitHub repository
        |
        v
fixed GitHub release
        |
        v
archival deposit / DOI
        |
        v
paper cites release + DOI + artifact SHA-256
```

After release, this audit history should remain public alongside the
paper and verification artifact.

Future corrections should be appended to this file rather than
rewriting earlier audit history.