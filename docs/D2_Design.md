# D2 — Design Document (structures used by the engine)

**Problem 4 — Text and Code Editor Engine**  
**Author:** Zaheer Jafar (F2024-0541)

This note records the Big-O justification that D1 deferred to Week 6. The
implementation lives under `include/` (structures) and `src/` (editor engine).

## Core #1–3 — Doubly linked list (document + cursor)

**Job.** Hold ~20,000 ordered lines and a cursor that can move to the previous
or next line without scanning the file.

**Choice.** Doubly linked list of `std::string` nodes, with the cursor stored
as a node pointer plus a 0-based index.

| Operation | Time | Why |
|-----------|------|-----|
| move up / down | O(1) | `cursor = cursor->prev` / `next` |
| insert after cursor | O(1) | relink four pointers |
| delete current line | O(1) | relink neighbors |
| goto line *k* | O(n) | walk from the current cursor |
| load file | O(n) | one `pushBack` per line |

**Rejected.** A dynamic array of lines gives O(1) index but O(n) insert/delete
in the middle (shifting ~20k strings). A singly linked list makes `moveUp`
O(n). An OS text buffer (gap buffer, piece table) is the right production
choice but is not a course structure we can claim as “built by us.”

## Core #4 — Two stacks (undo / redo)

**Job.** Reverse the last edit, then optionally replay it. A new user edit
must discard the redo branch.

**Choice.** Two linked-list stacks of `EditRecord`. Each record stores the
edit type, the 1-based line number, and the before/after text.

| Operation | Time |
|-----------|------|
| record edit / undo / redo | O(1) plus the underlying line edit |
| clear redo on new edit | O(k) in the size of the redo stack |

**Rejected.** A queue would replay in arrival order, not reverse order. An
array that we scan backwards still needs LIFO discipline; the stack makes
that invariant structural rather than conventional.

## Core #5 — One stack (brackets)

**Job.** Decide whether `()`, `[]`, and `{}` are balanced across the whole
document.

**Choice.** One left-to-right pass. Opening brackets are pushed with
`(line, column)`; a closer pops and must match. First mismatch or leftover
opener fails the report.

| Operation | Time | Extra space |
|-----------|------|-------------|
| check document | O(C) characters | O(openers) |

**Rejected.** Recursion on nested blocks is equivalent but blows the call
stack on a 20k-line worst case. Pairing by regex cannot handle nesting.

## Core #6 — FIFO queue (multi-user edits)

**Job.** Several simulated users submit insert/delete/replace requests.
They must run in arrival order, never “whoever we happen to scan first.”

**Choice.** Linked-list queue: enqueue at tail, dequeue at head.

| Operation | Time |
|-----------|------|
| enqueue / dequeue / apply one request | O(1) plus the line edit |

**Rejected.** A stack would apply the latest user first. A vector of
requests is fine at this size, but removing from the front is O(n) unless
we keep a head index — which is a queue in disguise.

## Advanced A — Named bookmarks (hash table)

**Job.** Up to ~1,000 named jumps: create, jump, delete by name in O(1).

**Choice.** Separate-chaining hash table, `string → {node*, line}`. The node
pointer makes the jump a pointer assign; the stored line is adjusted when
lines are inserted or deleted so `cursorIndex` stays correct without an
O(n) walk.

Average O(1) insert/find/erase at load factor 0.75 with power-of-two
buckets and djb2-style `std::hash<std::string>`. Rehash is O(n) amortized.

**Rejected.** A list of `(name, line)` pairs is O(n) to jump. A BST on names
is O(log n) and does not match the “hash table” advanced item.

## Advanced B — Word index (same hash table)

**Job.** For any word, list every line number that contains it (~8,000
distinct words, ~120,000 occurrences).

**Choice.** Same `HashTable`, keyed by normalized word, value =
`vector<line>`. Lookups are average O(1) then O(k) to copy *k* hits.

Edits update the index: insert shifts line numbers `≥ L` and adds the new
line’s tokens; delete removes that line’s tokens and shifts `> L`; replace
swaps tokens on that line only. Load rebuilds once in O(total words) so we
do not pay quadratic shifts while reading the file.

**Rejected.** Advanced C (alphabetical range query) needs an ordered tree
and a second structure. A and B share one hash table, which is why they
were selected in D1.

## Mapping back to D1 data kinds

| Data | Structure |
|------|-----------|
| Document lines | `DoublyLinkedList<string>` |
| Cursor | node pointer + index |
| Edit records | `Stack<EditRecord>` × 2 |
| Bracket entries | `Stack<BracketOpen>` (transient) |
| Multi-user requests | `Queue<EditRequest>` |
| Bookmarks | `HashTable<string, Bookmark>` |
| Word index | `HashTable<string, vector<size_t>>` |
