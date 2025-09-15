# CHTL Language Support for VS Code

This extension provides comprehensive language support for the CHTL template language in Visual Studio Code.

## Features

### 🎨 Syntax Highlighting
- Full syntax highlighting for CHTL files (`.chtl`)
- Full syntax highlighting for CHTL JS files (`.chtljs`, `.cjjs`)
- Support for all CHTL directives and constructs
- Support for HTML, CSS, and JavaScript within CHTL files

### 🔧 Language Features
- **IntelliSense**: Auto-completion for CHTL syntax
- **Hover Information**: Detailed information about CHTL constructs
- **Symbol Navigation**: Navigate to templates, custom elements, and functions
- **Error Detection**: Real-time syntax and semantic error checking
- **Code Actions**: Quick fixes for common issues

### 🚀 Commands
- **Compile CHTL**: Compile current CHTL file to HTML
- **Compile CHTL JS**: Compile current CHTL JS file to JavaScript
- **Preview CHTL**: Preview compiled CHTL output
- **Build Project**: Build entire CHTL project
- **Watch Files**: Watch for changes and auto-compile
- **Clean Build**: Clean build artifacts
- **Initialize Project**: Initialize new CHTL project

### 📝 Code Snippets
- Template snippets for `[Template] @Style`, `@Element`, `@Var`
- Custom element snippets for `[Custom]`
- Import, namespace, and configuration snippets
- HTML, CSS, and JavaScript snippets
- CHTL JS snippets for CJMOD, animations, routes, and more

### 🎯 Keybindings
- `Ctrl+Shift+C`: Compile CHTL file
- `Ctrl+Shift+J`: Compile CHTL JS file
- `Ctrl+Shift+P`: Preview CHTL output
- `Ctrl+Shift+B`: Build CHTL project

## Installation

1. Install the extension from the VS Code marketplace
2. Open a CHTL file (`.chtl` or `.chtljs`)
3. The extension will automatically activate

## Configuration

The extension can be configured through VS Code settings:

```json
{
  "chtl.compilerPath": "./build/working_chtl_compiler",
  "chtl.jsCompilerPath": "./build/chtl_js_compiler",
  "chtl.autoCompile": true,
  "chtl.autoPreview": false,
  "chtl.outputDirectory": "./build",
  "chtl.watchMode": false,
  "chtl.enableLinting": true,
  "chtl.enableIntelliSense": true
}
```

## Usage

### Basic CHTL File
```chtl
[Template] @Style MyButton {
    background-color: #007bff;
    color: white;
    padding: 10px 20px;
}

[Template] @Element ButtonContainer {
    <button class="MyButton">Click Me</button>
}

[Origin] @Html {
    <div class="container">
        <div class="ButtonContainer"></div>
    </div>
}
```

### CHTL JS File
```chtljs
[CJMOD] printMylove {
    function printMylove() {
        console.log("I love CHTL!");
    }
}

[Animation] fadeIn {
    duration: 1s
    easing: ease-in-out
    keyframes: 0%: {opacity: 0}, 100%: {opacity: 1}
}

$userName$ = "CHTL User"
const button = {{.my-button}}
button -> click: handleClick
```

## Commands

### Compile Commands
- `CHTL: Compile CHTL` - Compile current CHTL file
- `CHTL: Compile CHTL JS` - Compile current CHTL JS file
- `CHTL: Preview CHTL` - Preview compiled output

### Project Commands
- `CHTL: Build CHTL Project` - Build entire project
- `CHTL: Watch CHTL Files` - Watch for changes
- `CHTL: Clean CHTL Build` - Clean build artifacts
- `CHTL: Initialize CHTL Project` - Initialize new project

## Features in Detail

### Syntax Highlighting
- **Directives**: `[Template]`, `[Custom]`, `[Import]`, etc.
- **Types**: `@Style`, `@Element`, `@Var`, `Style`, `Element`, `Var`
- **Keywords**: `inherit`, `namespace`, `@Html`, `@Style`, `@JavaScript`
- **Variables**: `$variable$`, `{{selector}}`
- **Events**: `element -> event: handler`

### IntelliSense
- Auto-completion for all CHTL constructs
- Parameter hints for functions
- Quick info on hover
- Symbol navigation

### Error Detection
- Syntax error detection
- Semantic error checking
- Real-time validation
- Quick fixes for common issues

### Code Snippets
- Template snippets with placeholders
- Custom element snippets
- Import and configuration snippets
- HTML, CSS, and JavaScript snippets

## Requirements

- VS Code 1.74.0 or higher
- CHTL compiler (configured in settings)

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This extension is licensed under the MIT License.

## Changelog

### 1.0.0
- Initial release
- Full CHTL syntax support
- CHTL JS syntax support
- IntelliSense and error detection
- Code snippets and commands
- Project management features