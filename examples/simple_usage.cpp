// Simple example: Solve a single puzzle and print the solution
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

// Simple Grid structure
struct Grid
{
    int n = 0, m = 0;
    std::vector<int> clues;
    int cellIndex(int r, int c) const { return r * m + c; }
};

// Read grid from file
Grid readGrid(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        throw std::runtime_error("Could not open file: " + filename);

    Grid g;
    in >> g.n >> g.m;
    std::string line;
    std::getline(in, line); // consume newline

    g.clues.assign(g.n * g.m, -1);

    for (int r = 0; r < g.n; ++r)
    {
        if (!std::getline(in, line))
            throw std::runtime_error("Not enough grid lines");

        std::vector<int> row;
        for (char ch : line)
        {
            if (ch == ' ' || ch == '\t')
                continue;
            if (ch >= '0' && ch <= '3')
                row.push_back(ch - '0');
            else
                row.push_back(-1);
            if ((int)row.size() == g.m)
                break;
        }

        for (int c = 0; c < g.m; ++c)
            g.clues[g.cellIndex(r, c)] = row[c];
    }

    return g;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <puzzle-file>\n";
        std::cerr << "Example: " << argv[0] << " ../puzzles/examples/example4x4.txt\n";
        return 1;
    }

    try
    {
        std::cout << "=== Simple Slitherlink Solver Example ===\n\n";

        // Read the puzzle
        std::cout << "Reading puzzle from: " << argv[1] << "\n";
        Grid grid = readGrid(argv[1]);
        std::cout << "Puzzle size: " << grid.n << "x" << grid.m << "\n";

        // Count clues
        int clueCount = 0;
        for (int c : grid.clues)
            if (c >= 0)
                clueCount++;
        std::cout << "Number of clues: " << clueCount << " out of "
                  << (grid.n * grid.m) << " cells\n\n";

        // Display the puzzle
        std::cout << "Puzzle:\n";
        for (int r = 0; r < grid.n; ++r)
        {
            for (int c = 0; c < grid.m; ++c)
            {
                int clue = grid.clues[grid.cellIndex(r, c)];
                if (clue >= 0)
                    std::cout << clue << " ";
                else
                    std::cout << ". ";
            }
            std::cout << "\n";
        }

        std::cout << "\nTo solve this puzzle, use the main solver:\n";
        std::cout << "  ./slitherlink " << argv[1] << "\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
