#include <gtest/gtest.h>
#include <sstream>
#include <fstream>

// Simple Grid structure for testing (copied from main.cpp for now)
struct Grid
{
    int n = 0, m = 0;
    std::vector<int> clues;
    int cellIndex(int r, int c) const { return r * m + c; }
};

// Test fixture for Grid tests
class GridTest : public ::testing::Test
{
protected:
    Grid grid;
};

TEST_F(GridTest, DefaultConstruction)
{
    EXPECT_EQ(grid.n, 0);
    EXPECT_EQ(grid.m, 0);
    EXPECT_TRUE(grid.clues.empty());
}

TEST_F(GridTest, CellIndexCalculation)
{
    grid.n = 5;
    grid.m = 5;

    EXPECT_EQ(grid.cellIndex(0, 0), 0);
    EXPECT_EQ(grid.cellIndex(0, 4), 4);
    EXPECT_EQ(grid.cellIndex(1, 0), 5);
    EXPECT_EQ(grid.cellIndex(2, 3), 13);
    EXPECT_EQ(grid.cellIndex(4, 4), 24);
}

TEST_F(GridTest, CluesStorage)
{
    grid.n = 4;
    grid.m = 4;
    grid.clues.assign(grid.n * grid.m, -1);

    EXPECT_EQ(grid.clues.size(), 16);

    // Set some clues
    grid.clues[grid.cellIndex(0, 0)] = 2;
    grid.clues[grid.cellIndex(1, 1)] = 3;

    EXPECT_EQ(grid.clues[0], 2);
    EXPECT_EQ(grid.clues[5], 3);
    EXPECT_EQ(grid.clues[15], -1);
}

// Main function to run all tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
