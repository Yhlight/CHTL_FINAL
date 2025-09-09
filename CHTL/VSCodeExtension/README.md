# CHTL Language Support for VS Code

This extension provides comprehensive language support for CHTL (C++ HyperText Language) in Visual Studio Code.

## Features

### Syntax Highlighting
- Full syntax highlighting for CHTL files
- Support for HTML elements, CSS properties, and JavaScript code
- Special highlighting for CHTL-specific keywords and constructs

### IntelliSense
- Auto-completion for HTML elements, CHTL keywords, and CSS properties
- Hover information for elements, attributes, and functions
- Go to definition for templates and custom elements
- Find all references for symbols

### Code Formatting
- Automatic code formatting with configurable indentation
- Support for tabs and spaces
- Smart indentation based on CHTL syntax

### Linting
- Real-time syntax checking
- Error detection for invalid keywords and operators
- Warning for indentation issues
- Structure validation

### Preview
- Live preview of compiled CHTL files
- Auto-refresh on document changes
- Integrated webview for immediate feedback

### Compilation
- One-click compilation of CHTL files
- Watch mode for automatic recompilation
- Build and clean project commands
- Integrated output channel for compiler messages

### Snippets
- Comprehensive snippet collection for common CHTL patterns
- Template snippets for quick scaffolding
- Custom element and style snippets
- CHTL JS feature snippets

## Commands

- `CHTL: Compile` - Compile the current CHTL file
- `CHTL: Preview` - Open preview of the current CHTL file
- `CHTL: Format` - Format the current CHTL file
- `CHTL: Watch` - Start watch mode for the current file
- `CHTL: Build Project` - Build the entire CHTL project
- `CHTL: Clean Project` - Clean build artifacts

## Configuration

The extension can be configured through VS Code settings:

```json
{
  "chtl.compiler.path": "chtl",
  "chtl.compiler.options": {
    "minify": false,
    "sourcemap": true,
    "watch": false
  },
  "chtl.formatting.enabled": true,
  "chtl.formatting.indentSize": 2,
  "chtl.formatting.useTabs": false,
  "chtl.preview.autoOpen": true,
  "chtl.preview.port": 8080,
  "chtl.linting.enabled": true,
  "chtl.linting.strict": false
}
```

## Requirements

- CHTL compiler installed and available in PATH
- VS Code 1.74.0 or higher

## Installation

1. Install the CHTL compiler
2. Install this extension from the VS Code marketplace
3. Open a `.chtl` file to activate the extension

## Usage

1. Create a new `.chtl` file
2. Start typing CHTL code with full IntelliSense support
3. Use `Ctrl+Shift+C` to compile
4. Use `Ctrl+Shift+P` to preview
5. Use `Ctrl+Shift+F` to format

## Snippets

The extension includes many useful snippets:

- `chtl-basic` - Basic CHTL HTML structure
- `chtl-template` - CHTL template block
- `chtl-custom` - CHTL custom block
- `chtl-import` - CHTL import statement
- `chtl-namespace` - CHTL namespace block
- `chtl-style` - CHTL style block
- `chtl-script` - CHTL script block
- `chtl-fileloader` - CHTL JS fileloader
- `chtl-listen` - CHTL JS listen block
- `chtl-delegate` - CHTL JS delegate block
- `chtl-animate` - CHTL JS animate block
- `chtl-vir` - CHTL JS virtual object
- `chtl-router` - CHTL JS router

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This extension is licensed under the MIT License.