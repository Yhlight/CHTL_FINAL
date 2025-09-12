# CHTL Language Extension for Visual Studio Code

This extension provides comprehensive support for the CHTL language in Visual Studio Code, including syntax highlighting, code completion, error checking, and integrated compilation.

## Features

### ✨ Language Support
- **Syntax Highlighting** - Full syntax highlighting for CHTL and CHTL JS files
- **Code Completion** - Intelligent code completion for HTML tags, CHTL keywords, and CSS properties
- **Error Checking** - Real-time error detection and validation
- **Hover Information** - Detailed information about HTML tags and CHTL keywords
- **Code Snippets** - Pre-built code snippets for common patterns

### 🔧 Compilation & Build
- **One-Click Compilation** - Compile CHTL files with a single command
- **Auto-Compilation** - Automatically compile files on save
- **Watch Mode** - Monitor files for changes and recompile automatically
- **Multiple Export Formats** - Export HTML, CSS, and JavaScript separately

### 📁 Project Management
- **Project Initialization** - Create new CHTL projects with proper structure
- **Module Management** - View and manage CHTL modules in the sidebar
- **File Organization** - Automatic project structure with src, modules, and dist directories

### 🎨 Development Experience
- **Live Preview** - Preview compiled HTML in the browser
- **Integrated Terminal** - Run CHTL commands directly from VS Code
- **Error Reporting** - Clear error messages and suggestions
- **Configuration** - Customizable settings for compilation and development

## Installation

1. Install the extension from the VS Code marketplace
2. Open a CHTL project or create a new one
3. Start coding with full language support!

## Usage

### Basic Commands

- `Ctrl+Shift+C` (or `Cmd+Shift+C` on Mac) - Compile current CHTL file
- `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac) - Preview in browser
- Right-click on CHTL files for context menu options

### Project Initialization

1. Open Command Palette (`Ctrl+Shift+P`)
2. Type "CHTL: Initialize Project"
3. Enter project name
4. Project structure will be created automatically

### Module Management

- View all modules in the CHTL sidebar
- Right-click modules for management options
- Automatic module discovery and validation

## Configuration

The extension can be configured through VS Code settings:

```json
{
  "chtl.compilerPath": "chtl",
  "chtl.outputDirectory": "dist",
  "chtl.watchMode": false,
  "chtl.minify": false,
  "chtl.debug": false,
  "chtl.autoCompile": true,
  "chtl.previewPort": 8080
}
```

## File Types Supported

- `.chtl` - CHTL source files
- `.cmod` - CHTL module files
- `.cjjs` - CHTL JS source files
- `.cjmod` - CHTL JS module files

## Requirements

- Visual Studio Code 1.74.0 or higher
- CHTL Compiler installed and accessible in PATH

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This extension is licensed under the MIT License.

## Changelog

### 1.0.0
- Initial release
- Basic syntax highlighting
- Code completion
- Error checking
- Compilation support
- Module management
- Project initialization