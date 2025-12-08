# Improvement Options for Slitherlink Solver

This file summarizes concrete improvement areas to bring the codebase and documentation in line with industry standards. Each item includes what to change and why it matters.

## 1) Align Docs with Reality
- **Issue**: Several docs still describe the pre-migration layout (root `main.cpp`, 50 puzzles, numerous scripts) and counts that no longer match the tree.
- **Action**: Refresh structure diagrams, file counts, and reading paths so they match `apps/slitherlink_cli/main.cpp`, the current `puzzles/samples/` set, and the actual docs/scripts.
- **Impact**: Reduces onboarding confusion and prevents stale guidance.

## 2) Fix Public API & Library Wiring
- **Issue**: `SlitherlinkAPI` calls a `SolverFactory::createSolver(const SolverConfig&)` overload that is not implemented, and the CMake library target builds only a dummy source.
- **Action**: Either (a) implement the config-based factory, wire real sources into the library target, and expose a linkable artifact, or (b) hide/remove the API from headers and docs until it works.
- **Impact**: Ensures downstream consumers can actually link and use the advertised library.

## 3) Clarify Optional vs Required Dependencies
- **Issue**: TBB headers are included unconditionally in `src/solver/Solver.cpp`, so “optional” TBB will fail to build when absent.
- **Action**: Guard TBB includes/usage with `#ifdef USE_TBB` and provide a fallback path (std::async/thread), or declare TBB as required in docs/CMake and fail fast.
- **Impact**: Predictable builds and clear expectations for contributors and users.

## 4) Make Tests Exercise Production Code
- **Issue**: Unit tests re-declare tiny structs instead of including real headers; tests are off by default and the library target is a stub.
- **Action**: Point tests at real headers, enable `SLITHERLINK_BUILD_TESTS` in CI presets, and add coverage for grid parsing, solving a small puzzle, and failure paths. Ensure test builds link the actual sources.
- **Impact**: Catches regressions in real code paths and validates solver behavior.

## 5) Align Messaging with Shipped Features
- **Issue**: Docs promise “production ready,” GoogleTest suite, examples, packaging, and CI, but several are missing or stubbed.
- **Action**: Either implement the missing pieces (CI workflow, real examples, install/package steps) or trim the promises in README/docs to reflect the current state.
- **Impact**: Maintains credibility and sets accurate expectations for users.

## 6) Add Housekeeping Essentials
- **Issue**: No LICENSE, CONTRIBUTING, or CHANGELOG despite being referenced; Doxygen/docs aren’t tied to any build step.
- **Action**: Add a license, contributor guide, and changelog; consider a lightweight doc check (toc/links) to keep docs consistent.
- **Impact**: Meets open-source norms and improves maintainability.

## 7) Continuous Integration & Build Matrix
- **Issue**: No CI is present to enforce builds/tests across configurations.
- **Action**: Add CI to build and test with/without TBB, both Debug/Release, and to run a small puzzle smoke test. Cache dependencies where possible.
- **Impact**: Prevents breakage, documents supported configurations, and provides fast feedback.

## 8) Single Canonical Architecture Story
- **Issue**: Code currently mixes a monolithic CLI with a partially scaffolded SOLID/library stack.
- **Action**: Choose the canonical path (monolithic CLI or SOLID library) and make docs, CMake, and tests consistent. If both must coexist, document how they relate and which one is supported.
- **Impact**: Clearer mental model for contributors and easier future refactors.
