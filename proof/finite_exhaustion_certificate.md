# Certification of the Finite Cap-34 Exhaustion

## Purpose

This document explains why the static certificate included in this
repository is sufficient to certify the finite cap-34 exhaustion used in
the proof.

The central claim is:

> If the certificate verifier accepts both triangle-root certificate
> streams and reports zero completed configurations, then the two
> prescribed cap-34 restricted-growth search trees contain no completed
> configuration.

This document concerns the **finite exhaustion layer** only.

It does not re-prove:

- the graph-theoretic reduction to triangle-rooted configurations;
- the restricted-growth coverage theorem;
- the mathematical correctness of the original DFS cycle oracles.

Those issues are treated separately in

```text
proof/coverage_proposition.md
proof/oracle_correctness.md
```

The purpose here is to close the remaining implication

$$
\text{finite search tree}
\longrightarrow
\text{certified exhaustive traversal}
\longrightarrow
\text{zero completions}.
$$

---

# 1. Search tree being certified

Fix a point cap

$$
M=34
$$

and one of the two normalized triangle roots.

A search state consists of a partial linear incidence structure together
with the restricted-growth labeling and ordering information prescribed
by the search.

At each nonterminal state, the search:

1. chooses the least introduced point whose current degree is less than
   three;
2. generates the complete prescribed list of candidate blocks through
   that point;
3. considers those candidates in the fixed restricted-growth and
   lexicographic order;
4. for each candidate, either rejects it or creates a child state.

A candidate may be rejected because of:

1. a structural violation;
2. creation of a Berge 4-cycle, equivalently a Levi $C_8$;
3. creation of a Berge 8-cycle, equivalently a Levi $C_{16}$.

Otherwise the candidate is accepted and produces a child state.

A state in which every introduced point has degree three and the number
of blocks equals the number of introduced points is a completed
symmetric configuration.

The coverage theorem proves that every relevant target configuration
appears on at least one branch of this abstract search tree.

The problem addressed here is therefore finite:

> certify that every branch of each of the two cap-34 search trees is
> either correctly rejected or completely explored.

---

# 2. Why a DFS log alone would not be enough

A program printing

```text
completions=0
```

is not by itself a certificate of exhaustive search.

For example, an implementation could accidentally:

- fail to generate one legal candidate;
- skip a subtree;
- reject a candidate for an invalid reason;
- terminate early;
- corrupt its recursion state;
- print counters unrelated to the search actually performed.

Therefore the certificate verifier is not designed merely to check a
summary line emitted by the original DFS.

Instead, it reconstructs the prescribed search process and requires
explicit justification for every branch that is not expanded.

This creates the following separation:

$$
\boxed{
\text{search program proposes a result}
\qquad
\text{certificate verifier checks the result}
}
$$

The verifier does not need to trust the original DFS's statement that a
particular branch was safely discarded.

---

# 3. Certificate principle

For every state, the verifier independently reconstructs the ordered
candidate schedule prescribed by the restricted-growth generator.

For every candidate in that schedule, exactly one of the following must
be justified:

1. **structural rejection;**
2. **$C_8$ rejection;**
3. **$C_{16}$ rejection;**
4. **accepted child state.**

There is no certificate record meaning

> ignore this candidate.

Thus every prescribed candidate must be accounted for.

This is the essential completeness property of the certificate format.

---

# 4. Independent reconstruction of the proposal schedule

The certificate stream does not determine which candidate should be
examined next.

The verifier reconstructs that candidate from the current verified
state.

In particular, it independently determines:

- the least deficient point;
- the currently introduced labels;
- the candidates involving only old points;
- candidates involving one fresh point;
- the candidate involving two fresh points;
- their restricted-growth form;
- their prescribed lexicographic order.

Hence a certificate cannot omit a candidate merely by failing to mention
it.

If the next certificate record does not correspond to the candidate
that the verifier itself expects, verification fails.

This property is critical.

If the certificate generator itself were allowed to supply the candidate
list, then a generator bug and a certificate omission could be
identical, and the certificate would not establish exhaustive coverage.

Instead, the logical arrangement is

$$
\text{verified state}
\longrightarrow
\text{verifier reconstructs next proposal}
\longrightarrow
\text{certificate must justify that proposal}.
$$

---

# 5. Structural rejection certificates

A structural rejection does not rely on the original DFS's rejection
decision.

The verifier checks the structural condition directly from the current
state and candidate.

The relevant conditions are elementary:

- a point would exceed degree three; or
- a pair of points would occur together in more than one block.

Therefore a structural-rejection record is accepted only when the
candidate really violates the prescribed structural constraints.

Consequently,

$$
\text{verified structural rejection}
\Longrightarrow
\text{candidate cannot belong to a valid linear }v_3
\text{ configuration}.
$$

---

# 6. $C_8$ rejection witnesses

For a candidate block $t$, a $C_8$ certificate supplies three existing
block identifiers.

Call the corresponding old blocks

$$
B_i,\quad B_j,\quad B_h.
$$

The verifier reconstructs the four required intersections

$$
p_0=t\cap B_i,
$$

$$
p_1=B_i\cap B_j,
$$

$$
p_2=B_j\cap B_h,
$$

$$
p_3=B_h\cap t.
$$

It verifies that

1. each required intersection exists;
2. the three supplied old blocks are appropriate existing blocks of the
   current state;
3. the four points

   $$
   p_0,p_1,p_2,p_3
   $$

   are pairwise distinct.

If these checks succeed, then after the new block vertex $T$ is added,
the Levi graph contains

$$
T,p_0,B_i,p_1,B_j,p_2,B_h,p_3,T,
$$

which is a simple $C_8$.

Thus

$$
\boxed{
\text{accepted }C_8\text{ witness}
\Longrightarrow
\text{candidate really creates a forbidden }C_8.
}
$$

The verifier therefore need not trust the original implementation of
`creates_C8`.

The certificate provides a small, directly checkable witness.

---

# 7. $C_{16}$ rejection witnesses

For a candidate block $t$, a $C_{16}$ certificate identifies two members
of $t$ and a sequence of seven existing block identifiers.

The verifier checks that these old blocks and their intervening points
form an alternating simple path in the old Levi graph with exactly

$$
14
$$

incidence edges between the two selected candidate points.

Let those endpoints be

$$
x,y\in t.
$$

The verified old path has the form

$$
x,
B_1,
p_1,
B_2,
p_2,
\ldots,
B_7,
y,
$$

with all vertices required for simplicity checked directly from the
current state.

After adding the new block vertex $T$ corresponding to $t$, the two
incidence edges

$$
T-x
\qquad\text{and}\qquad
T-y
$$

close this length-14 path into a simple cycle of length

$$
14+2=16.
$$

Therefore

$$
\boxed{
\text{accepted }C_{16}\text{ witness}
\Longrightarrow
\text{candidate really creates a forbidden }C_{16}.
}
$$

Again, verification uses an explicit witness rather than trusting the
original DFS's `creates_C16` decision.

---

# 8. Accepted candidates

If a candidate is not rejected, the verifier constructs the
corresponding child state itself.

The child is obtained by adding the verified candidate block and
updating the state data prescribed by the restricted-growth search.

The certificate cannot replace this child by an unrelated state.

The verifier then continues recursively from that child.

Thus every accepted proposal produces the exact subtree prescribed by
the search definition.

Schematically,

$$
S
\xrightarrow{\text{accepted }t}
S+t
$$

is reconstructed by the verifier rather than supplied as an arbitrary
new state by the certificate.

---

# 9. Why an omitted branch is detectable

Suppose that at some verified state the prescribed candidate list is

$$
t_1,t_2,\ldots,t_k.
$$

The verifier reconstructs this list independently.

For each $t_i$, it requires either:

- a valid rejection justification; or
- recursive verification of the child state produced by $t_i$.

Therefore there is no way for a valid certificate stream to move from

$$
t_i
$$

directly to

$$
t_{i+2}
$$

while silently ignoring

$$
t_{i+1}.
$$

At the point where the verifier expects a record concerning
$t_{i+1}$, the stream would instead contain data belonging to another
candidate or would terminate.

Either case causes verification failure.

Hence

$$
\boxed{
\text{accepted certificate}
\Longrightarrow
\text{no prescribed candidate branch has been silently omitted}.
}
$$

This is the main reason the static certificate is stronger evidence than
the original DFS transcript alone.

---

# 10. Completion detection

While reconstructing accepted states, the verifier also checks the
completion condition.

A complete state represents a symmetric configuration when the current
introduced point set and block set have equal size and every introduced
point has degree three.

Such a state is counted as a solution.

Thus the verifier does not rely on the certificate stream merely
asserting

```text
solutions 0
```

without reconstructing the underlying states.

If a completed state occurs on a verified branch, it is encountered by
the verifier's own traversal and contributes to the solution count.

For the two cap-34 certificate streams, the verified result is

```text
solutions 0
```

for both roots.

---

# 11. Exhaustion theorem

We can now state the central certification result.

## Theorem

Fix one of the two normalized triangle roots and the cap-34
restricted-growth search rule.

Assume that the certificate verifier:

1. reconstructs from every verified state the complete prescribed
   ordered candidate list;
2. accepts a structural rejection only after directly verifying the
   structural violation;
3. accepts a $C_8$ rejection only after validating a genuine simple
   $C_8$ witness;
4. accepts a $C_{16}$ rejection only after validating a genuine simple
   $C_{16}$ witness;
5. constructs and recursively verifies the exact child state of every
   accepted candidate;
6. detects completed states during this traversal;
7. rejects malformed, missing, inconsistent, or prematurely terminated
   certificate data.

If the verifier accepts the entire certificate stream and reports zero
solutions, then the prescribed search tree rooted at that triangle root
contains no completed state.

### Proof

We argue recursively over the finite search tree.

Consider any verified state $S$.

The verifier independently reconstructs the complete ordered candidate
list

$$
t_1,\ldots,t_k.
$$

For each $t_i$, exactly one of two possibilities is certified.

### Rejected candidate

The verifier directly establishes either

- a structural violation;
- a genuine $C_8$;
- or a genuine $C_{16}$.

Therefore this candidate cannot extend to an admissible completed target
configuration avoiding the forbidden cycles.

Its branch is legitimately closed.

### Accepted candidate

The verifier constructs the exact child state

$$
S+t_i
$$

and recursively verifies the certificate for that child.

Hence no accepted child subtree is omitted.

Since every candidate generated at $S$ is processed in one of these two
ways, every child branch of $S$ is either validly closed or recursively
exhausted.

Applying the same argument recursively proves that every branch below
the root is accounted for.

Because the search tree is finite under the cap-34 point and block
bounds, this recursion terminates.

During the verified traversal, every completed state would be detected
and counted.

If the accepted certificate reports zero such states, no completed state
occurs anywhere in the prescribed search tree.

Therefore the rooted search tree contains no completion.

$\square$

---

# 12. Application to the two cap-34 roots

The artifact contains one certificate stream for each normalized
triangle root.

The independently executed verifier reports the following results.

## Root orbit 1

```text
VERIFIED EG70TRI1-UNIVERSAL side=34 orbit=1
states 23143
attempted 2972750
structural 180011
C8 1424785
C16 1344812
expanded 23142
solutions 0
```

## Root orbit 2

```text
VERIFIED EG70TRI1-UNIVERSAL side=34 orbit=2
states 202602
attempted 25227209
structural 1432521
C8 15207779
C16 8384308
expanded 202601
solutions 0
```

Thus both certificate streams are accepted and both verified trees have
zero completions.

The combined counts are

| Quantity | Total |
|---|---:|
| states | 225,745 |
| attempted proposals | 28,199,959 |
| structural rejections | 1,612,532 |
| $C_8$ rejections | 16,632,564 |
| $C_{16}$ rejections | 9,729,120 |
| completions | **0** |

For each root the bookkeeping identity

$$
\text{attempted}
=
\text{structural}
+
C_8
+
C_{16}
+
\text{expanded}
$$

holds.

Numerically,

for orbit 1,

$$
2\,972\,750
=
180\,011
+
1\,424\,785
+
1\,344\,812
+
23\,142,
$$

and for orbit 2,

$$
25\,227\,209
=
1\,432\,521
+
15\,207\,779
+
8\,384\,308
+
202\,601.
$$

Also,

$$
\text{expanded}=\text{states}-1
$$

for each rooted tree.

These identities are useful consistency checks.

They are **not** substitutes for the branch-by-branch certificate
verification.

---

# 13. Independent DFS reproduction

The static certificate is not the only computational evidence.

The original triangle-rooted DFS was also reproduced independently from
a fresh checkout of the immutable upstream commit

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

after successful verification of the upstream SHA-256 manifest.

The fixed upstream source was compiled with

```text
-DSIDE=34
```

and reproduced exactly the same two sets of cap-34 search counters.

It also reproduced the decision-transcript hashes

```text
orbit 1
db548be2919a7f8a8b28c5ded67aa622779dd3e22192ee8a2a1e065f8c07b4c5
```

and

```text
orbit 2
c2e03a0f97ea3c649a5feaf4d4145b229da6f56aa31b8ebbdb8b769b18848607
```

This direct reproduction and the static certificate play different
roles.

The DFS reproduction establishes that the cap-34 behavior is reproduced
from the fixed upstream search implementation.

The static certificate establishes that the resulting finite exhaustion
can be checked through explicit local branch witnesses rather than by
trusting only the DFS run.

---

# 14. Artifact integrity

The publication artifact is distributed with a SHA-256 manifest.

The archived artifact used in this project has SHA-256

```text
32558c65679736fb2ff75b67b2d75a85a2b7b91e6dde5d15b06c3a9b59f0d7de
```

for the packaged archive.

The compressed orbit certificates have SHA-256 values

```text
orbit 1
dbc3b405bff9cadbaf5687e2eb8b6ba033255fc99d23b4bf7f49efc4d6e8b79a
```

and

```text
orbit 2
f8cd411dc6b26b5a703df811ad53c055d1e7ea75dbca2704a52bf0bbea911bd0
```

respectively.

These hashes identify the exact certificate data that was independently
verified.

Hash identity is a provenance mechanism; it is not itself a proof that
the certificate is correct.

The proof value comes from successful execution of the verifier on those
fixed streams.

---

# 15. Independent verification

The static certificate artifact was subsequently verified in a fresh
environment using

```text
Ubuntu 24.04
g++ 13.3.0
WSL2
```

The verification script:

1. checked the artifact manifest;
2. built the verifier;
3. verified both certificate streams;
4. reproduced the exact expected counters;
5. reported zero solutions for both roots;
6. terminated with

```text
ARTIFACT VERIFIED
```

This independent run is computational evidence that the distributed
artifact is self-contained and reproducible outside the environment in
which it was constructed.

---

# 16. Tamper tests

Several negative tests were also performed.

The verifier rejected certificate data after:

- modification of a certificate byte;
- truncation of the certificate stream;
- alteration of header count data.

For example, a modified witness was rejected as an invalid $C_8$
witness.

These tests are useful evidence that the verifier does not merely parse
the certificate superficially.

However, tamper tests are empirical checks.

They do not replace the logical argument in Sections 3--11 establishing
why a successfully verified complete stream certifies exhaustion.

---

# 17. Relationship to the original cycle oracles

There are two distinct ways in which forbidden-cycle correctness enters
the project.

## Original DFS

The direct DFS uses the functions

```text
creates_C8(t)
creates_C16(t)
```

whose mathematical correctness is proved in

```text
proof/oracle_correctness.md
```

## Static certificate verifier

The static verifier does not need to trust the original oracle's Boolean
answer.

Instead, a rejected candidate is accompanied by an explicit local
witness that the verifier checks directly.

Thus the logical structure is deliberately redundant:

$$
\text{original oracle reasoning}
\qquad+\qquad
\text{independently checkable rejection witnesses}.
$$

Agreement between the two mechanisms substantially reduces the risk that
the final result depends on one opaque cycle-detection routine.

---

# 18. Relationship to restricted-growth coverage

The certificate theorem by itself does **not** say that every relevant
mathematical configuration appears in the search tree.

That is the role of

```text
proof/coverage_proposition.md
```

Conversely, the coverage theorem by itself does **not** say that the
finite search tree was actually exhausted.

That is the role of the present certificate argument.

The two statements combine as follows.

Let $\mathcal C$ be a hypothetical connected target configuration
satisfying the hypotheses of the coverage proposition.

Then:

1. the coverage theorem places a labeling of $\mathcal C$ on one of the
   two prescribed search trees;
2. oracle soundness ensures that this target branch is not falsely
   rejected;
3. the accepted static certificates prove that both finite search trees
   have been exhaustively closed;
4. both verified searches contain zero completions.

This is a contradiction.

Therefore no such $\mathcal C$ exists.

---

# 19. Full dependency chain

The computer-assisted proof can now be separated into the following
steps.

### Mathematical reduction

A hypothetical simple cubic bipartite counterexample on at most
68 vertices has a connected cubic bipartite component on fewer than
70 vertices.

If it avoids $C_4$ and $C_8$, then the associated incidence system is
linear.

O'Keefe--Wong's result

$$
f(3,10)=70
$$

together with the cubic Moore bound implies that the component contains
a $C_6$.

Hence its incidence configuration contains a Berge triangle.

### Search coverage

`coverage_proposition.md` proves that every such target configuration
appears on a branch of one of the two normalized cap-34 search trees.

### Safe pruning

`oracle_correctness.md` proves that the forbidden-cycle pruning rules do
not falsely reject such a target branch.

### Certified exhaustion

The present document shows that successful verification of the two
static certificate streams proves that every branch of both finite
search trees has been accounted for.

Both verified streams report

$$
\text{solutions}=0.
$$

Therefore the hypothetical target configuration cannot exist.

Consequently every simple cubic bipartite graph on at most 68 vertices
contains a cycle of length

$$
4,\quad 8,\quad\text{or}\quad16.
$$

Equivalently, any cubic bipartite counterexample to the
Erd?s--Gy¨¢rf¨¢s conjecture has at least 70 vertices.

---

# 20. Trust boundaries

The certificate substantially reduces the amount of computation that
must be trusted, but it does not eliminate all conventional software
trust assumptions.

The remaining trusted computing base includes:

- the source code of the certificate verifier;
- the correctness of its compilation;
- the C++ language implementation and compiler used;
- ordinary operating-system and hardware correctness;
- SHA-256 for artifact identity.

The verifier has not been formally proved correct in a proof assistant.

Accordingly, the strongest justified statement is not

> the finite exhaustion has been formally machine-verified from first
> principles,

but rather

> the finite exhaustion is accompanied by a static, independently
> executable certificate whose verifier reconstructs the full proposal
> schedule and checks explicit local witnesses for every rejected
> branch.

This is a substantially stronger reproducibility standard than relying
only on a single DFS execution and its printed summary.

---

# 21. What is proved and what is evidence

For clarity, the roles of the different ingredients are:

## Mathematical argument

The reasoning in Sections 3--11 proves:

$$
\boxed{
\text{if the specified verifier accepts a complete stream with }
\text{solutions}=0,
\text{ then the represented search tree has no completion}.
}
$$

## Computation

The actual verifier executions establish that the two distributed
cap-34 streams are accepted with zero solutions.

## Cross-checks

The following provide additional independent evidence:

- exact reproduction of the upstream cap-34 DFS;
- exact agreement of all counters;
- exact decision-transcript hashes;
- SHA-256 artifact identity;
- fresh-environment certificate verification;
- tamper tests.

None of these empirical cross-checks is silently substituted for the
logical certificate argument.

---

# 22. Conclusion

The cap-34 finite exhaustion is not supported merely by the statement

```text
completions=0
```

from a search program.

Instead, the two finite rooted search trees are represented by static
certificate streams whose verifier

- independently reconstructs every prescribed proposal;
- requires every rejected proposal to have a directly checkable reason
  or cycle witness;
- recursively reconstructs every accepted child;
- detects completed states;
- rejects missing or inconsistent stream data.

Both fixed cap-34 certificate streams have been independently verified
and contain zero completed configurations.

Therefore the finite-exhaustion step required by the main theorem is
certified, subject to the explicitly stated conventional software trust
boundary.