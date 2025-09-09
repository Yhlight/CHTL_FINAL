# CHTL - A Hypertext Language

CHTL is a hypertext language that provides a more developer-friendly way to write HTML code. This implementation is based on C++ as specified in the original document.

## Features

### Core CHTL Features
- **Text Nodes**: Use `text { }` to represent text content
- **Element Nodes**: Support for all HTML elements
- **Attributes**: Use `attribute: value;` syntax
- **Comments**: Support for `//`, `/* */`, and `--` comments
- **Literals**: Support for unquoted strings and various quote types
- **CE Equality**: `:` and `=` are completely equivalent

### Style System
- **Local Style Blocks**: Enhanced `<style>` blocks with nested styling
- **Auto Class/ID**: Automatic class and ID generation
- **Context Derivation**: Use `&` for pseudo-selectors
- **Property Condition Expressions**: Dynamic CSS based on element properties
- **Chain Expressions**: Multiple conditional expressions
- **Logic Operators**: Support for `&&` and `||`

### Template System
- **Style Templates**: Reusable style groups
- **Element Templates**: Reusable element structures
- **Variable Templates**: Reusable variable groups
- **Inheritance**: Template inheritance with `inherit` keyword

### Custom System
- **Custom Styles**: Custom style groups with specialization
- **Custom Elements**: Custom elements with modification capabilities
- **Custom Variables**: Custom variable groups
- **Specialization**: Add, delete, and modify inherited properties

### Advanced Features
- **Origin Embedding**: Raw HTML, CSS, and JS embedding
- **Import System**: Import external files and modules
- **Namespace System**: Prevent module pollution
- **Constraint System**: Restrict usage with `except` keyword
- **Configuration**: Customizable compiler behavior

### CHTL JS Features
- **File Loading**: AMD-style module loading
- **Local Scripts**: Enhanced script blocks
- **Enhanced Selectors**: DOM selection with `{{selector}}`
- **Event Delegation**: Stable event binding
- **Animations**: Simplified animation system
- **Virtual Objects**: Function overloading with `vir`
- **Routing**: SPA routing system

## Project Structure

```
CHTL/
├── main.cpp                           # CLI entry point
├── CMakeLists.txt                     # CMake build configuration
├── CHTLCompiler/                      # CHTL compiler
│   ├── CHTLContext/                   # Compilation context
│   │   ├── CHTLContext.h
│   │   └── CHTLContext.cpp
│   ├── CHTLLexer/                     # Lexical analysis
│   │   ├── CHTLLexer.h
│   │   └── CHTLLexer.cpp
│   ├── CHTLParser/                    # Syntax analysis (TODO)
│   ├── CHTLGenerator/                 # Code generation (TODO)
│   ├── CHTLNode/                      # AST nodes (TODO)
│   ├── CHTLState/                     # Compilation state (TODO)
│   ├── CHTLLoader/                    # File loading (TODO)
│   ├── CHTLManage/                    # Resource management (TODO)
│   ├── CHTLIOStream/                  # I/O operations (TODO)
│   └── CMODSystem/                    # CMOD module system (TODO)
├── CHTLJS/                            # CHTL JS compiler (TODO)
├── Scanner/                           # Unified scanner (TODO)
├── CompilerDispatcher/                # Compiler coordination (TODO)
├── CodeMerger/                        # Code merging (TODO)
├── CSS/                               # CSS compiler (TODO)
├── JS/                                # JavaScript compiler (TODO)
├── Util/                              # Utility functions (TODO)
├── Test/                              # Test functions (TODO)
├── test.chtl                          # Test file (Chinese)
├── test_en.chtl                       # Test file (English)
└── simple.chtl                        # Simple test file
```

## Current Implementation Status

### ✅ Completed
- **Project Structure**: Complete directory structure as per specification
- **CHTL Lexer**: Full lexical analysis with support for:
  - All CHTL keywords (text, style, script, Template, Custom, etc.)
  - Operators (:, =, ;, ,, ., /, @, #, !, ?, &, etc.)
  - Brackets ({}, [], ())
  - Comparison operators (>, <, >=, <=, ==, !=)
  - Logical operators (&&, ||)
  - Identifiers, string literals, number literals
  - Comments (//, /* */, --)
  - Whitespace handling
- **CHTL Parser**: Complete syntax analysis and AST generation with support for:
  - Text nodes with proper HTML escaping
  - Element nodes with attributes and content
  - Style blocks with CSS rule parsing
  - Template definitions ([Template] @Style syntax)
  - Custom definitions ([Custom] @Element syntax)
  - Element references (@Element, @Style)
  - Nested content parsing
  - Error handling and recovery
- **CHTL Nodes**: Complete AST node definitions for all CHTL constructs
- **CHTL Context**: Basic compilation context management
- **CLI Tool**: Command-line interface with help system
- **Build System**: CMake configuration for C++ compilation

### 🚧 In Progress
- **CHTL Generator**: Enhanced HTML code generation with proper template/custom resolution

### 📋 Pending
- **Style System**: Local style blocks, auto class/ID, context derivation
- **Template System**: Style, element, and variable templates
- **Custom System**: Custom styles, elements, and variables
- **Origin Embedding**: Raw HTML, CSS, and JS embedding
- **Import System**: File and module importing
- **Namespace System**: Module pollution prevention
- **Constraint System**: Usage restrictions
- **Configuration System**: Compiler customization
- **CHTL JS**: Complete CHTL JS implementation
- **Module System**: CMOD and CJMOD support
- **Unified Scanner**: Code fragment separation
- **Compiler Dispatcher**: Compiler coordination
- **Code Merger**: Fragment merging
- **Utilities**: File system, zip, string utilities
- **Testing**: Comprehensive test suite
- **VSCode Extension**: IDE support

## Usage

### Building
```bash
cd CHTL
mkdir build && cd build
cmake ..
make
```

### Basic Usage
```bash
# Compile CHTL file to HTML
./chtl input.chtl output.html

# Show help
./chtl --help
```

### Example CHTL Code

```chtl
// Basic text node
text
{
    "Hello, World!"
}

// Element with attributes
div
{
    id: main;
    class: container;
    
    text
    {
        "Content here"
    }
    
    // Local style block
    style
    {
        .container
        {
            width: 100px;
            height: 200px;
            background-color: red;
        }
        
        &:hover
        {
            background-color: blue;
        }
    }
}

// Template definition
[Template] @Style DefaultText
{
    color: "black";
    font-size: 16px;
    line-height: 1.6;
}

// Custom element
[Custom] @Element Box
{
    div
    {
        style
    {
            width: 200px;
            height: 200px;
            background-color: green;
        }
    }
}
```

## Testing

The project includes several test files:
- `simple.chtl`: Basic CHTL syntax test
- `test_en.chtl`: Comprehensive English test file
- `test.chtl`: Chinese test file (UTF-8 encoded)

## Development Notes

This implementation follows the CHTL specification document closely. The current focus is on completing the core compilation pipeline:

1. ✅ Lexical Analysis (CHTLLexer)
2. 🚧 Syntax Analysis (CHTLParser) - Next priority
3. 📋 Code Generation (CHTLGenerator)
4. 📋 Advanced Features (Templates, Custom, etc.)

## Contributing

This project is based on the CHTL specification document. Contributions are welcome to implement the remaining features according to the specification.

## License

MIT License - see LICENSE file for details.

## Acknowledgments

Based on the CHTL specification document by the CHTL Team.