/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Slitherlink Solver", "index.html", [
    [ "Slitherlink Solver", "md__r_e_a_d_m_e.html", [
      [ "Overview", "md__r_e_a_d_m_e.html#autotoc_md1", null ],
      [ "Project Structure (current)", "md__r_e_a_d_m_e.html#autotoc_md2", null ],
      [ "Build &amp; Run", "md__r_e_a_d_m_e.html#autotoc_md3", null ],
      [ "Puzzles Included (current repo)", "md__r_e_a_d_m_e.html#autotoc_md4", null ],
      [ "Testing &amp; Benchmarking", "md__r_e_a_d_m_e.html#autotoc_md5", null ],
      [ "Documentation Map", "md__r_e_a_d_m_e.html#autotoc_md6", null ],
      [ "Notes on Versions and History", "md__r_e_a_d_m_e.html#autotoc_md7", null ]
    ] ],
    [ "Slitherlink Solver - Complete User Guide", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html", [
      [ "Table of Contents", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md9", null ],
      [ "What is Slitherlink?", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md11", [
        [ "Rules:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md12", null ],
        [ "Simple Example:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md13", null ]
      ] ],
      [ "How the Program Works", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md15", [
        [ "1. <b>Reading the Puzzle</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md16", null ],
        [ "2. <b>Building the Graph</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md17", null ],
        [ "3. <b>Constraint Propagation</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md18", null ],
        [ "4. <b>Smart Heuristics</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md19", null ],
        [ "5. <b>Backtracking Search</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md20", null ],
        [ "6. <b>Parallel Processing</b>", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md21", null ]
      ] ],
      [ "Getting Started", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md23", [
        [ "Building the Program (current layout)", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md24", null ]
      ] ],
      [ "Creating Puzzle Files", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md26", [
        [ "Format:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md27", null ],
        [ "Rules:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md28", null ],
        [ "Example 1: Tiny 2×2 Puzzle", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md29", null ],
        [ "Example 2: 5×5 Puzzle (matches provided samples)", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md30", null ],
        [ "Example 3: 7×7 Puzzle (structure only)", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md31", null ]
      ] ],
      [ "Running the Solver", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md33", [
        [ "Basic Usage (current layout):", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md34", null ],
        [ "Command Line Options:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md35", null ]
      ] ],
      [ "Understanding the Output", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md37", [
        [ "Example Output:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md38", null ],
        [ "Output Breakdown:", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md39", null ]
      ] ],
      [ "Examples", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md41", [
        [ "Example 1: Trivial 2×2 Puzzle", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md42", null ],
        [ "Example 2: Classic 4×4 Puzzle", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md44", null ],
        [ "Example 3: Moderate 6×6 Puzzle", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md46", null ],
        [ "Example 4: Large 10×10 Puzzle", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md48", null ]
      ] ],
      [ "Advanced Usage", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md50", [
        [ "Finding All Solutions", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md51", null ],
        [ "Performance Tuning", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md53", null ],
        [ "Creating Your Own Puzzles", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md55", null ],
        [ "Understanding Performance", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md57", null ]
      ] ],
      [ "Troubleshooting", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md59", [
        [ "Problem: \"Could not open file\"", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md60", null ],
        [ "Problem: \"No solution found\"", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md61", null ],
        [ "Problem: Solver runs forever", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md62", null ],
        [ "Problem: Compilation errors", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md63", null ]
      ] ],
      [ "Quick Reference", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md65", [
        [ "File Format Cheat Sheet", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md66", null ],
        [ "Common Commands", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md67", null ],
        [ "Solution Symbols", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md68", null ]
      ] ],
      [ "Need Help?", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md70", null ],
      [ "Summary", "md_docs_2user_2_u_s_e_r___g_u_i_d_e.html#autotoc_md72", null ]
    ] ],
    [ "Project Architecture (Current)", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html", [
      [ "Overview", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md74", null ],
      [ "Directory Structure (current)", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md75", null ],
      [ "Modules (high level)", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md76", null ],
      [ "Tests", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md77", null ],
      [ "Documentation", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md78", null ],
      [ "Notes on accuracy", "md_docs_2developer_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md79", null ]
    ] ],
    [ "Code Structure", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html", [
      [ "Overview", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md81", null ],
      [ "Directory Structure", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md82", null ],
      [ "Module Descriptions", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md83", [
        [ "Core Data Structures (include/)", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md84", [
          [ "Grid.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md85", null ],
          [ "Edge.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md86", null ],
          [ "State.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md87", null ],
          [ "Solution.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md88", null ],
          [ "Solver.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md89", null ],
          [ "GridReader.h", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md90", null ]
        ] ],
        [ "Implementation Files (src/)", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md91", [
          [ "main.cpp (~50 lines)", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md92", null ],
          [ "Solver.cpp (~830 lines)", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md93", null ],
          [ "GridReader.cpp (~60 lines)", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md94", null ]
        ] ]
      ] ],
      [ "Build System", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md95", [
        [ "CMakeLists.txt", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md96", null ]
      ] ],
      [ "Benefits of This Structure", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md97", null ],
      [ "Compilation", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md98", null ],
      [ "File Size Summary", "md_docs_2developer_2_c_o_d_e___s_t_r_u_c_t_u_r_e.html#autotoc_md99", null ]
    ] ],
    [ "SOLID Architecture - Implementation Complete ✅", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html", [
      [ "🎯 Architecture Overview", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md101", [
        [ "Key Principles Applied", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md102", null ]
      ] ],
      [ "📁 File Structure", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md104", null ],
      [ "🔧 Building the SOLID Architecture", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md106", [
        [ "Prerequisites", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md107", null ],
        [ "Build Steps", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md108", null ],
        [ "Build Options", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md109", null ]
      ] ],
      [ "💡 Usage Example", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md111", null ],
      [ "🚀 Performance Optimizations Preserved", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md113", [
        [ "Phase 1 Optimizations (5-20x speedup)", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md114", null ],
        [ "Phase 2 Optimizations (111x additional speedup)", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md115", null ]
      ] ],
      [ "🧪 Testing", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md117", [
        [ "Run with Example Puzzle", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md118", null ],
        [ "Performance Benchmark", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md119", null ],
        [ "Expected Results", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md120", null ]
      ] ],
      [ "🎨 Extending the Architecture", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md122", [
        [ "Add a New Heuristic", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md123", null ],
        [ "Add a New Validator", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md124", null ]
      ] ],
      [ "📊 Benefits of SOLID Architecture", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md126", [
        [ "Maintainability", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md127", null ],
        [ "Testability", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md128", null ],
        [ "Extensibility", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md129", null ],
        [ "Performance", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md130", null ]
      ] ],
      [ "🔄 Migration from Original main.cpp", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md132", null ],
      [ "📝 Implementation Checklist", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md134", [
        [ "✅ Completed", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md135", null ],
        [ "🧪 Ready for Testing", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md136", null ]
      ] ],
      [ "🎯 Next Steps", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md138", null ],
      [ "📚 References", "md_docs_2developer_2_r_e_a_d_m_e___s_o_l_i_d.html#autotoc_md140", null ]
    ] ],
    [ "SOLID Architecture Refactoring - Design Document", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html", [
      [ "SOLID Principles Applied", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md144", [
        [ "1. Single Responsibility Principle (SRP)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md145", null ],
        [ "2. Open/Closed Principle (OCP)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md146", null ],
        [ "3. Liskov Substitution Principle (LSP)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md147", null ],
        [ "4. Interface Segregation Principle (ISP)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md148", null ],
        [ "5. Dependency Inversion Principle (DIP)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md149", null ]
      ] ],
      [ "Architecture Overview", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md151", null ],
      [ "Component Responsibilities", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md153", [
        [ "Core Data Structures", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md154", [
          [ "Grid", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md155", null ],
          [ "State", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md156", null ],
          [ "Solution", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md157", null ]
        ] ],
        [ "Strategy Interfaces", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md158", [
          [ "IHeuristic (Strategy Pattern)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md159", null ],
          [ "IValidator (Strategy Pattern)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md160", null ],
          [ "IPropagator (Strategy Pattern)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md161", null ]
        ] ]
      ] ],
      [ "Planned Refactoring Steps", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md163", [
        [ "✅ Phase 1: Foundation (COMPLETED)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md164", null ],
        [ "🚧 Phase 2: Core Implementations (IN PROGRESS)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md165", null ],
        [ "⏳ Phase 3: Solver Refactoring (TODO)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md166", null ],
        [ "⏳ Phase 4: Testing &amp; Integration (TODO)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md167", null ]
      ] ],
      [ "Example Usage (After Refactoring)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md169", null ],
      [ "Benefits of SOLID Architecture", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md171", [
        [ "Maintainability", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md172", null ],
        [ "Testability", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md173", null ],
        [ "Extensibility", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md174", null ],
        [ "Performance", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md175", null ],
        [ "Code Reuse", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md176", null ]
      ] ],
      [ "Performance Preservation", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md178", null ],
      [ "Migration Strategy", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md180", [
        [ "Option 1: Gradual Migration (Recommended)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md181", null ],
        [ "Option 2: Clean Break", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md182", null ]
      ] ],
      [ "Current Status", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md184", [
        [ "Completed (✅)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md185", null ],
        [ "In Progress (🚧)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md186", null ],
        [ "Not Started (⏳)", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md187", null ]
      ] ],
      [ "Next Steps", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md189", null ],
      [ "Estimated Effort", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md191", null ],
      [ "Notes", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md193", null ],
      [ "Conclusion", "md_docs_2developer_2_s_o_l_i_d___a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md195", null ]
    ] ],
    [ "SOLID Principles Implementation", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html", [
      [ "Overview", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md197", null ],
      [ "SOLID Principles Applied", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md199", [
        [ "1. **S**ingle Responsibility Principle (SRP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md200", [
          [ "Implementation:", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md201", null ]
        ] ],
        [ "2. **O**pen/Closed Principle (OCP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md203", [
          [ "Implementation:", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md204", null ]
        ] ],
        [ "3. **L**iskov Substitution Principle (LSP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md206", [
          [ "Implementation:", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md207", null ]
        ] ],
        [ "4. **I**nterface Segregation Principle (ISP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md209", [
          [ "Implementation:", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md210", null ]
        ] ],
        [ "5. **D**ependency Inversion Principle (DIP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md212", [
          [ "Implementation:", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md213", null ]
        ] ]
      ] ],
      [ "Architecture Diagram", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md215", null ],
      [ "Benefits Achieved", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md217", [
        [ "✅ Testability", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md218", null ],
        [ "✅ Extensibility", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md219", null ],
        [ "✅ Maintainability", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md220", null ],
        [ "✅ Flexibility", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md221", null ]
      ] ],
      [ "File Structure (SOLID-Compliant)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md223", null ],
      [ "Code Examples", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md225", [
        [ "Before SOLID (Monolithic)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md226", null ],
        [ "After SOLID (Decoupled)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md227", null ]
      ] ],
      [ "Testing Strategy", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md229", [
        [ "Unit Tests (Made Possible by SOLID)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md230", null ],
        [ "Integration Tests", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md231", null ]
      ] ],
      [ "Future Extensions (Made Easy by SOLID)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md233", [
        [ "1. New Output Formats (OCP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md234", null ],
        [ "2. Alternative Heuristics (OCP + LSP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md235", null ],
        [ "3. Parallel Solution Collection (OCP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md236", null ],
        [ "4. Custom Constraint Propagators (OCP)", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md237", null ]
      ] ],
      [ "Summary", "md_docs_2developer_2_s_o_l_i_d___p_r_i_n_c_i_p_l_e_s.html#autotoc_md239", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_config_8cpp.html",
"classslitherlink_1_1_state.html#aca5bdc02ffd1fe8f2afe2b8c78206bcc",
"struct_solver.html#a152377fc8dd52e014481b3f7a1f236fd"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';