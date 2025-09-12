# CHTL Language Support for VS Code

This extension provides language support for CHTL (C++ HyperText Language) in Visual Studio Code.

## Features

- **Syntax Highlighting**: Full syntax highlighting for CHTL files
- **Code Completion**: IntelliSense for CHTL elements, attributes, and functions
- **Code Formatting**: Automatic code formatting for CHTL files
- **Linting**: Real-time error detection and warnings
- **Hover Information**: Detailed information on hover
- **Go to Definition**: Navigate to definitions
- **Find References**: Find all references to symbols
- **Document Symbols**: Outline view for CHTL files
- **Workspace Symbols**: Search symbols across workspace
- **Code Actions**: Quick fixes and refactoring
- **Diagnostics**: Real-time error and warning reporting
- **Templates View**: Browse and manage templates
- **Customs View**: Browse and manage customs
- **Imports View**: Browse and manage imports
- **Modules View**: Browse and manage modules
- **Commands**: Various CHTL commands accessible via Command Palette
- **Context Menu**: Right-click context menu for CHTL files
- **Keybindings**: Keyboard shortcuts for common operations
- **Configuration**: Customizable settings

## Commands

- `CHTL: Compile CHTL` - Compile current CHTL file
- `CHTL: Parse CHTL` - Parse current CHTL file
- `CHTL: Format Document` - Format current CHTL file
- `CHTL: Lint Document` - Lint current CHTL file
- `CHTL: Preview HTML` - Preview compiled HTML
- `CHTL: Export HTML` - Export compiled HTML
- `CHTL: Build Project` - Build entire project
- `CHTL: Clean Project` - Clean project files
- `CHTL: Run Tests` - Run project tests
- `CHTL: Start Server` - Start development server
- `CHTL: Watch Files` - Watch files for changes

## Keybindings

- `Ctrl+Shift+C` - Compile CHTL
- `Ctrl+Shift+F` - Format Document
- `Ctrl+Shift+P` - Preview HTML

## Configuration

- `chtl.compiler.path` - Path to the CHTL compiler executable
- `chtl.compiler.options` - Additional options to pass to the CHTL compiler
- `chtl.format.enable` - Enable CHTL formatting
- `chtl.lint.enable` - Enable CHTL linting
- `chtl.preview.autoOpen` - Automatically open preview when compiling
- `chtl.preview.port` - Port for the preview server
- `chtl.watch.enable` - Enable file watching
- `chtl.watch.pattern` - File pattern to watch

## Requirements

- CHTL compiler must be installed and available in PATH
- VS Code 1.74.0 or higher

## Installation

1. Install the CHTL compiler
2. Install this extension from the VS Code marketplace
3. Open a CHTL file (.chtl extension)
4. The extension will automatically activate

## Usage

1. Open a CHTL file
2. Use the Command Palette (Ctrl+Shift+P) to access CHTL commands
3. Right-click on CHTL files for context menu options
4. Use the CHTL views in the Activity Bar to browse templates, customs, imports, and modules
5. Configure settings in VS Code preferences

## Development

To contribute to this extension:

1. Clone the repository
2. Run `npm install`
3. Open in VS Code
4. Press F5 to run the extension in a new window
5. Make changes and reload the window

## License

MIT License - see LICENSE file for details

## Support

For issues and feature requests, please visit the GitHub repository.