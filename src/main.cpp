#include "slitherlink/io/GridReader.h"
#include "slitherlink/factory/SolverFactory.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>

using namespace std;
using namespace slitherlink;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <inputfile> [--all]\n";
        return 1;
    }
    string filename = argv[1];
    bool allSolutions = false;
    if (argc >= 3)
    {
        string arg2 = argv[2];
        if (arg2 == "--all")
            allSolutions = true;
    }

    try
    {
        Grid grid = readGridFromFile(filename);

        // SOLID: Dependency Inversion - depend on abstractions via factory
        auto solver = SolverFactory::createSolver(grid, allSolutions);

        auto start = chrono::steady_clock::now();
        solver->solve();
        auto end = chrono::steady_clock::now();
        double seconds = chrono::duration_cast<chrono::duration<double>>(end - start).count();

        solver->printResults(cout);
        cout << "Time: " << seconds << " s\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
