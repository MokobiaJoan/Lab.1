/*Mokobia Joan Chidiebere, st135508@student.spbu.ru, 24.B83-mm*/

/**
 * @file main.cpp
 * @brief Main program for BMP image processing.
 *
 * This program performs the following operations on a BMP image:
 * - Loads a grayscale BMP image from disk.
 * - Rotates the image clockwise and counterclockwise.
 * - Applies a Gaussian filter (parallelized with OpenMP).
 * - Measures performance of each operation.
 * - Saves the resulting images to disk.
 *
 * @author Mokobia Joan Chidiebere
 * @date 2025-06-03
 * @version 1.0
 */
#include "main.h"
#include <chrono>
using namespace std::chrono;

// Function to load BMP image and convert to grayscale
unsigned char* load_bmp(const char* file_path, int& width, int& height) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return nullptr;
    }

    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER dibHeader;

    file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
    file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

    if (bmpHeader.bfType != 0x4D42) {
        std::cerr << "Not a valid BMP file." << std::endl;
        return nullptr;
    }

    width = dibHeader.biWidth;
    height = std::abs(dibHeader.biHeight);  

    // Move file pointer to pixel data
    file.seekg(bmpHeader.bfOffBits, std::ios::beg);

    // Calculate the row size including padding
    int row_padded = (width * 3 + 3) & (~3); 
    std::vector<unsigned char> image_data(row_padded * height);

    file.read(reinterpret_cast<char*>(image_data.data()), image_data.size());

    unsigned char* grayscale_image = new unsigned char[width * height];

    // Convert to grayscale (assuming BMP is in 24-bit color)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pixel_index = y * row_padded + x * 3;
            unsigned char r = image_data[pixel_index + 2];
            unsigned char g = image_data[pixel_index + 1];
            unsigned char b = image_data[pixel_index];
            unsigned char gray = static_cast<unsigned char>(0.3 * r + 0.59 * g + 0.11 * b);
            grayscale_image[y * width + x] = gray;
        }
    }

    return grayscale_image;
}

// Function to save BMP image
void save_bmp(const char* file_path, unsigned char* image_data, int width, int height) {
    uint32_t biSizeImage = (width + 3) & (~3); 
    biSizeImage *= height;  // Total image size

    BITMAPINFOHEADER dibHeader = { 40, width, height, 1, 8, 0, biSizeImage, 0, 0, 256, 0 };

    BITMAPFILEHEADER bmpHeader = { 0x4D42, 0, 0, 0, 54 + 256 * 4 };
    bmpHeader.bfSize = bmpHeader.bfOffBits + dibHeader.biSizeImage;

    std::ofstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Error saving image: " << file_path << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(&bmpHeader), sizeof(bmpHeader));
    file.write(reinterpret_cast<const char*>(&dibHeader), sizeof(dibHeader));

    // Write grayscale color palette (0-255, one color per index)
    for (int i = 0; i < 256; ++i) {
        file.put(i); file.put(i); file.put(i); file.put(0);
    }

    // Row padding
    int row_padded = (width + 3) & (~3);
    std::vector<unsigned char> padded_row(row_padded);

    // Write pixel data (indexing into the grayscale palette)
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            padded_row[x] = image_data[y * width + x];
        }
        file.write(reinterpret_cast<const char*>(padded_row.data()), row_padded);
    }
}


// Function to calculate memory usage
size_t calculate_memory_usage(int width, int height) {
    // Memory allocated = width * height * size of one pixel (1 byte for grayscale images)
    return static_cast<size_t>(width * height);
}


// Rotate image 90 degrees clockwise
unsigned char* rotate_clockwise(unsigned char* image_data, int width, int height) {
    unsigned char* rotated_image = new unsigned char[width * height];

    #pragma omp parallel for collapse(2)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            rotated_image[x * height + (height - y - 1)] = image_data[y * width + x];
        }
    }

    return rotated_image;
}

// Rotate image 90 degrees counterclockwise
unsigned char* rotate_counterclockwise(unsigned char* image_data, int width, int height) {
    unsigned char* rotated_image = new unsigned char[width * height];

    #pragma omp parallel for collapse(2)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            rotated_image[(width - x - 1) * height + y] = image_data[y * width + x];
        }
    }

    return rotated_image;
}



// Apply a Gaussian filter with a larger kernel and better edge handling

/**
 * @brief Applies a 5x5 Gaussian filter to the image data.
 *
 * @param image_data Pointer to the raw grayscale image data.
 * @param width Width of the image.
 * @param height Height of the image.
 * @return A new dynamically allocated buffer containing the filtered image.
 */
unsigned char* apply_gaussian_filter(unsigned char* image_data, int width, int height);

unsigned char* apply_gaussian_filter(unsigned char* image_data, int width, int height) {
    unsigned char* filtered_image = new unsigned char[width * height];

    // Define a larger 5x5 Gaussian kernel (normalized)
    float kernel[5][5] = {
        {1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f, 1 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f, 7 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f, 1 / 273.0f}
    };

    // Apply the kernel to each pixel (skip edges for simplicity)
    #pragma omp parallel for collapse(2)
    for (int y = 2; y < height - 2; ++y) {
        for (int x = 2; x < width - 2; ++x) {
            float sum = 0.0f;

            // Convolve the 5x5 region
            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    int pixel_x = x + kx;
                    int pixel_y = y + ky;
                    unsigned char pixel_value = image_data[pixel_y * width + pixel_x];
                    sum += pixel_value * kernel[ky + 2][kx + 2];
                }
            }

            // Set the filtered value
            filtered_image[y * width + x] = static_cast<unsigned char>(sum);
        }
    }


    // Handle edges by copying original pixel values
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y < 2 || x < 2 || y >= height - 2 || x >= width - 2) {
                filtered_image[y * width + x] = image_data[y * width + x];
            }
        }
    }

    return filtered_image;
}

#ifndef TESTING
int main() {
    int width, height;
    const char* input_file = "images/input_image.bmp";
    const char* output_file_clockwise = "output_clockwise.bmp";
    const char* output_file_counterclockwise = "output_counterclockwise.bmp";
    const char* output_file_filtered = "output_filtered.bmp";

    // Timing image loading
    auto start_load = std::chrono::high_resolution_clock::now();
    unsigned char* image_data = load_bmp(input_file, width, height);
    auto end_load = std::chrono::high_resolution_clock::now();
    if (!image_data) return -1;
    std::chrono::duration<double> load_duration = end_load - start_load;
    std::cout << "Time to load image: " << load_duration.count() << " seconds\n";

    // Calculate memory usage
    size_t memory_allocated = calculate_memory_usage(width, height);
    std::cout << "Memory allocated for loading the image: "
              << memory_allocated << " bytes ("
              << (memory_allocated / 1024.0) << " KB)\n";

    // Timing clockwise rotation
    auto start_rotate_cw = std::chrono::high_resolution_clock::now();
    unsigned char* rotated_clockwise = rotate_clockwise(image_data, width, height);
    auto end_rotate_cw = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> rotate_cw_duration = end_rotate_cw - start_rotate_cw;
    std::cout << "Time to rotate clockwise: " << rotate_cw_duration.count() << " seconds\n";

    save_bmp(output_file_clockwise, rotated_clockwise, height, width);

    // Timing counterclockwise rotation
    auto start_rotate_ccw = std::chrono::high_resolution_clock::now();
    unsigned char* rotated_counterclockwise = rotate_counterclockwise(image_data, width, height);
    auto end_rotate_ccw = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> rotate_ccw_duration = end_rotate_ccw - start_rotate_ccw;
    std::cout << "Time to rotate counterclockwise: " << rotate_ccw_duration.count() << " seconds\n";

    save_bmp(output_file_counterclockwise, rotated_counterclockwise, height, width);

    // Timing Sequential Gaussian filter
    auto start_filter_seq = std::chrono::high_resolution_clock::now();
    unsigned char* filtered_image_seq = apply_gaussian_filter(rotated_clockwise, height, width);
    auto end_filter_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> filter_duration_seq = end_filter_seq - start_filter_seq;
    std::cout << "Time to apply Sequential Gaussian filter: " << filter_duration_seq.count() << " seconds\n";

    save_bmp("output_filtered_sequential.bmp", filtered_image_seq, height, width);

    // Timing Gaussian filter
    auto start_filter = std::chrono::high_resolution_clock::now();
    unsigned char* filtered_image = apply_gaussian_filter(rotated_clockwise, height, width);
    auto end_filter = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> filter_duration = end_filter - start_filter;
    std::cout << "Time to apply Gaussian filter: " << filter_duration.count() << " seconds\n";

    save_bmp(output_file_filtered, filtered_image, height, width);

    // Debugging pixel values
    std::cout << "Debugging pixel values (before and after Gaussian filter):\n";
    std::cout << "Original: " << static_cast<int>(rotated_clockwise[100 * height + 100])
              << ", Filtered: " << static_cast<int>(filtered_image[100 * height + 100]) << std::endl;

    // Free memory
    delete[] image_data;
    delete[] rotated_clockwise;
    delete[] rotated_counterclockwise;
    delete[] filtered_image;
    delete[] filtered_image_seq;

    std::cout << "Processing complete.\n";
    return 0;
}
#endif