
# Performance Evaluation Report

## Assignment: Image Processing with Parallelization

**Student**: Mokobia Joan Chidiebere  
**Email**: st135508@student.spbu.ru  
**Group**: 24.B83-mm

---

## Summary

This report evaluates the performance of a grayscale image processing program that performs the following operations:

1. Load a BMP image.
2. Rotate the image clockwise and counterclockwise.
3. Apply a Gaussian filter using both sequential and parallel methods.
4. Measure performance of each operation.
5. Saves the resulting images to disk.

---

## System Information

- **Image Size**: 2.4 MB (2457600 bytes)
- **Kernel**: 5x5 Gaussian blur
- **Parallelization Method**: OpenMP

---

## Performance Results

| Operation                          | Time (s)     |
|------------------------------------|--------------|
| Load BMP image                     | 0.068        |
| Rotate clockwise                   | 0.016        |
| Rotate counterclockwise            | 0.017        |
| Apply Gaussian filter (sequential) | 0.131        |
| Apply Gaussian filter (parallel)   | 0.119        |

---

## Analysis

The parallelized Gaussian filter achieved approximately **10% speedup** compared to the sequential version:

**Speedup = 0.131 / 0.119 ≈ 1.10**

This demonstrates a modest performance gain from parallel processing, particularly beneficial for larger images or multi-core systems.

---

## Pixel Verification

A sample pixel comparison after filtering confirmed correctness:

- Original value at pixel `[100][100]`: **188**
- Filtered value: **187**

---

## Conclusion

Parallelizing the Gaussian filter improved performance while maintaining result consistency. This validates OpenMP as an effective tool for enhancing image processing workloads in C++.

