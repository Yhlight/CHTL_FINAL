@echo off
REM CHTL Test Suite Runner (Windows Version)
REM Tests all CHTL syntax features and functionality

echo === CHTL Test Suite ===
echo Starting all tests...

REM Set colors
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "NC=[0m"

REM Test counters
set /a TOTAL_TESTS=0
set /a PASSED_TESTS=0
set /a FAILED_TESTS=0

REM Test function
:run_test
set "test_name=%~1"
set "input_file=%~2"
set "expected_output=%~3"

echo.
echo %YELLOW%Running test: %test_name%%NC%
echo Input file: %input_file%

set /a TOTAL_TESTS+=1

REM Run CHTL compiler
..\build\bin\chtl.exe "%input_file%" > "output_%test_name%.html" 2>&1
if %errorlevel% equ 0 (
    echo %GREEN%✓ Compilation successful%NC%
    set /a PASSED_TESTS+=1
    
    REM Show output content
    echo Output content:
    type "output_%test_name%.html"
    echo.
) else (
    echo %RED%✗ Compilation failed%NC%
    set /a FAILED_TESTS+=1
    
    REM Show error information
    echo Error information:
    type "output_%test_name%.html"
    echo.
)
goto :eof

REM Run all tests
echo Starting basic syntax tests...
call :run_test "basic_syntax" "basic_syntax.chtl" ""

echo Starting template system tests...
call :run_test "template_system" "template_system.chtl" ""

echo Starting custom system tests...
call :run_test "custom_system" "custom_system.chtl" ""

echo Starting origin embedding tests...
call :run_test "origin_embedding" "origin_embedding.chtl" ""

echo Starting import system tests...
call :run_test "import_system" "import_system.chtl" ""

echo Starting constraint system tests...
call :run_test "constraint_system" "constraint_system.chtl" ""

echo Starting configuration system tests...
call :run_test "config_system" "config_system.chtl" ""

echo Starting namespace system tests...
call :run_test "namespace_system" "namespace_system.chtl" ""

echo Starting CHTL JS extended syntax tests...
call :run_test "chtl_js_extended" "chtl_js_extended.chtl" ""

echo Starting module system tests...
call :run_test "module_system" "module_system.chtl" ""

echo Starting SPA page tests...
call :run_test "spa_page" "spa_page.chtl" ""

echo Starting complete features tests...
call :run_test "complete_features" "complete_features.chtl" ""

REM Test default structure output
echo.
echo %YELLOW%Running default structure test: spa_page_default_struct%NC%
echo Input file: spa_page.chtl

set /a TOTAL_TESTS+=1

..\build\bin\chtl.exe --default-struct "spa_page.chtl" > "output_spa_page_default_struct.html" 2>&1
if %errorlevel% equ 0 (
    echo %GREEN%✓ Default structure compilation successful%NC%
    set /a PASSED_TESTS+=1
    
    REM Show output content
    echo Output content:
    type "output_spa_page_default_struct.html"
    echo.
) else (
    echo %RED%✗ Default structure compilation failed%NC%
    set /a FAILED_TESTS+=1
    
    REM Show error information
    echo Error information:
    type "output_spa_page_default_struct.html"
    echo.
)

REM Show test results
echo.
echo === Test Results ===
echo Total tests: %TOTAL_TESTS%
echo %GREEN%Passed: %PASSED_TESTS%%NC%
echo %RED%Failed: %FAILED_TESTS%%NC%

if %FAILED_TESTS% equ 0 (
    echo.
    echo %GREEN%🎉 All tests passed!%NC%
    exit /b 0
) else (
    echo.
    echo %RED%❌ %FAILED_TESTS% tests failed%NC%
    exit /b 1
)