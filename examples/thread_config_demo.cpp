// Example showing thread control and performance comparison
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <puzzle-file>\n";
        return 1;
    }

    std::string puzzleFile = argv[1];

    std::cout << "=== Thread Configuration Example ===\n\n";
    std::cout << "This example demonstrates different thread configurations:\n\n";

    // Example configurations
    std::vector<std::string> configs = {
        "--threads 1",
        "--threads 4",
        "--threads 8",
        "--cpu 0.25",
        "--cpu 0.5",
        "--cpu 1.0"};

    std::cout << "Test these configurations with your puzzle:\n\n";

    for (const auto &config : configs)
    {
        std::cout << "./slitherlink " << puzzleFile << " " << config << "\n";
    }

    std::cout << "\n=== Performance Tips ===\n";
    std::cout << "1. For small puzzles (4x4): --threads 4 is often optimal\n";
    std::cout << "2. For medium puzzles (5x5-7x7): --threads 8 works well\n";
    std::cout << "3. For large puzzles (10x10+): Use all cores (no flag)\n";
    std::cout << "4. To save battery: --cpu 0.5 (uses 50% CPU)\n";
    std::cout << "5. Background solving: --cpu 0.25 (uses 25% CPU)\n\n";

    std::cout << "Example benchmark script:\n";
    std::cout << "for threads in 1 2 4 8; do\n";
    std::cout << "  echo \"Testing with $threads threads:\"\n";
    std::cout << "  ./slitherlink puzzle.txt --threads $threads\n";
    std::cout << "done\n";

    return 0;
}
