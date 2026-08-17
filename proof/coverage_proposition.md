# Coverage of the Triangle-Rooted Restricted-Growth Search

## Purpose

This document proves the completeness of the triangle-rooted
restricted-growth enumeration used in the cap-34 computation.

The result is purely a **coverage statement**:

> every connected target configuration satisfying the stated hypotheses
> has at least one branch in the search tree that reaches it.

The proof is deliberately separated from the implementation-specific
correctness of the $C_8$ and $C_{16}$ pruning predicates. For the
coverage argument below, those predicates are required only to be
**sound**: they must never reject a block unless adding that block really
creates the corresponding forbidden cycle.

The implementation-specific oracle proofs are given separately in
`oracle_correctness.md`.

---

## 1. Configurations

A **linear symmetric $v_3$-configuration** consists of

- a set $P$ of $v$ points;
- a set $\mathcal B$ of $v$ blocks;
- every block contains exactly three points;
- every point belongs to exactly three blocks;
- every pair of distinct points belongs to at most one common block.

Its Levi graph is the bipartite incidence graph with vertex classes
$P$ and $\mathcal B$.

We say that the configuration is **connected** when its Levi graph is
connected.

A Berge cycle of length $k$ consists of distinct points

$$
x_0,x_1,\ldots,x_{k-1}
$$

and distinct blocks

$$
B_0,B_1,\ldots,B_{k-1}
$$

such that

$$
x_i\in B_{i-1}\cap B_i
$$

with indices modulo $k$.

For a linear configuration, Berge cycles of lengths $4$ and $8$
correspond respectively to simple cycles of lengths $8$ and $16$ in
the Levi graph.

---

## 2. The normalized triangle roots

Assume that the target configuration contains a Berge triangle.

Choose the three triangle points and relabel them

$$
0,1,2.
$$

The three triangle blocks may then be written, after relabeling their
third points,

$$
\{0,1,3\},\qquad
\{1,2,4\},\qquad
\{0,2,5\}.
$$

Consider the third block through point $0$.

Because the configuration is linear, this block cannot contain

$$
1,\ 2,\ 3,\ \text{or }5,
$$

since each of those points already occurs with $0$ in one of the two
triangle blocks through $0$.

Among the already introduced labels, the only possible additional
point is therefore $4$.

Using first-occurrence labeling for all new points, the third block
through $0$ consequently has exactly one of the two forms

$$
\{0,4,6\}
$$

or

$$
\{0,6,7\}.
$$

Hence every target configuration containing a Berge triangle admits a
labeling beginning with one of the following two roots.

### Root orbit 1

$$
\{0,1,3\},\qquad
\{1,2,4\},\qquad
\{0,2,5\},\qquad
\{0,4,6\}.
$$

### Root orbit 2

$$
\{0,1,3\},\qquad
\{1,2,4\},\qquad
\{0,2,5\},\qquad
\{0,6,7\}.
$$

These are the two initial states considered by the search.

---

## 3. Abstract search rule

At every state, the introduced point labels form an initial interval

$$
0,1,\ldots,m-1.
$$

The search selects the least introduced point $p$ whose current degree
is less than three.

A new block through $p$ is written

$$
\{p,q,r\},
$$

with

$$
p<q<r.
$$

New labels are introduced in restricted-growth order.

Consequently every candidate has one of exactly three forms.

### Case A: both remaining points are old

$$
\{p,q,r\},
\qquad
p<q<r<m.
$$

### Case B: exactly one remaining point is new

$$
\{p,q,m\},
\qquad
p<q<m.
$$

### Case C: both remaining points are new

$$
\{p,m,m+1\}.
$$

For a fixed $p$, candidates are processed in lexicographic order of the
pair

$$
(q,r).
$$

The search rejects a proposed block if it violates one of the following
conditions:

1. a point would acquire degree greater than three;
2. a pair of points would occur in two blocks;
3. the block is inconsistent with the prescribed lexicographic order;
4. adding the block is certified to create a forbidden Berge
   $4$-cycle;
5. adding the block is certified to create a forbidden Berge
   $8$-cycle.

The search also has a point cap $M$ and a block cap $M$.

Completion is tested before the block-cap cutoff is applied.

---

## 4. Coverage proposition

### Proposition

Let $M\ge 8$.

Let $\mathcal C$ be a connected linear symmetric $v_3$-configuration
with

$$
v\le M.
$$

Assume that $\mathcal C$

1. contains a Berge triangle;
2. contains no Berge cycle of length $4$;
3. contains no Berge cycle of length $8$.

Assume also that the two cycle-rejection predicates used by the search
are sound:

- a Berge-$4$ rejection occurs only if the proposed block really
  creates a Berge cycle of length $4$;
- a Berge-$8$ rejection occurs only if the proposed block really
  creates a Berge cycle of length $8$.

Then one of the two triangle-rooted restricted-growth search trees with
point cap $M$ contains a completed state isomorphic to $\mathcal C$.

---

## 5. Proof

Choose a Berge triangle in $\mathcal C$ and normalize it as in
Section 2.

Thus, after relabeling, the search begins in one of the two prescribed
root states.

We now follow one particular branch of the search and show
inductively that every block chosen on that branch can be taken to be a
block of $\mathcal C$.

At every stage let

$$
0,1,\ldots,m-1
$$

be the points of $\mathcal C$ that have already been introduced.

Let $p$ be the least introduced point whose current degree in the
partial state is less than three.

We maintain the following three invariants.

### Invariant I1

Every block of $\mathcal C$ whose least point is smaller than $p$ has
already been inserted.

### Invariant I2

Among the blocks of $\mathcal C$ whose least point is exactly $p$, the
blocks already inserted form a lexicographic initial segment when the
remaining pair $(q,r)$ is ordered lexicographically.

### Invariant I3

The introduced target points have labels exactly

$$
0,1,\ldots,m-1,
$$

and whenever a previously unseen target point first occurs, it receives
the next unused label.

These invariants hold at the normalized root by construction.

For completeness, we verify the only nontrivial part of the base case.

After the four root blocks have been inserted, point $0$ is saturated,
so the least deficient introduced point is $p=1$.

Among the target blocks whose least point is $1$, the root already
contains

$$
\{1,2,4\}.
$$

The remaining target block through $1$ cannot use $0$ or $3$, since
$\{0,1,3\}$ is already a block. It also cannot use $2$ or $4$, since
$\{1,2,4\}$ is already a block and the target configuration is linear.

Consequently, if the remaining block through $1$ is

$$
\{1,q,r\},
\qquad q<r,
$$

then necessarily

$$
q\ge 5.
$$

Hence

$$
(2,4)<_{\mathrm{lex}}(q,r),
$$

so the already inserted target blocks with least point $1$ form a
lexicographic initial segment.

Thus Invariant I2 holds at the base state. Invariants I1 and I3 are
immediate from the normalized first-occurrence labeling.

Suppose now that they hold at some later state.

Because $p$ is deficient, at least one target block through $p$ has not
yet been inserted.

Let

$$
B=\{p,q,r\},
\qquad
p<q<r,
$$

be the lexicographically first missing target block whose least point is
$p$.

We show that the search proposes $B$ and does not reject it.

---

### 5.1 No point smaller than $p$ can occur in $B$

By definition, $p$ is the least deficient introduced point.

Hence every introduced point smaller than $p$ already has degree three
in the current partial state.

All blocks currently present belong to the target configuration
$\mathcal C$.

Since every point has target degree exactly three, no additional target
block can contain any already saturated point smaller than $p$.

Therefore $p$ is indeed the least point of $B$.

---

### 5.2 The generator proposes the next target block

Consider which of $q$ and $r$ have already been introduced.

There are exactly three possibilities.

#### Case A: both $q$ and $r$ are already introduced

Then

$$
q,r<m,
$$

and the candidate is of the old-old form

$$
\{p,q,r\}.
$$

The generator enumerates such candidates.

#### Case B: exactly one of $q,r$ is new

Since $q<r$, the new point can be assigned the next unused label $m$ by
Invariant I3.

Thus, after restricted-growth relabeling,

$$
B=\{p,q,m\},
$$

which is precisely one of the old-new candidates generated by the
search.

#### Case C: both $q$ and $r$ are new

By first-occurrence labeling, they receive the consecutive labels

$$
m,\qquad m+1.
$$

Hence

$$
B=\{p,m,m+1\},
$$

which is exactly the new-new candidate generated by the search.

Thus in every case the next target block is among the proposed
candidates.

---

### 5.3 The lexicographic condition cannot reject $B$

By Invariant I2, the target blocks through $p$ that have already been
inserted form an initial segment of the lexicographic ordering.

The block $B$ was chosen to be the lexicographically first missing
target block through $p$.

Therefore $B$ comes immediately after the already inserted target
segment, and the lexicographic filter cannot exclude it.

After inserting $B$, Invariant I2 continues to hold.

---

### 5.4 The degree condition cannot reject $B$

The partial state consists only of blocks of $\mathcal C$.

Because every point of $\mathcal C$ has degree exactly three, adding
another block of $\mathcal C$ can never give a point degree greater than
its target degree three.

Hence the degree filter cannot reject $B$.

---

### 5.5 The repeated-pair condition cannot reject $B$

The target configuration $\mathcal C$ is linear.

Therefore no pair of points occurs in two distinct target blocks.

Since both the current partial state and $B$ consist only of target
blocks, adding $B$ cannot repeat a pair.

Hence the repeated-pair filter cannot reject $B$.

---

### 5.6 The cycle filters cannot reject $B$

The configuration $\mathcal C$ contains neither a Berge cycle of length
$4$ nor one of length $8$.

The current partial state together with $B$ is a subconfiguration of
$\mathcal C$.

Consequently adding $B$ cannot create either forbidden Berge cycle.

By the assumed soundness of the cycle-rejection predicates, neither
cycle filter can reject $B$.

Notice that this step requires only

$$
\text{rejection}
\Longrightarrow
\text{a genuine forbidden cycle exists}.
$$

The converse implication is not needed for coverage.

---

### 5.7 Preservation of the invariants

After inserting $B$, either point $p$ remains deficient or it reaches
degree three.

If it remains deficient, then the newly inserted blocks through $p$
still form a lexicographic initial segment, so Invariant I2 is
preserved.

If $p$ becomes saturated, the search moves to the least later deficient
introduced point. Every target block whose least point is $p$ has then
been inserted, so Invariant I1 advances to the new value of $p$.

Whenever the block introduces one or two previously unseen target
points, they receive the next unused labels, preserving Invariant I3.

Thus all three invariants continue to hold.

By induction, the search contains a branch consisting entirely of
blocks of $\mathcal C$.

---

## 6. Every target point is eventually introduced

It remains to show that this branch cannot terminate after completing
all currently introduced points while leaving some point of
$\mathcal C$ unseen.

Suppose, for contradiction, that the set $S$ of introduced target
points is a proper nonempty subset of the point set of $\mathcal C$,
and every point of $S$ has already received all three of its target
blocks.

Every target block incident with a point of $S$ has therefore already
been inserted.

Whenever such a block was inserted, all of its points were introduced.

Hence every block meeting $S$ is entirely contained in $S$.

Therefore no incidence edge of the Levi graph joins a point of $S$ to a
block containing a point outside $S$.

The incidences supported on $S$ consequently form a union of connected
components of the Levi graph.

Because the normalized root is nonempty, this union is nonempty.
Because $S$ was assumed proper, it is not the whole Levi graph.

This contradicts the connectedness of $\mathcal C$.

Therefore every target point is eventually introduced.

---

## 7. Safety of the point cap

The target has

$$
v\le M
$$

points.

By Invariant I3, target points receive labels in first-occurrence order

$$
0,1,\ldots,v-1.
$$

Hence the target branch never requires the introduction of a label
greater than or equal to $M$.

The point cap therefore cannot remove the target branch.

---

## 8. Safety of the block cap

A symmetric $v_3$-configuration has exactly $v$ blocks.

If

$$
v<M,
$$

the target branch completes before the block count can reach $M$.

If

$$
v=M,
$$

then after the $M$-th target block has been inserted, every target point
has degree three and the complete configuration is present.

The implementation tests for completion before applying the
block-count cutoff.

Thus the state with exactly $M$ target blocks is recorded as a
completion rather than discarded.

Therefore the block cap cannot remove the target branch.

---

## 9. Completion

Eventually all $v$ target points are introduced and all $v$ target
blocks are inserted.

Every point then has degree three, and the partial state is exactly the
chosen restricted-growth labeling of $\mathcal C$.

The search therefore reaches a completed state isomorphic to
$\mathcal C$.

This proves the proposition.

$\square$

---

## 10. What this proposition proves

The proposition establishes the following implication:

> If a connected linear symmetric $v_3$-configuration with $v\le M$
> contains a Berge triangle and avoids Berge cycles of lengths $4$ and
> $8$, then the triangle-rooted search cannot miss it solely because of
> the restricted-growth labeling, candidate ordering, structural
> filters, or point/block caps.

For the computation in this project,

$$
M=34.
$$

Thus, once the graph-theoretic reduction establishes that every
relevant cubic bipartite graph on at most $68$ vertices contains a
$C_6$, the two triangle-rooted searches cover all remaining candidates.

---

## 11. What this proposition does not prove

This proposition does **not** by itself prove that

- the C++ implementation exactly realizes the abstract candidate
  generator;
- `creates_C8(t)` is a sound implementation of Berge-$4$ rejection;
- `creates_C16(t)` is a sound implementation of Berge-$8$ rejection;
- the reported cap-34 computation actually exhausted both search trees;
- the static certificate verifier is correct.

Those are separate claims.

In this project they are addressed respectively by

1. direct source inspection against the immutable upstream commit;
2. the $C_8$ oracle correctness lemma;
3. the $C_{16}$ oracle correctness lemma;
4. the independently reproduced cap-34 DFS counters and transcript
   hashes;
5. the static certificate and its independent verifier.

Keeping these claims separate prevents computational observations or
implementation assumptions from being silently used as mathematical
proof steps.