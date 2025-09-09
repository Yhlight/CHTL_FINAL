@echo off
REM CHTL Windows Installation Script

echo === CHTL Windows Installation Program ===
echo.

REM Check if binary files exist
if not exist "build\bin\chtl.exe" (
    echo Error: CHTL compiler binary files not found
    echo Please compile the project first: make -j4
    pause
    exit /b 1
)

REM Create installation directory
set "INSTALL_DIR=%USERPROFILE%\CHTL"
echo Installation directory: %INSTALL_DIR%

if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
    echo Created installation directory: %INSTALL_DIR%
)

REM Copy binary files
echo Copying binary files...
copy "build\bin\chtl.exe" "%INSTALL_DIR%\"
if not exist "%INSTALL_DIR%\lib" mkdir "%INSTALL_DIR%\lib"
copy "build\lib\*.a" "%INSTALL_DIR%\lib\"

REM Copy test suite
echo Copying test suite...
xcopy "test_suite\*" "%INSTALL_DIR%\test_suite\" /E /I

REM Copy documentation
echo Copying documentation...
copy "README_WINDOWS.md" "%INSTALL_DIR%\"
copy "PROJECT_SUMMARY.md" "%INSTALL_DIR%\"
copy "FINAL_SUMMARY.md" "%INSTALL_DIR%\"

REM Create launcher script
echo Creating launcher script...
echo @echo off > "%INSTALL_DIR%\chtl.bat"
echo "%INSTALL_DIR%\chtl.exe" %%* >> "%INSTALL_DIR%\chtl.bat"

REM Create test runner script
echo Creating test runner script...
echo @echo off > "%INSTALL_DIR%\run_tests.bat"
echo cd /d "%INSTALL_DIR%\test_suite" >> "%INSTALL_DIR%\run_tests.bat"
echo run_tests_windows.bat >> "%INSTALL_DIR%\run_tests.bat"

REM Add to PATH
echo Adding to PATH...
setx PATH "%PATH%;%INSTALL_DIR%" /M
if %errorlevel% neq 0 (
    echo Warning: Unable to automatically add to system PATH
    echo Please manually add %INSTALL_DIR% to system PATH
)

echo.
echo === Installation Complete ===
echo CHTL has been installed to: %INSTALL_DIR%
echo.
echo Usage:
echo   chtl input.chtl                    # Clean output for SPA
echo   chtl --default-struct input.chtl  # Complete HTML document
echo   chtl --help                        # Show help
echo.
echo Run tests:
echo   run_tests.bat
echo.
echo Test files location: %INSTALL_DIR%\test_suite\
echo.

REM Run tests
set /p "run_tests=Would you like to run tests now? (y/n): "
if /i "%run_tests%"=="y" (
    echo.
    echo Running test suite...
    cd /d "%INSTALL_DIR%\test_suite"
    run_tests_windows.bat
)

echo.
echo Installation complete! Press any key to exit...
pause > nul