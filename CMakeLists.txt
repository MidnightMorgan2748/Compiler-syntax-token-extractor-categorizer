cmake_minimum_required(VERSION 3.15)
project(CompilerSyntaxTokenExtractor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(COMMON_SOURCES
    src/Token.cpp
    src/Tokenizer.cpp
    src/KeywordExtractor.cpp
    src/SyntaxCategorizer.cpp
    src/ComplexityEvaluator.cpp
    src/JsonExporter.cpp
)

# Main Executable
add_executable(syntax_analyzer
    src/main.cpp
    ${COMMON_SOURCES}
)

# Test Suite Executable
add_executable(test_runner
    tests/test_runner.cpp
    ${COMMON_SOURCES}
)

set_target_properties(syntax_analyzer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(test_runner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
