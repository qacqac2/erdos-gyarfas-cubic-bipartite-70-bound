# Correctness of the \(C_8\) and \(C_{16}\) Pruning Oracles

## Purpose

This document proves the correctness of the two cycle-detection
predicates used by the triangle-rooted restricted-growth search:

```text
creates_C8(t)
creates_C16(t)
```

The reference implementation is

```text
research/src/triangle_root_universal_search.cpp
```

from the immutable upstream commit

```text
67adc92d31d5d1edfbd0a7b845dc232e31c412cd
```

whose SHA-256 is

```text
e778648bc3d5b0a19c860bec1371a542c464652693c2cbc4c2cd5ef05f314f41
```

The proofs below concern the mathematical meaning of the predicates in
that fixed source.

They are logically separate from the restricted-growth coverage proof
in `coverage_proposition.md`.

For coverage, only **soundness** is required:

> if an oracle rejects a proposed block, then the corresponding
> forbidden cycle really exists.

The arguments below establish the stronger statements that both
predicates are exact on structurally admissible search states.

---

## 1. Search-state assumptions

Let

$$
\mathcal S=(P,\mathcal B)
$$

be a current partial incidence structure in the search.

Every existing block is a triple of distinct points.

The structural filters maintain the following properties.

1. Every point has current degree at most three.
2. No pair of distinct points occurs together in two existing blocks.

Thus the current incidence structure is linear.

Let

$$
t=\{a,b,c\}
$$

be a candidate block that has already passed the structural
degree-and-pair test but has not yet been inserted.

Consequently, adding \(t\) preserves linearity.

In particular, \(t\) meets every existing block in at most one point.

Let \(L(\mathcal S)\) denote the Levi graph of the current partial
configuration.

Its two vertex classes are:

- point vertices;
- block vertices corresponding to the existing blocks in
  \(\mathcal B\).

When \(t\) is inserted, it gives a new block vertex \(T\) adjacent to
the three point vertices \(a,b,c\).

Throughout this document, saying that adding \(t\) **creates** a cycle
means that the enlarged Levi graph contains such a cycle using the new
block vertex \(T\).

Any cycle already present before \(t\) is inserted is irrelevant to the
local predicate.

---

# Part I. The \(C_8\) oracle

## 2. Source-level description

The function

```text
creates_C8(t)
```

enumerates ordered triples of pairwise distinct existing blocks

$$
B_i,\ B_j,\ B_h.
$$

For each triple it computes

$$
p_0=t\cap B_i,
$$

$$
p_1=B_i\cap B_j,
$$

$$
p_2=B_j\cap B_h,
$$

and

$$
p_3=B_h\cap t.
$$

The helper

```text
unique_intersection(x,y)
```

returns the common point when two blocks have exactly one common point,
and otherwise reports failure.

The oracle accepts a witness only when all four intersections exist and

$$
p_0,p_1,p_2,p_3
$$

are pairwise distinct.

It then returns `true`.

---

## 3. \(C_8\) soundness

### Lemma 1

If

```text
creates_C8(t)
```

returns `true`, then adding \(t\) creates a simple cycle of length \(8\)
in the Levi graph.

Equivalently, adding \(t\) creates a Berge cycle of length \(4\).

### Proof

Suppose the predicate returns `true`.

Then it has found three pairwise distinct existing blocks

$$
B_i,\ B_j,\ B_h
$$

and four pairwise distinct points

$$
p_0,p_1,p_2,p_3
$$

such that

$$
p_0\in t\cap B_i,
$$

$$
p_1\in B_i\cap B_j,
$$

$$
p_2\in B_j\cap B_h,
$$

and

$$
p_3\in B_h\cap t.
$$

After the new block vertex \(T\) corresponding to \(t\) is inserted,
the Levi graph therefore contains the closed alternating sequence

$$
T,
p_0,
B_i,
p_1,
B_j,
p_2,
B_h,
p_3,
T.
$$

The four block vertices

$$
T,\ B_i,\ B_j,\ B_h
$$

are distinct.

The four point vertices

$$
p_0,p_1,p_2,p_3
$$

are also distinct by the explicit checks performed by the predicate.

Point vertices and block vertices belong to opposite parts of the Levi
graph, so no point vertex can coincide with a block vertex.

Hence the sequence contains exactly eight distinct vertices before
returning to \(T\).

It is therefore a simple \(8\)-cycle.

Thus

$$
\texttt{creates\_C8(t)=true}
\Longrightarrow
\text{adding }t\text{ creates a }C_8.
$$

\(\square\)

---

## 4. \(C_8\) completeness

### Lemma 2

If adding \(t\) creates a simple \(C_8\) containing the new block
vertex \(T\), then

```text
creates_C8(t)
```

returns `true`.

### Proof

Suppose adding \(t\) creates a simple \(8\)-cycle containing \(T\).

Because the Levi graph is bipartite, the cycle alternates between four
block vertices and four point vertices.

Besides \(T\), let the three old block vertices on the cycle, in cyclic
order, be

$$
B_i,\ B_j,\ B_h.
$$

Let the intervening point vertices be

$$
p_0,p_1,p_2,p_3.
$$

The cycle therefore has the form

$$
T,
p_0,
B_i,
p_1,
B_j,
p_2,
B_h,
p_3,
T.
$$

Since the cycle is simple,

$$
B_i,\ B_j,\ B_h
$$

are pairwise distinct and

$$
p_0,p_1,p_2,p_3
$$

are pairwise distinct.

The required incidences give

$$
p_0\in t\cap B_i,
$$

$$
p_1\in B_i\cap B_j,
$$

$$
p_2\in B_j\cap B_h,
$$

and

$$
p_3\in B_h\cap t.
$$

Because the current state and the structurally admissible candidate are
linear, each of these intersections consists of exactly one point.

The implementation enumerates every ordered triple of distinct existing
blocks.

In particular, it eventually examines

$$
(B_i,B_j,B_h).
$$

For that triple, `unique_intersection` returns exactly

$$
p_0,p_1,p_2,p_3.
$$

The four distinctness tests all succeed.

Therefore the function returns `true`.

Hence

$$
\text{adding }t\text{ creates a }C_8
\Longrightarrow
\texttt{creates\_C8(t)=true}.
$$

\(\square\)

---

## 5. Exactness of the \(C_8\) oracle

Combining Lemmas 1 and 2 gives:

### Theorem 3

For every structurally admissible candidate block \(t\),

$$
\texttt{creates\_C8(t)=true}
\iff
\text{adding }t\text{ creates a simple }C_8
$$

where the new \(C_8\) contains the new block vertex corresponding to
\(t\).

Equivalently,

$$
\texttt{creates\_C8(t)=true}
\iff
\text{adding }t\text{ creates a Berge }4\text{-cycle}.
$$

For the restricted-growth coverage proof, only the forward implication
is needed.

---

# Part II. The exact-path routine

## 6. Levi-graph representation used by `exact_path`

The \(C_{16}\) oracle works directly in the old Levi graph.

The source indexes point vertices by

$$
0,\ldots,V-1,
$$

and an existing block with vector index `id` by

$$
V+\text{id}.
$$

For a point vertex, its adjacent block vertices are obtained from the
stored incidence list.

For a block vertex, its three adjacent point vertices are obtained from
the stored block triple.

Thus one recursive step of `exact_path` is exactly one incidence edge
of the current Levi graph.

---

## 7. Meaning of the `seen` array

The routine carries an array

```text
seen
```

indexed by both point and block vertices.

Before the search starts, the initial point is marked seen.

Whenever the recursion moves to another vertex, that vertex is marked
before the recursive call and unmarked when backtracking.

Therefore, along any active recursive branch, no point vertex and no
block vertex can occur twice.

Every branch explored by the routine is consequently a simple path in
the old Levi graph.

This is important: the routine is not merely searching for a walk of
length \(14\).

It searches for a **simple path** of length \(14\).

---

## 8. Exact target depth

The routine uses

```text
needed = 14
```

and succeeds at the terminal depth exactly when

```text
current == target
```

at depth \(14\).

If the target is reached before depth \(14\), the branch is rejected.

Thus a successful branch has exactly fourteen incidence edges.

The target cannot be used as an internal vertex and then revisited at
depth \(14\), both because early arrival is forbidden and because
visited vertices cannot be repeated.

---

## 9. Correctness of `exact_path`

### Lemma 4

For two distinct point vertices \(x\) and \(y\) in the current Levi
graph,

```text
exact_path(x,y,0,seen)
```

returns `true` if and only if the current Levi graph contains a simple
path of exactly \(14\) edges from \(x\) to \(y\).

Here the initial `seen` array marks \(x\) and no other vertex.

### Proof

#### Forward direction

Suppose `exact_path` returns `true`.

Each recursive call follows an actual incidence edge of the current
Levi graph.

The `seen` condition prevents any vertex from appearing twice on the
active branch.

The branch is therefore a simple path.

The routine reports success only at recursion depth \(14\), and only
when the current vertex equals \(y\).

Thus the successful recursive branch is a simple path of exactly
fourteen edges from \(x\) to \(y\).

#### Reverse direction

Now suppose the current Levi graph contains a simple path

$$
x=v_0,v_1,\ldots,v_{14}=y
$$

of exactly fourteen edges.

At every recursive level, `exact_path` enumerates all neighbors of the
current vertex.

Therefore it considers the next edge

$$
v_i v_{i+1}
$$

of this path.

Because the path is simple,

$$
v_{i+1}
$$

has not already been marked `seen`.

Because \(y\) occurs only as the last vertex of the simple path, the
early-target rule does not reject this branch.

Thus the recursion can follow the entire sequence

$$
v_0,v_1,\ldots,v_{14}.
$$

At depth \(14\), the current vertex is \(y\), so the routine returns
`true`.

Therefore

$$
\texttt{exact\_path(x,y)=true}
\iff
\text{there is a simple length-14 path from }x\text{ to }y.
$$

\(\square\)

---

# Part III. The \(C_{16}\) oracle

## 10. Source-level description

For

$$
t=\{a,b,c\},
$$

the function

```text
creates_C16(t)
```

examines all three unordered pairs

$$
\{a,b\},\qquad
\{a,c\},\qquad
\{b,c\}.
$$

For each pair it starts a fresh `seen` array and asks whether
`exact_path` finds a simple path of exactly \(14\) edges between the two
points in the old Levi graph.

The predicate returns `true` if such a path exists for at least one of
the three pairs.

---

## 11. \(C_{16}\) soundness

### Lemma 5

If

```text
creates_C16(t)
```

returns `true`, then adding \(t\) creates a simple cycle of length
\(16\) in the Levi graph.

Equivalently, it creates a Berge cycle of length \(8\).

### Proof

Suppose `creates_C16(t)` returns `true`.

Then for some two distinct members

$$
x,y\in t,
$$

the routine `exact_path` has found a simple path of exactly fourteen
edges in the old Levi graph:

$$
x=v_0,v_1,\ldots,v_{14}=y.
$$

Insert the new block vertex \(T\) corresponding to \(t\).

By definition of incidence,

$$
T\sim x
$$

and

$$
T\sim y.
$$

Therefore the enlarged graph contains

$$
T,x=v_0,v_1,\ldots,v_{14}=y,T.
$$

The old path is simple.

The new vertex \(T\) did not exist in the old graph and hence does not
occur on the path.

Thus this closed sequence contains

$$
1+15=16
$$

distinct vertices before returning to \(T\).

It is therefore a simple \(16\)-cycle.

Hence

$$
\texttt{creates\_C16(t)=true}
\Longrightarrow
\text{adding }t\text{ creates a }C_{16}.
$$

\(\square\)

---

## 12. \(C_{16}\) completeness

### Lemma 6

If adding \(t\) creates a simple \(C_{16}\) containing the new block
vertex \(T\), then

```text
creates_C16(t)
```

returns `true`.

### Proof

Suppose the enlarged Levi graph contains a simple \(16\)-cycle using
\(T\).

Every vertex on a simple cycle has exactly two incident cycle edges.

Therefore the cycle uses exactly two of the three incidence edges from
\(T\) to the members of \(t\).

Call their point endpoints

$$
x,y\in t.
$$

Delete \(T\) and those two cycle edges from the \(16\)-cycle.

What remains is a simple path in the old Levi graph from \(x\) to \(y\).

The original cycle had sixteen edges, and the two edges incident with
\(T\) have been removed, so the remaining path has exactly

$$
16-2=14
$$

edges.

The function `creates_C16` examines all three pairs of members of
\(t\), so it examines the pair

$$
\{x,y\}.
$$

By Lemma 4, `exact_path(x,y)` returns `true`.

Therefore `creates_C16(t)` returns `true`.

Hence

$$
\text{adding }t\text{ creates a }C_{16}
\Longrightarrow
\texttt{creates\_C16(t)=true}.
$$

\(\square\)

---

## 13. Exactness of the \(C_{16}\) oracle

Combining Lemmas 5 and 6 gives:

### Theorem 7

For every structurally admissible candidate block \(t\),

$$
\texttt{creates\_C16(t)=true}
\iff
\text{adding }t\text{ creates a simple }C_{16}
$$

where the new cycle contains the new block vertex corresponding to
\(t\).

Equivalently,

$$
\texttt{creates\_C16(t)=true}
\iff
\text{adding }t\text{ creates a Berge }8\text{-cycle}.
$$

Again, the coverage theorem requires only the forward implication.

---

# Part IV. Edge cases and implementation details

## 14. The third point of \(t\) may lie on the old path

Suppose `creates_C16(t)` detects a length-\(14\) path between two
members

$$
x,y\in t,
$$

while the third member

$$
z\in t
$$

also occurs internally on that old path.

After \(T\) is inserted, the edge

$$
Tz
$$

is then a chord of the resulting \(16\)-cycle.

This does **not** invalidate the cycle.

A simple cycle is allowed to have chords in the ambient graph.

The vertices of the cycle

$$
T,x,\ldots,y,T
$$

remain distinct, so it is still a simple \(C_{16}\).

Therefore the oracle correctly does not forbid the third member of
\(t\) from appearing internally on the old path.

---

## 15. Why the candidate must pass the structural filter first

The exactness statement for `creates_C8` uses the fact that the current
state together with \(t\) is linear.

In particular, two blocks cannot share two points.

The implementation checks the degree and repeated-pair conditions
before calling either cycle oracle.

Thus a candidate reaching `creates_C8` or `creates_C16` has already
passed the structural admissibility test.

This ordering is part of the implementation assumptions of the lemmas
above.

---

## 16. The candidate block is not yet inserted

Both cycle predicates are evaluated before the search executes

```text
add(e)
```

for an accepted candidate.

Consequently:

- the old block vector contains only the pre-existing blocks;
- `exact_path` searches only the old Levi graph;
- the new block vertex \(T\) is represented mathematically by the
  candidate \(t\), not by an already inserted graph vertex.

This exactly matches the proofs above:

$$
\text{old path}
+
\text{new block vertex}
=
\text{new cycle}.
$$

---

## 17. Array-index safety relevant to `exact_path`

The `seen` array has size

$$
2V.
$$

Point vertices use indices

$$
0,\ldots,V-1.
$$

An existing block with vector index `id` uses index

$$
V+\text{id}.
$$

Candidate generation occurs only while the number of existing blocks is
strictly less than \(V\).

Thus

$$
0\le \text{id}\le V-1,
$$

and every block-vertex index satisfies

$$
V\le V+\text{id}\le 2V-1.
$$

All point and block vertices used by `exact_path` therefore lie within
the `seen` array.

For the cap-34 run,

$$
V=34,
$$

so this array has

$$
68
$$

entries.

This observation concerns memory/index correctness of the fixed
implementation; it is separate from the graph-theoretic proof of the
path criterion.

---

## 18. Order of the two cycle predicates

For a structurally valid candidate, the search tests

1. `creates_C8(t)`;
2. then, only if the first test is false, `creates_C16(t)`.

Therefore a candidate that creates both a \(C_8\) and a \(C_{16}\) is
counted as a \(C_8\) rejection.

This affects the interpretation of the reported rejection counters, but
not the correctness of the pruning.

The classes

```text
C8 rejection
C16 rejection
```

are therefore ordered search outcomes rather than a disjoint
classification of all cycles created by the candidate.

---

# Part V. Relation to the coverage proof

## 19. What coverage actually needs

Let \(\mathcal C\) be a target configuration having no Berge
\(4\)-cycle and no Berge \(8\)-cycle.

Suppose the restricted-growth induction proposes a genuine target block
\(t\).

Because the partial state plus \(t\) is a subconfiguration of
\(\mathcal C\), adding \(t\) cannot create either forbidden cycle.

By Lemma 1,

$$
\texttt{creates\_C8(t)=true}
\Longrightarrow
\text{a Berge }4\text{-cycle exists}.
$$

Therefore `creates_C8(t)` must be false on the target branch.

Similarly, by Lemma 5,

$$
\texttt{creates\_C16(t)=true}
\Longrightarrow
\text{a Berge }8\text{-cycle exists}.
$$

Therefore `creates_C16(t)` must also be false on the target branch.

Thus neither cycle oracle can falsely prune the target branch.

This is exactly the oracle assumption used in
`coverage_proposition.md`.

The reverse implications proved above are stronger than necessary for
coverage.

---

# Part VI. Trust boundaries

## 20. What has been proved here

For the fixed upstream implementation and for candidates reaching the
cycle tests after structural validation, this document proves:

$$
\boxed{
\texttt{creates\_C8(t)}
\iff
\text{a new }C_8\text{ containing }t\text{ exists}
}
$$

and

$$
\boxed{
\texttt{creates\_C16(t)}
\iff
\text{a new }C_{16}\text{ containing }t\text{ exists}.
}
$$

In particular, both predicates have no false positives.

That is sufficient for the mathematical coverage argument.

---

## 21. What has not been proved here

This document does not establish that

- the restricted-growth generator covers every target configuration;
- the two normalized triangle roots cover every relevant target;
- the point and block caps are safe;
- the DFS actually visited every state prescribed by the search;
- the reported cap-34 counters are genuine;
- the decision-transcript hashes are genuine;
- the static certificate generator is correct;
- the static certificate verifier is correct.

Those are separate claims.

They are addressed by:

1. `coverage_proposition.md`;
2. the fixed-source implementation audit;
3. the fresh upstream cap-34 reproduction;
4. the decision transcripts;
5. the static certificate and its verifier.

The separation is intentional: a proof of search coverage should not
silently assume that a particular implementation of a pruning oracle is
correct, and a proof of oracle correctness should not silently assume
that the enumeration itself is complete.

---

## 22. Conclusion

The two cycle predicates used in the fixed triangle-rooted DFS are exact
for structurally admissible candidate blocks.

Therefore the cycle-pruning layer introduces no false-positive
rejections.

Combined with the independent restricted-growth coverage proposition,
this establishes the mathematical part of the search correctness needed
before the finite exhaustion and certificate results are invoked.