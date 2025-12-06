# Examples CMakeLists.txt

# Example 1: Simple usage
add_executable(example_simple_usage simple_usage.cpp)
target_compile_features(example_simple_usage PRIVATE cxx_std_17)

# Example 2: Thread configuration demo
add_executable(example_thread_config thread_config_demo.cpp)
target_compile_features(example_thread_config PRIVATE cxx_std_17)

# Example 3: Puzzle generator
add_executable(example_puzzle_generator puzzle_generator.cpp)
target_compile_features(example_puzzle_generator PRIVATE cxx_std_17)

# Install examples (optional)
install(TARGETS 
    example_simple_usage 
    example_thread_config 
    example_puzzle_generator
    DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
    OPTIONAL
)
