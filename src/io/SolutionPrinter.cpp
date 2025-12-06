#include "slitherlink/io/SolutionPrinter.h"
#include <iostream>
namespace slitherlink {

void SolutionPrinter::print(const Solution &sol, std::ostream &out) const
{
    int n = grid.n, m = grid.m;

    auto isHorizOn = [&](int r, int c) -> bool
    {
        int idx = horizEdgeIndex[r * m + c];
        return sol.edgeState[idx] == 1;
    };
    auto isVertOn = [&](int r, int c) -> bool
    {
        int idx = vertEdgeIndex[r * (m + 1) + c];
        return sol.edgeState[idx] == 1;
    };

    for (int r = 0; r <= n; ++r)
    {
        std::string line;
        for (int c = 0; c < m; ++c)
        {
            line += "+";
            line += (isHorizOn(r, c) ? "-" : " ");
        }
        line += "+";
        out << line << "\n";

        if (r == n)
            break;

        std::string vline;
        for (int c = 0; c < m; ++c)
        {
            vline += (isVertOn(r, c) ? "|" : " ");
            int clue = grid.clues[grid.cellIndex(r, c)];
            char ch = ' ';
            if (clue >= 0)
                ch = char('0' + clue);
            vline += ch;
        }
        vline += (isVertOn(r, m) ? "|" : " ");
        out << vline << "\n";
    }

    out << "Cycle (point coordinates row,col):\n";
    for (size_t i = 0; i < sol.cyclePoints.size(); ++i)
    {
        auto [r, c] = sol.cyclePoints[i];
        out << "(" << r << "," << c << ")";
        if (i + 1 < sol.cyclePoints.size())
            out << " -> ";
    }
    out << "\n";
}

void SolutionPrinter::printSummary(int count, std::ostream &out) const
{
    out << "\n=== SUMMARY ===\n";
    out << "Total solutions found: " << count << "\n";
}

} // namespace slitherlink
