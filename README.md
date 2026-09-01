# Text and Code Editor Engine

**Problem 4** · **Zaheer Jafar (F2024-0541)**  
**GitHub:** Zaheerjaffar23 · **Email:** f2024-0541@bnu.edu.pk

A line-oriented editor engine built only from course data structures:

| Requirement | Structure |
|-------------|-----------|
| Document lines + cursor (Core 1–3) | Doubly linked list |
| Undo / redo (Core 4) | Two stacks |
| Bracket matching (Core 5) | One stack |
| Multi-user edits (Core 6) | FIFO queue |
| Named bookmarks (Advanced A) | Self-built hash table |
| Word index (Advanced B) | Same hash table, keyed by word |

Proposal: [`docs/D1_Proposal.md`](docs/D1_Proposal.md)  
Design / Big-O: [`docs/D2_Design.md`](docs/D2_Design.md)

## Build and run

Requires a C++17 compiler (`g++` or `clang++`).

```bash
make                 # bin/editor
make demo            # walkthrough on data/sample/manuscript.txt (develop here)
make run             # interactive CLI on the sample
make demo-full       # same walkthrough on data/full/manuscript.txt (graded file)
```

Interactive session:

```bash
./bin/editor data/sample/manuscript.txt
> status
> print 2
> insert a new line of text
> undo
> bookmark add intro
> find cursor
> brackets
> enqueue alice insert 10 hello from alice
> process
> help
> quit
```

`manuscript.txt` has one unclosed bracket. `manuscript_clean.txt` is the same document with every pair closed — the checker must stay silent on that file.

## Project layout

```
README.md
include/     headers (.hpp)     ← same role as a folder named H/
src/         sources (.cpp)    ← same role as a folder named CPP/
data/        official sample/ and full/ manuscripts
docs/        D1 proposal and D2 design
tools/       optional extra generator (not used for grading)
```

There is no separate `H/` next to `include/`: those two names mean the same thing. Use one header folder, one source folder, plus `data/` and `README.md`. A `tests/` folder is a good addition for graders; it is not required by the data pack.

All four containers are implemented by hand (linked nodes + separate chaining).
The STL is used only for `string`, `vector` (bucket array / line-number lists),
and I/O — not for `list`, `stack`, `queue`, or `unordered_map`.
