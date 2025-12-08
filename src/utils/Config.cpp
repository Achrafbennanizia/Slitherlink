#include "slitherlink/Config.h"
#include <stdexcept>
#include <algorithm>

namespace slitherlink
{

    void SolverConfig::validate()
    {
        if (cpuUsagePercent < 0.0 || cpuUsagePercent > 100.0)
        {
            throw std::invalid_argument("CPU usage must be between 0 and 100");
        }

        if (numThreads < 0)
        {
            throw std::invalid_argument("Number of threads cannot be negative");
        }

        if (maxSolutions < 1 && maxSolutions != -1)
        {
            throw std::invalid_argument("Max solutions must be positive or -1 for unlimited");
        }

        if (timeoutSeconds < 0.0)
        {
            throw std::invalid_argument("Timeout cannot be negative");
        }

        // Auto-correct stopAfterFirst
        if (maxSolutions == 1)
        {
            stopAfterFirst = true;
        }
    }

    SolverConfig SolverConfig::fromCommandLine(int argc, char *argv[])
    {
        SolverConfig config;

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--all" || arg == "-a")
            {
                config.stopAfterFirst = false;
                config.maxSolutions = -1;
            }
            else if (arg == "--max-solutions" && i + 1 < argc)
            {
                config.maxSolutions = std::stoi(argv[++i]);
            }
            else if (arg == "--timeout" && i + 1 < argc)
            {
                config.timeoutSeconds = std::stod(argv[++i]);
            }
            else if (arg == "--threads" && i + 1 < argc)
            {
                config.numThreads = std::stoi(argv[++i]);
            }
            else if (arg == "--cpu" && i + 1 < argc)
            {
                config.cpuUsagePercent = std::stod(argv[++i]);
            }
            else if (arg == "--verbose" || arg == "-v")
            {
                config.verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q")
            {
                config.printSolutions = false;
                config.printStatistics = false;
            }
            else if (arg == "--no-parallel")
            {
                config.enableParallelization = false;
            }
        }

        config.validate();
        return config;
    }

} // namespace slitherlink
