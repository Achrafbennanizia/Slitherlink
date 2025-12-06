#ifndef SLITHERLINK_GRID_H
#define SLITHERLINK_GRID_H

#include <vector>
#include <string>

namespace slitherlink
{

    /**
     * @brief Represents a Slitherlink puzzle grid
     *
     * Single Responsibility: Grid data management
     */
    class Grid
    {
    public:
        Grid() = default;
        Grid(int rows, int cols);

        int getRows() const { return n; }
        int getCols() const { return m; }
        int getClue(int row, int col) const;
        void setClue(int row, int col, int value);

        int cellIndex(int r, int c) const { return r * m + c; }
        const std::vector<int> &getClues() const { return clues; }

        bool loadFromFile(const std::string &filename);

    private:
        int n = 0;              ///< Number of rows
        int m = 0;              ///< Number of columns
        std::vector<int> clues; ///< Clue values (-1 for no clue, 0-3 for clue value)
    };

} // namespace slitherlink

#endif // SLITHERLINK_GRID_H
