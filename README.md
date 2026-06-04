# Image Preprocessing Pipeline

A lightweight, high-performance image preprocessing pipeline implemented in modern C++ (C++17). This project is designed to handle low-level image operations, starting with structural validation and moving toward robust pixel transformations.

## Current Version: v0.1

### Features Implemented
* **Folder Iteration:** Dynamically scans a target directory (`images/`) using `std::filesystem`.
* **Stage 1 (Format Validation):** Verifies filename extension bounds (`.ppm` / `.PPM`) and inspects the inner header for valid magic numbers (`P3` or `P6`).
* **Stage 2 (Header Parsing & Metadata Extraction):** * Implements lookahead byte parsing via `file.peek()` to robustly skip multi-line image comments (`#`).
  * Extracts structural metadata (`width`, `height`, `maxColorValue`) into a modular `ImageMetaData` structure passed by reference.

---

## Getting Started

### Prerequisites
* A compiler supporting **C++17** or higher (e.g., MinGW-w64 GCC/G++).
* **VS Code** with the official C/C++ Extension Pack installed.

### Project Structure
```text
├── images/               # Put your sample test .ppm images here
├── .gitignore            
├── README.md             
└── imageopp.cpp              # Core execution pipeline source code
