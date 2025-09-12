#!/bin/bash

# CHTL VSCode Extension Build Script

echo "Building CHTL VSCode Extension..."

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo "Error: Node.js is not installed. Please install Node.js first."
    exit 1
fi

# Check if npm is installed
if ! command -v npm &> /dev/null; then
    echo "Error: npm is not installed. Please install npm first."
    exit 1
fi

# Install dependencies
echo "Installing dependencies..."
npm install

# Compile TypeScript
echo "Compiling TypeScript..."
npm run compile

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "Extension is ready to use."
else
    echo "❌ Build failed!"
    exit 1
fi

# Create package
echo "Creating extension package..."
npx vsce package

echo "✅ Extension package created successfully!"
echo "You can install the .vsix file in VS Code."