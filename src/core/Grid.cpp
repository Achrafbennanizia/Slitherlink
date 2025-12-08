#include "Grid.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace slitherlink
{

    Grid::Grid(int rows, int cols) : n(rows), m(cols)
    {
        clues.resize(n * m, -1);
    }

    int Grid::getClue(int row, int col) const
    {
        if (row < 0 || row >= n || col < 0 || col >= m)
            return -1;
        return clues[cellIndex(row, col)];
    }

    void Grid::setClue(int row, int col, int value)
    {
        if (row >= 0 && row < n && col >= 0 && col < m)
            clues[cellIndex(row, col)] = value;
    }

    bool Grid::loadFromFile(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            return false;

        if (!(file >> n >> m))
            return false;

        clues.assign(n * m, -1);

        std::string line;
        std::getline(file, line); // consume newline

        for (int r = 0; r < n; ++r)
        {
            if (!std::getline(file, line))
                return false;

            std::istringstream iss(line);
            for (int c = 0; c < m; ++c)
            {
                char ch;
                if (!(iss >> ch))
                    return false;

                if (ch >= '0' && ch <= '3')
                    clues[cellIndex(r, c)] = ch - '0';
                else if (ch == '.' || ch == '-' || ch == 'x' || ch == 'X')
                    clues[cellIndex(r, c)] = -1;
                else
                    return false;
            }
        }

        return true;
    }

} // namespace slitherlink
