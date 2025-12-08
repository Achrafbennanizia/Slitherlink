#include <gtest/gtest.h>
#include <vector>

// Basic State structure for testing
struct State
{
    std::vector<char> edgeState;
    std::vector<int> pointDegree;
    std::vector<int> cellEdgeCount;
    std::vector<int> cellUndecided;
    std::vector<int> pointUndecided;
};

class StateTest : public ::testing::Test
{
protected:
    State state;
};

TEST_F(StateTest, DefaultConstruction)
{
    EXPECT_TRUE(state.edgeState.empty());
    EXPECT_TRUE(state.pointDegree.empty());
    EXPECT_TRUE(state.cellEdgeCount.empty());
}

TEST_F(StateTest, EdgeStateInitialization)
{
    int numEdges = 40;
    state.edgeState.assign(numEdges, 0);

    EXPECT_EQ(state.edgeState.size(), 40);

    for (char e : state.edgeState)
    {
        EXPECT_EQ(e, 0); // All undecided
    }
}

TEST_F(StateTest, EdgeStateModification)
{
    state.edgeState = {0, 0, 0, 0, 0};

    state.edgeState[0] = 1;  // ON
    state.edgeState[2] = -1; // OFF

    EXPECT_EQ(state.edgeState[0], 1);
    EXPECT_EQ(state.edgeState[1], 0);
    EXPECT_EQ(state.edgeState[2], -1);
}

TEST_F(StateTest, PointDegreeTracking)
{
    state.pointDegree.assign(25, 0);

    // Simulate adding edges
    state.pointDegree[0]++;
    state.pointDegree[0]++;
    state.pointDegree[5]++;

    EXPECT_EQ(state.pointDegree[0], 2);
    EXPECT_EQ(state.pointDegree[5], 1);
    EXPECT_EQ(state.pointDegree[10], 0);
}

TEST_F(StateTest, CopySemantics)
{
    state.edgeState = {1, -1, 0, 1};
    state.pointDegree = {2, 1, 0};

    State copy = state;

    EXPECT_EQ(copy.edgeState.size(), 4);
    EXPECT_EQ(copy.pointDegree.size(), 3);
    EXPECT_EQ(copy.edgeState[0], 1);
    EXPECT_EQ(copy.edgeState[1], -1);
}

TEST_F(StateTest, MoveSemantics)
{
    state.edgeState = {1, -1, 0, 1, 0};
    state.pointDegree = {2, 1, 0, 1};

    State moved = std::move(state);

    EXPECT_EQ(moved.edgeState.size(), 5);
    EXPECT_EQ(moved.pointDegree.size(), 4);
    // Original state should be moved-from (empty or valid but unspecified)
}
