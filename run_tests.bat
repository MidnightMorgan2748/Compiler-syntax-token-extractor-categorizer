@echo off
if not exist bin\test_runner.exe (
    echo [!] test_runner.exe not found. Building now...
    call build.bat
)

echo [*] Running unit tests...
bin\test_runner.exe
