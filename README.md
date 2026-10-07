# Compiler Syntax Token Unique Extractor & Categorizer

**Group 5 &bull; Term 1 C++ Course Project**  
*Core STL Concepts: `std::set`, `std::vector`, `std::string`, Red-Black Tree Bounds & Lexical Analysis*

---

## Overview

This project is a high-performance **Compiler Syntax Token Unique Extractor & Categorizer** developed in modern C++ (C++17) with an interactive **3D Monochromatic Web Visualizer**. 

The system ingests raw source code into a dynamic `std::vector<std::string>` and uses `std::set<std::string>` to isolate distinct syntax keywords in strict lexicographical order. It calculates original keyword usage frequency metrics and provides an empirical benchmarking engine validating the logarithmic insertion bounds ($O(\log N)$) and duplicate rejection mechanics of the underlying Red-Black Binary Search Tree.

---

## Tech Stack & Architectural Rationale

| Layer / Component | Technology | Rationale & Purpose |
| :--- | :--- | :--- |
| **Core Parser & Algorithm** | **C++17 (MSVC / ISO C++)** | Selected as the core language for the C++ course. Offers zero-overhead abstractions, deterministic memory management (RAII), high-performance string scanning, and direct access to the Standard Template Library (STL). |
| **Token Ingestion Buffer** | **`std::vector<std::string>`** | Dynamic array with contiguous memory layout. Provides amortized $O(1)$ `push_back()` token ingestion, high cache line locality, and sequential indexing matching source code token ordering. |
| **Unique Filtering & Sorting** | **`std::set<std::string>`** | Self-balancing Red-Black Binary Search Tree (RB-BST). Guarantees strict lexicographical ordering on traversal ($Left \to Node \to Right$) and evaluates duplicate rejection in $O(\log U)$ time via `insert().second` without memory reallocation. |
| **Lexical Scanner** | **Custom C++ DFA (Deterministic Finite Automaton)** | Character-by-character scanner without regex overhead. Accurately handles multi-character operators (`::`, `->`, `++`, `<<`), block comments (`/* ... */`), single-line comments (`//`), escaped string literals, and preprocessor directives. |
| **Complexity Benchmarking** | **`std::chrono::high_resolution_clock` + C++ STL** | Empirically tests $N \in [500, 30\,000]$ tokens, measuring and comparing execution times for naive vector search ($O(N^2)$), `std::set` ($O(N \log U)$), and `std::unordered_set` ($O(N)$). Proves $h \le 2\log_2(U+1)$ maximum height bound. |
| **Build Automation** | **Batch (`build.bat`) & CMake (`CMakeLists.txt`)** | Dual build support: native one-click Windows MSVC compilation using Developer Command Prompt `cl.exe`, plus cross-platform CMake configuration for Linux / macOS portability. |
| **Localhost Bridge Server** | **Python 3 (`http.server` & `subprocess`)** | Lightweight zero-dependency HTTP server (`server.py`) bridging the compiled C++ executable with the frontend via standard JSON REST endpoints (`/api/analyze`, `/api/benchmark`, `/api/sample`). |
| **3D Visualization Engine** | **Three.js (WebGL)** | Renders an interactive 3D balanced binary syntax tree directly in the browser. Users can orbit, pan, and zoom in 3D to inspect keywords, height bounds, and binary tree levels in real time. |
| **Frontend Interface** | **Vanilla HTML5, CSS3, JavaScript** | Zero-dependency, framework-free client interface. Adopts a **minimalist niche monochromatic aesthetic** (pure black `#000000`, white `#ffffff`, and gray shades) inspired by Swiss typography and technical hardware design. Completely free of generic AI gradient clichés. |

---

## Key Deliverables & Academic Findings

### 1. Lexicographical Sorting & Frequency Metrics
- Standard C++ keywords are parsed and isolated in ascending alphabetical order.
- Calculates total token counts, distinct keyword counts, keyword density percentage, and duplicate suppression ratio.
- Retains exact original occurrences using frequency mapping while deduplicating in `std::set`.

### 2. $O(\log N)$ Logarithmic Bounds Evaluation
- **Height Invariant**: Red-Black Tree maximum height is strictly $h \le 2 \log_2(U + 1)$. For 9,127 unique keys, height is $\le 28$.
- **Empirical Scaling**: Vector linear search slows down quadratically by $42.7\times$ over $6\times$ input growth ($3.28\text{ ms} \to 140.4\text{ ms}$), while `std::set` scales smoothly by only $6.38\times$ ($0.51\text{ ms} \to 3.25\text{ ms}$), achieving over **$43\times$ speedup**.

---

## Directory Structure

```
Compiler-syntax-token-extractor-categorizer/
├── CMakeLists.txt                         # Cross-platform CMake configuration
├── build.bat                              # MSVC 2022 Windows compilation script
├── run_tests.bat                          # Automated test runner script
├── start_server.bat                       # Helper script to launch localhost server
├── server.py                              # Python REST server bridge
├── PROJECT_REPORT.md                      # Comprehensive academic project report
├── include/
│   ├── Token.hpp                          # Token, TokenType, KeywordCategory
│   ├── Tokenizer.hpp                      # Lexical scanner & vector ingestion
│   ├── KeywordExtractor.hpp               # std::set unique isolation & metrics
│   ├── SyntaxCategorizer.hpp              # Syntax taxonomy & category breakdown
│   ├── ComplexityEvaluator.hpp            # Empirical benchmarking engine
│   └── JsonExporter.hpp                   # JSON & HTML data serialization
├── src/
│   ├── Token.cpp                          # String conversion helpers
│   ├── Tokenizer.cpp                      # Finite automaton scanning implementation
│   ├── KeywordExtractor.cpp               # C++ keyword dictionary & set logic
│   ├── SyntaxCategorizer.cpp              # Syntax taxonomy classification
│   ├── ComplexityEvaluator.cpp            # Timing benchmarks & RB-tree height
│   ├── JsonExporter.cpp                   # JSON export & dashboard generator
│   └── main.cpp                           # Interactive CLI menu & flags driver
├── web/
│   ├── index.html                         # Minimalist monochromatic HTML interface
│   ├── style.css                          # High-contrast black & white styling
│   ├── app.js                             # Client-side 3D WebGL logic & API bridge
│   └── three.min.js                       # Local Three.js WebGL engine bundle
├── samples/
│   ├── sample1_simple.cpp                 # Prime checker sample
│   ├── sample2_complex.cpp                # Advanced OOP, templates, RAII, exceptions
│   └── sample3_keywords_stress.cpp        # Repeated keyword duplicate stress test
├── tests/
│   └── test_runner.cpp                    # Automated unit verification suite
└── export/                                # Output directory for JSON & dashboard
```

---

## Quickstart Guide

### 1. Build the C++ Executables
```cmd
build.bat
```
Produces `bin/syntax_analyzer.exe` and `bin/test_runner.exe`.

### 2. Run Automated Verification Tests
```cmd
run_tests.bat
```

### 3. Launch Interactive Terminal CLI
```cmd
.\bin\syntax_analyzer.exe
```
Or run directly via command-line flags:
```cmd
.\bin\syntax_analyzer.exe --file samples\sample2_complex.cpp
.\bin\syntax_analyzer.exe --benchmark
.\bin\syntax_analyzer.exe --demo
```

### 4. Launch the 3D Monochromatic Web Visualizer
```cmd
python server.py
REM Or double-click start_server.bat
```
Navigate to: **`http://localhost:8080`**

---

## Academic Report
The full project report containing the complete source code and line-by-line design explanations is available in [`PROJECT_REPORT.md`](PROJECT_REPORT.md).
