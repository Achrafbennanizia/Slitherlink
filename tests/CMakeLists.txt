# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test executable for Grid tests
add_executable(test_grid test_grid.cpp)
target_link_libraries(test_grid PRIVATE GTest::gtest_main)
target_compile_features(test_grid PRIVATE cxx_std_17)

# Test executable for Solver basic tests
add_executable(test_solver_basic test_solver_basic.cpp)
target_link_libraries(test_solver_basic PRIVATE GTest::gtest_main)
target_compile_features(test_solver_basic PRIVATE cxx_std_17)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_grid)
gtest_discover_tests(test_solver_basic)
