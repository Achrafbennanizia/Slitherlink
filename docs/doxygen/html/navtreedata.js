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
      [ "📁 Project Structure", "md__r_e_a_d_m_e.html#autotoc_md1", null ],
      [ "Table of Contents", "md__r_e_a_d_m_e.html#autotoc_md2", null ],
      [ "Overview", "md__r_e_a_d_m_e.html#autotoc_md4", [
        [ "What is Slitherlink?", "md__r_e_a_d_m_e.html#autotoc_md5", null ]
      ] ],
      [ "Features", "md__r_e_a_d_m_e.html#autotoc_md7", [
        [ "Core Capabilities", "md__r_e_a_d_m_e.html#autotoc_md8", null ],
        [ "Supported Puzzle Sizes", "md__r_e_a_d_m_e.html#autotoc_md9", null ]
      ] ],
      [ "Quick Start", "md__r_e_a_d_m_e.html#autotoc_md11", [
        [ "Run a Test Puzzle", "md__r_e_a_d_m_e.html#autotoc_md12", null ],
        [ "Run Comprehensive Benchmarks", "md__r_e_a_d_m_e.html#autotoc_md13", null ],
        [ "View Results", "md__r_e_a_d_m_e.html#autotoc_md14", null ]
      ] ],
      [ "Performance Highlights", "md__r_e_a_d_m_e.html#autotoc_md16", [
        [ "Version Evolution (V1 → V10)", "md__r_e_a_d_m_e.html#autotoc_md17", null ],
        [ "Key Optimizations", "md__r_e_a_d_m_e.html#autotoc_md18", null ],
        [ "Puzzle Difficulty Impact", "md__r_e_a_d_m_e.html#autotoc_md19", null ]
      ] ],
      [ "Architecture", "md__r_e_a_d_m_e.html#autotoc_md21", [
        [ "Data Structures", "md__r_e_a_d_m_e.html#autotoc_md22", [
          [ "<span class=\"tt\">Grid</span>", "md__r_e_a_d_m_e.html#autotoc_md23", null ],
          [ "<span class=\"tt\">Edge</span>", "md__r_e_a_d_m_e.html#autotoc_md24", null ],
          [ "<span class=\"tt\">State</span>", "md__r_e_a_d_m_e.html#autotoc_md25", null ],
          [ "<span class=\"tt\">Solution</span>", "md__r_e_a_d_m_e.html#autotoc_md26", null ]
        ] ]
      ] ],
      [ "Code Structure", "md__r_e_a_d_m_e.html#autotoc_md28", [
        [ "Main Solver Class", "md__r_e_a_d_m_e.html#autotoc_md29", null ],
        [ "Key Methods", "md__r_e_a_d_m_e.html#autotoc_md30", [
          [ "1. <span class=\"tt\">buildEdges()</span>", "md__r_e_a_d_m_e.html#autotoc_md31", null ],
          [ "2. <span class=\"tt\">calculateOptimalParallelDepth()</span>", "md__r_e_a_d_m_e.html#autotoc_md32", null ],
          [ "3. <span class=\"tt\">initialState()</span>", "md__r_e_a_d_m_e.html#autotoc_md33", null ],
          [ "4. <span class=\"tt\">applyDecision(State &amp;s, int edgeIdx, int val)</span>", "md__r_e_a_d_m_e.html#autotoc_md34", null ],
          [ "5. <span class=\"tt\">quickValidityCheck(const State &amp;s)</span>", "md__r_e_a_d_m_e.html#autotoc_md35", null ],
          [ "6. <span class=\"tt\">propagateConstraints(State &amp;s)</span>", "md__r_e_a_d_m_e.html#autotoc_md36", null ],
          [ "7. <span class=\"tt\">selectNextEdge(const State &amp;s)</span>", "md__r_e_a_d_m_e.html#autotoc_md37", null ],
          [ "8. <span class=\"tt\">finalCheckAndStore(State &amp;s)</span>", "md__r_e_a_d_m_e.html#autotoc_md38", null ],
          [ "9. <span class=\"tt\">search(State s, int depth)</span>", "md__r_e_a_d_m_e.html#autotoc_md39", null ],
          [ "10. <span class=\"tt\">run(bool allSolutions)</span>", "md__r_e_a_d_m_e.html#autotoc_md40", null ]
        ] ]
      ] ],
      [ "Algorithms", "md__r_e_a_d_m_e.html#autotoc_md42", [
        [ "Backtracking with Constraint Propagation", "md__r_e_a_d_m_e.html#autotoc_md43", null ],
        [ "Constraint Propagation Details", "md__r_e_a_d_m_e.html#autotoc_md44", null ],
        [ "Heuristic Edge Selection", "md__r_e_a_d_m_e.html#autotoc_md45", null ],
        [ "Cycle Verification", "md__r_e_a_d_m_e.html#autotoc_md46", null ]
      ] ],
      [ "Performance Optimization Journey", "md__r_e_a_d_m_e.html#autotoc_md48", null ],
      [ "Complete Development Journey", "md__r_e_a_d_m_e.html#autotoc_md50", [
        [ "Timeline Overview", "md__r_e_a_d_m_e.html#autotoc_md51", null ],
        [ "Version 1: Initial Implementation (Baseline)", "md__r_e_a_d_m_e.html#autotoc_md53", null ],
        [ "Experiment 1A: First TBB Attempt (Partial Failure)", "md__r_e_a_d_m_e.html#autotoc_md55", null ],
        [ "Version 2: TBB Integration (Success)", "md__r_e_a_d_m_e.html#autotoc_md57", null ],
        [ "Version 3: CPU Limiting (50% Usage)", "md__r_e_a_d_m_e.html#autotoc_md59", null ],
        [ "Experiment 2A: Adaptive Depth (First Attempt - Failed)", "md__r_e_a_d_m_e.html#autotoc_md61", null ],
        [ "Experiment 2B: Density-Based Depth (Partial Success)", "md__r_e_a_d_m_e.html#autotoc_md63", null ],
        [ "Version 4: Dynamic Parallel Depth (Success)", "md__r_e_a_d_m_e.html#autotoc_md65", null ],
        [ "Version 5: Intelligent Edge Selection", "md__r_e_a_d_m_e.html#autotoc_md67", null ],
        [ "Experiment 3A: Simple Forward Checking (Partial Success)", "md__r_e_a_d_m_e.html#autotoc_md69", null ],
        [ "Version 6: Enhanced Constraint Propagation", "md__r_e_a_d_m_e.html#autotoc_md71", null ],
        [ "Experiment 4A: Google OR-Tools Integration (Major Failure)", "md__r_e_a_d_m_e.html#autotoc_md73", null ],
        [ "Version 7: OR-Tools Removal &amp; Recovery", "md__r_e_a_d_m_e.html#autotoc_md75", null ],
        [ "Version 8: Code Cleanup", "md__r_e_a_d_m_e.html#autotoc_md77", null ],
        [ "Version 2: TBB Integration", "md__r_e_a_d_m_e.html#autotoc_md79", null ],
        [ "Version 3: CPU Limiting (50% Usage)", "md__r_e_a_d_m_e.html#autotoc_md81", null ],
        [ "Version 4: Dynamic Parallel Depth", "md__r_e_a_d_m_e.html#autotoc_md83", null ],
        [ "Version 5: Intelligent Edge Selection", "md__r_e_a_d_m_e.html#autotoc_md85", null ],
        [ "Version 6: Enhanced Constraint Propagation", "md__r_e_a_d_m_e.html#autotoc_md87", null ],
        [ "Version 7: OR-Tools Experiment (Failed)", "md__r_e_a_d_m_e.html#autotoc_md89", null ],
        [ "Version 8: Code Cleanup", "md__r_e_a_d_m_e.html#autotoc_md91", null ],
        [ "Version 9: TBB Optimization in Final Check", "md__r_e_a_d_m_e.html#autotoc_md93", null ],
        [ "Version 10: Lambda Optimization &amp; Code Polish", "md__r_e_a_d_m_e.html#autotoc_md95", [
          [ "1. Simplified Edge Selection with Lambda Helper", "md__r_e_a_d_m_e.html#autotoc_md96", null ],
          [ "2. Streamlined Cycle Building", "md__r_e_a_d_m_e.html#autotoc_md97", null ],
          [ "3. Added Inline Hints for Hot Functions", "md__r_e_a_d_m_e.html#autotoc_md98", null ]
        ] ]
      ] ],
      [ "Final Comprehensive Comparison", "md__r_e_a_d_m_e.html#autotoc_md100", [
        [ "Performance Summary Table", "md__r_e_a_d_m_e.html#autotoc_md101", null ],
        [ "Optimization Impact Breakdown", "md__r_e_a_d_m_e.html#autotoc_md102", null ],
        [ "Cumulative Speedup Analysis", "md__r_e_a_d_m_e.html#autotoc_md103", null ],
        [ "Code Evolution Timeline", "md__r_e_a_d_m_e.html#autotoc_md104", null ],
        [ "Failed Experiments (Learning Journey)", "md__r_e_a_d_m_e.html#autotoc_md105", null ],
        [ "Success Factors", "md__r_e_a_d_m_e.html#autotoc_md106", null ],
        [ "Key Technical Insights", "md__r_e_a_d_m_e.html#autotoc_md107", null ]
      ] ],
      [ "Lessons for Future Work", "md__r_e_a_d_m_e.html#autotoc_md109", [
        [ "What We'd Try Next (Time Permitting)", "md__r_e_a_d_m_e.html#autotoc_md110", null ],
        [ "What We Wouldn't Try Again", "md__r_e_a_d_m_e.html#autotoc_md111", null ]
      ] ],
      [ "Appendix: Complete Development Timeline", "md__r_e_a_d_m_e.html#autotoc_md113", [
        [ "Week 1: Foundation (Days 1-7)", "md__r_e_a_d_m_e.html#autotoc_md114", null ],
        [ "Week 2: Optimization (Days 8-14)", "md__r_e_a_d_m_e.html#autotoc_md115", null ],
        [ "Week 3: Advanced Features &amp; Recovery (Days 15-24)", "md__r_e_a_d_m_e.html#autotoc_md116", null ],
        [ "Week 4: Polish &amp; Documentation (Days 25-28)", "md__r_e_a_d_m_e.html#autotoc_md117", null ],
        [ "Final Performance Summary", "md__r_e_a_d_m_e.html#autotoc_md120", null ]
      ] ],
      [ "Build &amp; Usage", "md__r_e_a_d_m_e.html#autotoc_md122", [
        [ "Prerequisites", "md__r_e_a_d_m_e.html#autotoc_md123", null ],
        [ "Build", "md__r_e_a_d_m_e.html#autotoc_md124", null ],
        [ "Usage", "md__r_e_a_d_m_e.html#autotoc_md125", null ],
        [ "Puzzle File Format", "md__r_e_a_d_m_e.html#autotoc_md126", null ]
      ] ],
      [ "Performance Benchmarks", "md__r_e_a_d_m_e.html#autotoc_md128", [
        [ "Test Environment", "md__r_e_a_d_m_e.html#autotoc_md129", null ],
        [ "Benchmark Results", "md__r_e_a_d_m_e.html#autotoc_md130", [
          [ "Small Puzzles (4×4, 5×5)", "md__r_e_a_d_m_e.html#autotoc_md131", null ],
          [ "Medium Puzzles (6×6, 7×7, 8×8)", "md__r_e_a_d_m_e.html#autotoc_md132", null ],
          [ "Large Puzzles (10×10+)", "md__r_e_a_d_m_e.html#autotoc_md133", null ]
        ] ],
        [ "Performance Characteristics", "md__r_e_a_d_m_e.html#autotoc_md134", null ]
      ] ],
      [ "Testing &amp; Benchmarking", "md__r_e_a_d_m_e.html#autotoc_md136", [
        [ "Automated Test Suite", "md__r_e_a_d_m_e.html#autotoc_md137", null ],
        [ "Test Coverage", "md__r_e_a_d_m_e.html#autotoc_md138", null ],
        [ "Benchmark Results Summary", "md__r_e_a_d_m_e.html#autotoc_md139", null ],
        [ "Understanding Puzzle Difficulty", "md__r_e_a_d_m_e.html#autotoc_md140", null ],
        [ "Puzzle Difficulty Examples", "md__r_e_a_d_m_e.html#autotoc_md141", null ],
        [ "How the Algorithm Responds", "md__r_e_a_d_m_e.html#autotoc_md142", [
          [ "On Dense Puzzles (60%+ clues):", "md__r_e_a_d_m_e.html#autotoc_md143", null ],
          [ "On Sparse Puzzles (&lt;30% clues):", "md__r_e_a_d_m_e.html#autotoc_md144", null ]
        ] ],
        [ "Creating Test Puzzles", "md__r_e_a_d_m_e.html#autotoc_md145", null ],
        [ "Running Custom Tests", "md__r_e_a_d_m_e.html#autotoc_md146", null ],
        [ "Complete Documentation", "md__r_e_a_d_m_e.html#autotoc_md147", null ]
      ] ],
      [ "Technical Details", "md__r_e_a_d_m_e.html#autotoc_md150", [
        [ "TBB Components Used", "md__r_e_a_d_m_e.html#autotoc_md151", [
          [ "<span class=\"tt\">tbb::task_arena</span>", "md__r_e_a_d_m_e.html#autotoc_md152", null ],
          [ "<span class=\"tt\">tbb::task_group</span>", "md__r_e_a_d_m_e.html#autotoc_md153", null ],
          [ "<span class=\"tt\">tbb::concurrent_vector</span>", "md__r_e_a_d_m_e.html#autotoc_md154", null ],
          [ "<span class=\"tt\">tbb::parallel_reduce</span>", "md__r_e_a_d_m_e.html#autotoc_md155", null ],
          [ "<span class=\"tt\">tbb::parallel_for</span>", "md__r_e_a_d_m_e.html#autotoc_md156", null ],
          [ "<span class=\"tt\">tbb::spin_mutex</span>", "md__r_e_a_d_m_e.html#autotoc_md157", null ]
        ] ],
        [ "Memory Management", "md__r_e_a_d_m_e.html#autotoc_md158", null ],
        [ "Compiler Optimizations", "md__r_e_a_d_m_e.html#autotoc_md159", null ],
        [ "Debug vs Release Performance", "md__r_e_a_d_m_e.html#autotoc_md160", null ]
      ] ],
      [ "Testing", "md__r_e_a_d_m_e.html#autotoc_md162", [
        [ "Test Suite", "md__r_e_a_d_m_e.html#autotoc_md163", null ],
        [ "Test Results", "md__r_e_a_d_m_e.html#autotoc_md164", null ]
      ] ],
      [ "Debugging &amp; Troubleshooting", "md__r_e_a_d_m_e.html#autotoc_md166", [
        [ "Common Issues", "md__r_e_a_d_m_e.html#autotoc_md167", [
          [ "1. <b>Slow Performance</b>", "md__r_e_a_d_m_e.html#autotoc_md168", null ],
          [ "2. <b>Segmentation Fault</b>", "md__r_e_a_d_m_e.html#autotoc_md169", null ],
          [ "3. <b>No Solution Found</b>", "md__r_e_a_d_m_e.html#autotoc_md170", null ],
          [ "4. <b>High CPU Usage</b>", "md__r_e_a_d_m_e.html#autotoc_md171", null ]
        ] ]
      ] ],
      [ "Future Improvements", "md__r_e_a_d_m_e.html#autotoc_md173", [
        [ "Potential Optimizations", "md__r_e_a_d_m_e.html#autotoc_md174", null ],
        [ "Known Limitations", "md__r_e_a_d_m_e.html#autotoc_md175", null ]
      ] ],
      [ "References", "md__r_e_a_d_m_e.html#autotoc_md177", [
        [ "Slitherlink Resources", "md__r_e_a_d_m_e.html#autotoc_md178", null ],
        [ "Technical References", "md__r_e_a_d_m_e.html#autotoc_md179", null ],
        [ "Papers", "md__r_e_a_d_m_e.html#autotoc_md180", null ]
      ] ],
      [ "License", "md__r_e_a_d_m_e.html#autotoc_md182", null ],
      [ "Author", "md__r_e_a_d_m_e.html#autotoc_md183", null ],
      [ "Appendix: Complete Optimization Timeline", "md__r_e_a_d_m_e.html#autotoc_md185", [
        [ "Phase 1: Baseline (Week 1)", "md__r_e_a_d_m_e.html#autotoc_md186", null ],
        [ "Phase 2: TBB Integration (Week 1)", "md__r_e_a_d_m_e.html#autotoc_md187", null ],
        [ "Phase 3: Resource Control (Week 2)", "md__r_e_a_d_m_e.html#autotoc_md188", null ],
        [ "Phase 4: Dynamic Optimization (Week 2)", "md__r_e_a_d_m_e.html#autotoc_md189", null ],
        [ "Phase 5: Heuristic Enhancement (Week 2)", "md__r_e_a_d_m_e.html#autotoc_md190", null ],
        [ "Phase 6: Propagation Optimization (Week 3)", "md__r_e_a_d_m_e.html#autotoc_md191", null ],
        [ "Phase 7: OR-Tools Experiment (Week 3)", "md__r_e_a_d_m_e.html#autotoc_md192", null ],
        [ "Phase 8: Code Cleanup (Week 3)", "md__r_e_a_d_m_e.html#autotoc_md193", null ],
        [ "Phase 9: TBB Enhancement (Week 4)", "md__r_e_a_d_m_e.html#autotoc_md194", null ],
        [ "Phase 10: Final Polish (Week 4)", "md__r_e_a_d_m_e.html#autotoc_md195", null ]
      ] ],
      [ "📚 Documentation", "md__r_e_a_d_m_e.html#autotoc_md198", [
        [ "Complete Documentation Index", "md__r_e_a_d_m_e.html#autotoc_md199", null ],
        [ "Quick Links by Purpose", "md__r_e_a_d_m_e.html#autotoc_md200", [
          [ "For Users &amp; Testing", "md__r_e_a_d_m_e.html#autotoc_md201", null ],
          [ "For Understanding Performance", "md__r_e_a_d_m_e.html#autotoc_md202", null ],
          [ "For Understanding Code Evolution", "md__r_e_a_d_m_e.html#autotoc_md203", null ],
          [ "For Learning from Failures", "md__r_e_a_d_m_e.html#autotoc_md204", null ]
        ] ],
        [ "Historical Code Versions", "md__r_e_a_d_m_e.html#autotoc_md205", null ]
      ] ]
    ] ],
    [ "Quick Reference - Improved Architecture", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html", [
      [ "📁 Project Structure", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md208", null ],
      [ "🎯 Key Files", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md209", [
        [ "Public API", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md210", null ],
        [ "Core Data", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md211", null ],
        [ "Main Solver", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md212", null ],
        [ "Factory", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md213", null ]
      ] ],
      [ "🔧 Build Commands", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md214", null ],
      [ "📦 Build Artifacts", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md215", null ],
      [ "💻 Usage Examples", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md216", [
        [ "CLI Application", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md217", null ],
        [ "Library Integration (C++)", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md218", null ],
        [ "Custom Configuration", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md219", null ]
      ] ],
      [ "📊 Architecture Layers", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md220", null ],
      [ "🔍 Include Paths", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md221", [
        [ "Before (Old)", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md222", null ],
        [ "After (New)", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md223", null ]
      ] ],
      [ "🎨 Namespace Usage", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md224", null ],
      [ "📝 CMake Integration", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md225", [
        [ "In your CMakeLists.txt", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md226", null ]
      ] ],
      [ "🛠️ Extension Points", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md227", [
        [ "Add Custom Heuristic", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md228", null ],
        [ "Add Custom Output", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md229", null ]
      ] ],
      [ "📚 Documentation Files", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md230", null ],
      [ "✅ Verification", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md231", null ],
      [ "🎯 Quick Start", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md232", null ],
      [ "🚀 Next Steps", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md233", null ],
      [ "📈 Version", "md__q_u_i_c_k___r_e_f_e_r_e_n_c_e.html#autotoc_md234", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
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
"_common_8h.html",
"classslitherlink_1_1utils_1_1_logger.html#a1b117f05b43963fd79c7ddc755d85295",
"namespaceslitherlink.html#a64ebd5a186a5682085606ab3dadb792d"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';