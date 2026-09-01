# D1 — Project Proposal

**Problem 4 — Text and Code Editor Engine**  
**Team member:** Zaheer Jafar (F2024-0541)

## 1. Problem Chosen and Justification

**Problem:** Problem 4 — Text and Code Editor Engine

We chose this problem because it maps cleanly onto data structures our team already has practical experience building: a linked list for sequential, cursor-based data; a stack-driven undo mechanism; and stack-based bracket validation. This lets us focus our effort on correctness, performance proof, and clean design rather than learning an unfamiliar structure from scratch under time pressure.

The problem also has a clear, unambiguous mapping between each functional requirement and a specific structure — cursor movement needs O(1) bidirectional traversal, undo/redo needs strict LIFO reversal, and bracket checking needs a single-pass stack scan — which keeps our design decisions well-justified rather than arbitrary.

## 2. Advanced Items Selected

- **Advanced A — Named bookmarks:** create, jump to by name, and delete, in O(1) via a self-built hash table.
- **Advanced B — Word index:** for any word, list every line number where it appears, using the same hash table keyed by word instead of bookmark name.

We are choosing A and B over C (alphabetical word range query) because both reuse a single hash table implementation, minimizing the number of net-new structures the team has to build, test, and document while still meeting the two-of-three requirement. We may attempt C afterward for the bonus if time allows.

## 3. Team Member Roles

| Team Member   | Roll Number | Primary Role |
|---------------|-------------|--------------|
| Zaheer Jafar  | F2024-0541  | Document engine: Doubly Linked List (lines + cursor), Undo/Redo manager, Bracket Checker; D2 design doc for these structures |

## 4. Kinds of Data the System Holds

| Kind of Data | Description |
|--------------|-------------|
| Document lines | Ordered sequence of text lines (strings) making up the loaded document — the `manuscript.txt` file has ~20,000 lines and ~120,000 words. |
| Cursor position | A single pointer/reference into the line list representing the user's current line. |
| Edit records | Reversible records of insert/delete/replace operations, used to drive undo and redo. |
| Bracket stack entries | Open-bracket characters with their line and column, held only while a bracket is still unmatched. |
| Multi-user edit requests | Incoming edit requests from several simulated users, queued strictly in arrival order. |
| Bookmarks | Named references to specific lines (advanced item), up to ~1,000 entries. |
| Word index | Mapping from each of the ~8,000 distinct words to the line numbers where it appears (advanced item). |

## 5. Planned Structures (Preview)

- **Doubly linked list** — document lines and cursor (Core #1–3)
- **Two stacks** — undo and redo history (Core #4)
- **One stack** — bracket matching (Core #5)
- **Custom FIFO queue** — multi-user edit requests (Core #6)
- **Self-built hash table** — bookmarks and word index (Advanced A & B)

Full Big-O justification and rejected alternatives will be presented in the D2 Design Document (Week 6).
