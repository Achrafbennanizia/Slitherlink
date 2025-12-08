#include "io/GridReader.h"
#include <fstream>
#include <stdexcept>
#include <vector>
namespace slitherlink
{

    Grid readGridFromFile(const std::string &filename)
    {
        std::ifstream in(filename);
        if (!in)
        {
            throw std::runtime_error("Could not open file " + filename);
        }
        Grid g;
        in >> g.n >> g.m;
        std::string line;
        getline(in, line);

        g.clues.assign(g.n * g.m, -1);

        for (int r = 0; r < g.n; ++r)
        {
            if (!getline(in, line))
            {
                throw std::runtime_error("Not enough grid lines in file");
            }
            if (line.empty())
            {
                r--;
                continue;
            }
            std::vector<int> row;
            for (char ch : line)
            {
                if (ch == ' ' || ch == '\t')
                    continue;
                if (ch >= '0' && ch <= '3')
                {
                    row.push_back(ch - '0');
                }
                else
                {
                    row.push_back(-1);
                }
                if ((int)row.size() == g.m)
                    break;
            }
            if ((int)row.size() != g.m)
            {
                throw std::runtime_error("Row " + std::to_string(r) + " does not have m entries");
            }
            for (int c = 0; c < g.m; ++c)
            {
                g.clues[g.cellIndex(r, c)] = row[c];
            }
        }
        return g;
    }

} // namespace slitherlink
