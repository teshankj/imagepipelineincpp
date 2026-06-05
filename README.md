# Image Preprocessing Pipeline

A lightweight, high-performance image preprocessing pipeline implemented in modern C++ (C++17). This project is designed to handle low-level image operations, starting with structural validation and moving toward robust pixel transformations.

## Current Version: v 01

### Features Implemented
* **Folder Iteration:** Dynamically scans a target directory (`images/`) using `std::filesystem`.
* **Stage 1 (Format Validation):** Verifies filename extension bounds (`.ppm` / `.PPM`) and inspects the inner header for valid magic numbers (`P3` or `P6`).
* **Stage 2 (Header Parsing & Metadata Extraction):** Implements lookahead byte parsing via `file.peek()` to robustly skip multi-line image comments (`#`).
  * Extracts structural metadata (`width`, `height`, `maxColorValue`) into a modular `ImageMetaData` structure passed by reference.
* **Stage 3 (Image resizing to 256 *256 ):*** resize images using Nearest Neighbor and Bilinear Interpolation.

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
```
---

### 📊 Image Resizing Analysis: No-Reference Quality Metrics

To evaluate and compare our custom **Nearest Neighbor Interpolation (NNI)** and **Bilinear Interpolation (BI)** algorithms, we utilized three industry-standard MATLAB **No-Reference (Blind) Image Quality Assessment (IQA)** metrics. Unlike metrics like PSNR or MSE, these evaluate perceptual quality *without* requiring a pristine target reference.

### 📐 Understanding the Metrics

* **BRISQUE** (Blind/Referenceless Image Spatial Quality Evaluator): Analyzes local luminance normalization to detect unnatural distortions or pixel artifacts.
* **NIQE** (Natural Image Quality Evaluator): Measures deviations from statistical regularities observed in natural, undistorted images.
* **PIQE** (Perception-based Image Quality Evaluator): Evaluates local block-based distortions (like blockiness or jaggedness) to mimic human visual perception.

> 💡 **Key Rule for Interpretation:** For all three metrics (BRISQUE, NIQE, and PIQE), **a LOWER score indicates BETTER perceptual quality / fewer artifacts.**

---

### 📈 Experimental Results Summary

| Metric | Original Image (Baseline) | Nearest Neighbor (NNI) | Bilinear Interpolation (BI) |
| --- | --- | --- | --- |
| **BRISQUE** | **25.64** | 30.68 | **27.72** |
| **NIQE** | **4.12** | 8.81 | **7.65** |
| **PIQE** | 33.95 | 36.55 | **30.93** |

---

### 🔍 Key Takeaways

1. **Bilinear Interpolation Universally Outperforms Nearest Neighbor:**
Across BRISQUE, NIQE, and PIQE, the Bilinear algorithm achieved lower scores than NNI. This provides mathematical proof that the weighted-average blending math successfully mitigates unnatural pixel boundaries.
2. **The "Jaggy" Effect in NNI:**
NNI suffered the worst scores (highest penalty) in **NIQE (8.81)** and **BRISQUE (30.68)**. This is a direct consequence of the spatial aliasing, staircase artifacts, and sharp grid transitions inherent to nearest-neighbor mapping.
3. **Perceptual Smoothing Advantage (PIQE):**
Fascinatingly, the Bilinear output achieved a **PIQE score of 30.93**, which is lower than even the original image (33.95). Because PIQE penalizes high-frequency noise and block edge discontinuities, the natural low-pass filtering (soft smoothing) behavior of bilinear averaging registers as a perceptually "cleaner" structure to the algorithm.
