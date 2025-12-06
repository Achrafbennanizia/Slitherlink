#include "slitherlink/factory/SolverFactory.h"
#include "slitherlink/io/SolutionCollector.h"
#include "slitherlink/io/SolutionPrinter.h"
#include "slitherlink/solver/GraphBuilder.h"
namespace slitherlink {

std::unique_ptr<SlitherlinkSolver> SolverFactory::createSolver(const Grid &grid, bool findAll)
{
    // SOLID: Dependency Inversion - inject dependencies via interfaces
    auto solutionCollector = std::make_shared<SolutionCollector>(findAll);
    
    // Build graph to get indices for printer
    auto graphBuilder = std::make_shared<GraphBuilder>();
    graphBuilder->buildGraph(grid);
    
    auto solutionPrinter = std::make_shared<SolutionPrinter>(
        grid,
        graphBuilder->getHorizEdgeIndex(),
        graphBuilder->getVertEdgeIndex()
    );

    return std::make_unique<SlitherlinkSolver>(
        grid,
        solutionCollector,
        solutionPrinter
    );
}

} // namespace slitherlink
