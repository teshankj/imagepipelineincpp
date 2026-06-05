# Image Preprocessing Pipeline

A lightweight, high-performance image preprocessing pipeline implemented in modern C++ (C++17). This project is designed to handle low-level image operations, starting with structural validation and moving toward robust pixel transformations.

## Current Version: v0.1

### Features Implemented
* **Folder Iteration:** Dynamically scans a target directory (`images/`) using `std::filesystem`.
* **Stage 1 (Format Validation):** Verifies filename extension bounds (`.ppm` / `.PPM`) and inspects the inner header for valid magic numbers (`P3` or `P6`).
* **Stage 2 (Header Parsing & Metadata Extraction):** * Implements lookahead byte parsing via `file.peek()` to robustly skip multi-line image comments (`#`).
  * Extracts structural metadata (`width`, `height`, `maxColorValue`) into a modular `ImageMetaData` structure passed by reference.
* **Stage 3 (Image resizing to 256 *256 ):*** * resize images using Nearest Neighbor and Bilinear Interpolation.

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
├── imageopp.cpp              # Core execution for stage 1 & 2 pipeline code
└── evalsizer.cpp             # Core execution for stage 3 pipeline code
