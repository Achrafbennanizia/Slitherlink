// C++ benchmark program using chrono for precise timing
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <map>
#include <cmath>

struct BenchmarkResult
{
    std::string puzzleName;
    int threads;
    std::vector<double> times;

    double average() const
    {
        double sum = 0;
        for (double t : times)
            sum += t;
        return sum / times.size();
    }

    double stddev() const
    {
        double avg = average();
        double sum_sq = 0;
        for (double t : times)
            sum_sq += (t - avg) * (t - avg);
        return std::sqrt(sum_sq / times.size());
    }

    double min() const
    {
        double m = times[0];
        for (double t : times)
            if (t < m)
                m = t;
        return m;
    }

    double max() const
    {
        double m = times[0];
        for (double t : times)
            if (t > m)
                m = t;
        return m;
    }
};

class Benchmarker
{
private:
    std::string solverPath;
    std::vector<BenchmarkResult> results;

public:
    Benchmarker(const std::string &solver) : solverPath(solver) {}

    void run(const std::string &puzzle, int threads, int iterations = 5)
    {
        BenchmarkResult result;
        result.puzzleName = puzzle;
        result.threads = threads;

        std::cout << "Benchmarking " << puzzle << " with " << threads << " threads (" << iterations << " runs)...\n";

        for (int i = 0; i < iterations; ++i)
        {
            std::string cmd = solverPath + " " + puzzle + " --threads " + std::to_string(threads) + " 2>&1";

            auto start = std::chrono::high_resolution_clock::now();
            int ret = system(cmd.c_str());
            auto end = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration<double>(end - start).count();
            result.times.push_back(duration);

            std::cout << "  Run " << (i + 1) << ": " << std::fixed << std::setprecision(6) << duration << "s\n";
        }

        results.push_back(result);
    }

    void printSummary() const
    {
        std::cout << "\n=== Benchmark Summary ===\n";
        std::cout << std::setw(30) << "Puzzle"
                  << std::setw(10) << "Threads"
                  << std::setw(12) << "Avg (s)"
                  << std::setw(12) << "StdDev"
                  << std::setw(12) << "Min"
                  << std::setw(12) << "Max"
                  << "\n";
        std::cout << std::string(88, '-') << "\n";

        for (const auto &r : results)
        {
            std::cout << std::setw(30) << r.puzzleName
                      << std::setw(10) << r.threads
                      << std::setw(12) << std::fixed << std::setprecision(6) << r.average()
                      << std::setw(12) << r.stddev()
                      << std::setw(12) << r.min()
                      << std::setw(12) << r.max()
                      << "\n";
        }
    }

    void saveCSV(const std::string &filename) const
    {
        std::ofstream out(filename);
        out << "Puzzle,Threads,Average,StdDev,Min,Max\n";

        for (const auto &r : results)
        {
            out << r.puzzleName << ","
                << r.threads << ","
                << r.average() << ","
                << r.stddev() << ","
                << r.min() << ","
                << r.max() << "\n";
        }

        std::cout << "\nResults saved to: " << filename << "\n";
    }
};

int main(int argc, char **argv)
{
    std::string solver = argc >= 2 ? argv[1] : "./cmake-build-debug/slitherlink";

    std::cout << "=== Slitherlink Performance Benchmark ===\n";
    std::cout << "Solver: " << solver << "\n\n";

    Benchmarker bench(solver);

    // Benchmark different puzzles
    std::vector<std::string> puzzles = {
        "puzzles/examples/example4x4_easy.txt",
        "puzzles/examples/example5x5_medium.txt",
        "puzzles/examples/example6x6_medium.txt"};

    // Test thread scaling
    std::vector<int> threadCounts = {1, 2, 4, 8};

    for (const auto &puzzle : puzzles)
    {
        for (int threads : threadCounts)
        {
            bench.run(puzzle, threads, 3);
        }
    }

    bench.printSummary();
    bench.saveCSV("benchmark_results.csv");

    return 0;
}
