@echo off
echo CHTL Windows Build Script
echo ========================

echo.
echo Checking Rust installation...
rustc --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Rust is not installed!
    echo Please install Rust from https://rustup.rs/
    pause
    exit /b 1
)

echo Rust is installed: 
rustc --version

echo.
echo Checking Cargo installation...
cargo --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Cargo is not installed!
    pause
    exit /b 1
)

echo Cargo is installed:
cargo --version

echo.
echo Adding Windows target...
rustup target add x86_64-pc-windows-gnu
if %errorlevel% neq 0 (
    echo WARNING: Failed to add Windows target, trying MSVC...
    rustup target add x86_64-pc-windows-msvc
)

echo.
echo Building CHTL compiler...
echo This may take a few minutes...

cargo build --release --target x86_64-pc-windows-gnu
if %errorlevel% neq 0 (
    echo Trying MSVC target...
    cargo build --release --target x86_64-pc-windows-msvc
    if %errorlevel% neq 0 (
        echo ERROR: Build failed!
        pause
        exit /b 1
    )
    set TARGET_DIR=x86_64-pc-windows-msvc
) else (
    set TARGET_DIR=x86_64-pc-windows-gnu
)

echo.
echo Build successful!
echo.

echo Running tests...
cargo test --target %TARGET_DIR%
if %errorlevel% neq 0 (
    echo WARNING: Some tests failed!
) else (
    echo All tests passed!
)

echo.
echo Creating distribution package...

if not exist "dist" mkdir dist
if not exist "dist\bin" mkdir dist\bin
if not exist "dist\examples" mkdir dist\examples
if not exist "dist\docs" mkdir dist\docs

echo Copying executable...
copy "target\%TARGET_DIR%\release\chtl.exe" "dist\bin\"

echo Copying examples...
copy "examples\*.chtl" "dist\examples\"

echo Copying documentation...
copy "*.md" "dist\docs\"

echo.
echo Creating README for Windows...
(
echo CHTL Windows Distribution
echo =========================
echo.
echo This is a pre-compiled Windows distribution of CHTL.
echo.
echo Files:
echo - bin\chtl.exe          - CHTL compiler executable
echo - examples\             - Example CHTL files
echo - docs\                 - Documentation
echo.
echo Usage:
echo   chtl compile input.chtl
echo   chtl init my-project
echo   chtl build src/
echo   chtl serve --port 3000
echo.
echo For more information, see the documentation in the docs\ folder.
) > "dist\README.txt"

echo.
echo Creating installer script...
(
echo @echo off
echo echo Installing CHTL...
echo.
echo if not exist "C:\chtl" mkdir "C:\chtl"
echo copy "bin\chtl.exe" "C:\chtl\"
echo.
echo echo Adding to PATH...
echo setx PATH "%%PATH%%;C:\chtl" /M
echo.
echo echo Installation complete!
echo echo Please restart your command prompt to use CHTL.
echo pause
) > "dist\install.bat"

echo.
echo Distribution package created in 'dist' folder:
dir dist

echo.
echo Build Summary:
echo ==============
echo - Executable: dist\bin\chtl.exe
echo - Examples: dist\examples\
echo - Documentation: dist\docs\
echo - Installer: dist\install.bat
echo.
echo To install CHTL system-wide, run: dist\install.bat
echo.
echo Build completed successfully!
pause