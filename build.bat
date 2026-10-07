@echo off
setlocal enabledelayedexpansion

echo ======================================================================
echo    Compiling Group 5: Compiler Syntax Token Extractor and Categorizer
echo ======================================================================

if not exist bin mkdir bin
if not exist export mkdir export

set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if exist %VCVARS% (
    call %VCVARS% > nul
) else (
    echo [!] vcvars64.bat not found at default location. Assuming cl.exe is in PATH.
)

echo [*] Compiling main executable (bin/syntax_analyzer.exe)...
cl.exe /nologo /EHsc /std:c++17 /O2 /Iinclude ^
    src\main.cpp ^
    src\Token.cpp ^
    src\Tokenizer.cpp ^
    src\KeywordExtractor.cpp ^
    src\SyntaxCategorizer.cpp ^
    src\ComplexityEvaluator.cpp ^
    src\JsonExporter.cpp ^
    /Fe:bin\syntax_analyzer.exe ^
    /Fo:bin\

if %errorlevel% neq 0 (
    echo [-] Compilation of main executable failed!
    exit /b %errorlevel%
)

echo [*] Compiling test suite (bin/test_runner.exe)...
cl.exe /nologo /EHsc /std:c++17 /O2 /Iinclude ^
    tests\test_runner.cpp ^
    src\Token.cpp ^
    src\Tokenizer.cpp ^
    src\KeywordExtractor.cpp ^
    src\SyntaxCategorizer.cpp ^
    src\ComplexityEvaluator.cpp ^
    src\JsonExporter.cpp ^
    /Fe:bin\test_runner.exe ^
    /Fo:bin\

if %errorlevel% neq 0 (
    echo [-] Compilation of test runner failed!
    exit /b %errorlevel%
)

echo.
echo [+] Build completed successfully!
echo     - Application : bin\syntax_analyzer.exe
echo     - Test Suite  : bin\test_runner.exe
echo.
