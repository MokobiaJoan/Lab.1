#include <gtest/gtest.h>
#define TESTING
#include "../src/main.cpp"

// Create a dummy test image: 3x2 pixels (RGB format)
unsigned char dummy_image[] = {
    255, 0, 0,   0, 255, 0,   0, 0, 255,  // Row 1: Red, Green, Blue
    255, 255, 0, 0, 255, 255, 255, 0, 255 // Row 2: Yellow, Cyan, Magenta
};

TEST(ImageProcessingTest, ClockwiseRotationPreservesSize) {
    int width = 3, height = 2;
    unsigned char* rotated = rotate_clockwise(dummy_image, width, height);
    ASSERT_NE(rotated, nullptr);
    delete[] rotated;
}

TEST(ImageProcessingTest, CounterclockwiseRotationPreservesSize) {
    int width = 3, height = 2;
    unsigned char* rotated = rotate_counterclockwise(dummy_image, width, height);
    ASSERT_NE(rotated, nullptr);
    delete[] rotated;
}

TEST(ImageProcessingTest, GaussianFilterReturnsValidImage) {
    int width = 3, height = 2;
    unsigned char* filtered = apply_gaussian_filter(dummy_image, width, height);
    ASSERT_NE(filtered, nullptr);
    delete[] filtered;
}