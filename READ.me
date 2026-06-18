# 🧩 High-Performance Sudoku Solver in C

A high-performance Sudoku solver developed in C, capable of processing multi-sized boards ranging from $4 \times 4$ up to $49 \times 49$. The project is designed with a modular structure and implements strategies from classic brute-force backtracking to advanced bitwise optimization and transposition tables to ensure maximum CPU and memory efficiency.

---

## 🚀 Features and Algorithms

The solver engine features different resolution strategies (*backends*) that can be selected at runtime:

* **Strict Brute Force (Classic Backtracking):** The baseline trial-and-error algorithm, isolated by recursive stack frames. Excellent for integrity validation and performance benchmarking.
* **Vector-Based Backtracking (Array Tracking):** An optimization that replaces linear verification loops with fast lookup arrays allocated dynamically.
* **Bitmasking Optimization:** Uses bitwise operations (`AND`, `OR`, `NOT`, `XOR`) alongside compiler-native builtins like `__builtin_ffsll` to reduce candidate selection and move validation to single-cycle CPU operations ($O(1)$).
* **Zobrist Hashing & Transposition Tables:** Allocates pseudo-random keys to memorize board states (hashing), preventing the engine from re-evaluating dead-end branches already explored in the search tree.

---

## 📂 Project Structure

The project adopts a modular organization pattern to prevent scope pollution and linker conflicts in C:

```text
├── include/
│   ├── config.h      # UI/Lobby configuration structures
│   └── solver.h      # Core engine signatures, BitTrack, Track, and Zobrist structs
├── solvers/
│   ├── config.c      # Input management and file loading routines
│   └── solver.c      # Algorithm implementations (Brute, Bitmask, Vectors)
├── sheet*.txt        # Pre-configured board files (4x4, 9x9... up to 49x49)
├── main.c            # Program entry point and backend dispatching
└── Makefile          # Automated compilation script
