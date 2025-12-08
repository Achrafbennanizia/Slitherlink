# Documentation

> Doxygen/auto-generated docs reference. For current layout and entrypoints, see `README.md` and `docs/developer/ARCHITECTURE.md`.

## Generated API Documentation

This directory contains auto-generated API documentation using Doxygen.

### Viewing the Documentation

Open `doxygen/html/index.html` in your web browser:

```bash
open docs/doxygen/html/index.html   # macOS
xdg-open docs/doxygen/html/index.html   # Linux
start docs/doxygen/html/index.html  # Windows
```

Or start a local server:

```bash
cd docs/doxygen/html
python3 -m http.server 8000
# Then open http://localhost:8000 in your browser
```

### Regenerating Documentation

To regenerate the documentation after code changes:

```bash
doxygen Doxyfile
```

The configuration is in the root `Doxyfile`.

### What's Documented

The documentation includes:

- **Data Structures**: Grid, State, Edge, Solution
- **Classes**: Solver class with all methods
- **Functions**: I/O functions, main entry point
- **Source Code**: Browsable source with cross-references
- **Call Graphs**: Visual representation of function calls (if Graphviz is installed)
- **Class Diagrams**: UML-style class relationships

### Documentation Standards

When adding new code, use Doxygen-style comments:

```cpp
/**
 * @brief Short description
 *
 * Longer description that explains the purpose
 * and behavior of the function or class.
 *
 * @param paramName Description of parameter
 * @return Description of return value
 * @throws ExceptionType When this exception is thrown
 */
```

### Markdown Files

The following markdown files are included in the documentation:

- README.md - Project overview
- QUICK_REFERENCE.md - Quick start guide
- ARCHITECTURE_IMPROVEMENTS.md - Architecture documentation
- PERFORMANCE_OPTIMIZATION.md - Performance notes

### Continuous Documentation

Consider integrating documentation generation into your CI/CD pipeline:

```yaml
- name: Generate Docs
  run: |
    doxygen Doxyfile

- name: Deploy to GitHub Pages
  uses: peaceiris/actions-gh-pages@v3
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./docs/doxygen/html
```

## Manual Documentation

See the following files for comprehensive guides:

- **PROJECT_ARCHITECTURE_COMPLETE.md** - Complete architecture overview
- **QUICK_REFERENCE.md** - Quick reference guide
- **docs/IMPROVED_ARCHITECTURE.md** - Detailed architecture documentation
- **PERFORMANCE_OPTIMIZATION.md** - Performance optimization guide
