#include <gtest/gtest.h>

// Test simplu de verificare a integrării Google Test
TEST(SanityCheck, DetectableTest) {
    EXPECT_EQ(1, 1);
}

// Aici poți adăuga și alte teste unitare dacă ai, de ex:
// TEST(NetworkGeneratorTest, GenerateSmallNetwork) { ... }
