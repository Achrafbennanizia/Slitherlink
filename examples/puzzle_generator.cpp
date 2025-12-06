// Example: Generate a random Slitherlink puzzle
#include <iostream>
#include <fstream>
#include <random>
#include <string>

void generatePuzzle(int n, int m, double cluePercentage, const std::string &filename)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> clueDist(0, 3);
    std::uniform_real_distribution<> probDist(0.0, 1.0);

    std::ofstream out(filename);
    if (!out)
        throw std::runtime_error("Could not create file: " + filename);

    // Write dimensions
    out << n << " " << m << "\n";

    // Generate clues
    for (int r = 0; r < n; ++r)
    {
        for (int c = 0; c < m; ++c)
        {
            if (probDist(gen) < cluePercentage)
            {
                out << clueDist(gen);
            }
            else
            {
                out << ".";
            }

            if (c + 1 < m)
                out << " ";
        }
        out << "\n";
    }

    std::cout << "Generated " << n << "x" << m << " puzzle: " << filename << "\n";
    std::cout << "Clue density: " << (cluePercentage * 100) << "%\n";
}

int main(int argc, char **argv)
{
    std::cout << "=== Puzzle Generator Example ===\n\n";

    if (argc >= 4)
    {
        int n = std::atoi(argv[1]);
        int m = std::atoi(argv[2]);
        std::string output = argv[3];
        double density = argc >= 5 ? std::atof(argv[4]) : 0.6;

        if (n < 2 || m < 2)
        {
            std::cerr << "Error: Grid size must be at least 2x2\n";
            return 1;
        }

        generatePuzzle(n, m, density, output);
        std::cout << "\nSolve it with: ./slitherlink " << output << "\n";

        return 0;
    }

    // Default: generate some example puzzles
    std::cout << "Generating example puzzles...\n\n";

    generatePuzzle(4, 4, 0.5, "generated_4x4_easy.txt");
    generatePuzzle(5, 5, 0.6, "generated_5x5_medium.txt");
    generatePuzzle(6, 6, 0.7, "generated_6x6_hard.txt");

    std::cout << "\nUsage: " << argv[0] << " <rows> <cols> <output-file> [clue-density]\n";
    std::cout << "Example: " << argv[0] << " 8 8 my_puzzle.txt 0.65\n";

    return 0;
}
