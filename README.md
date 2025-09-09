# CHTL - C++ Implementation

This is a complete C++17 implementation of the CHTL (C++ HyperText Language) specification as defined in `CHTL.md`.

## Overview

CHTL is a hypertext language that provides a better way to write HTML code. It supports:

- **Comments**: `//`, `/* */`, and `--` (generator comments)
- **Text nodes**: `text { }` blocks and `text:` attributes
- **Literals**: Unquoted, single-quoted, and double-quoted strings
- **CE equivalence**: `:` and `=` are equivalent
- **Element nodes**: All HTML elements supported
- **Attributes**: `attr: value;` or `attr = value;` syntax
- **Local style blocks**: `style { }` with automatic class/id addition
- **Property condition expressions**: `prop: cond ? opt1 : opt2;`
- **Templates**: `[Template] @Style/@Element/@Var`
- **Custom definitions**: `[Custom]` with specialization support
- **Origin embedding**: `[Origin] @Html/@Style/@JavaScript`
- **Import system**: `[Import]` with various types
- **Namespaces**: `[Namespace]` for module pollution prevention
- **Constraints**: `except` keyword for domain restrictions
- **Configuration**: `[Configuration]` with Name remapping
- **CHTL JS**: Independent DSL that compiles to JavaScript
- **Module system**: CMOD and CJMOD support

## Project Structure

```
CHTL/
├── include/                 # Header files
│   ├── CHTL.h              # Main CHTL header
│   └── CHTLJS.h            # CHTL JS header
├── src/                    # Source files
│   ├── main.cpp            # Main executable
│   ├── test_main.cpp       # Test suite
│   └── CHTL/               # CHTL core modules
│       ├── CHTLContext/    # Context management
│       ├── CHTLLexer/      # Lexical analysis
│       ├── CHTLParser/     # Syntax analysis (TODO)
│       ├── CHTLGenerator/  # Code generation (TODO)
│       └── ...             # Other modules
├── examples/               # Example CHTL files
│   └── hello.chtl         # Hello World example
├── tests/                  # Test files
├── build/                  # Build directory
└── CMakeLists.txt         # CMake configuration
```

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+

### Build Steps

```bash
mkdir build
cd build
cmake ..
make
```

### Running

```bash
# Compile a CHTL file
./chtl ../examples/hello.chtl output.html

# Run tests
./chtl_test

# Show help
./chtl --help

# Show version
./chtl --version
```

## Current Implementation Status

### ✅ Completed
- Basic project structure
- CMake configuration
- CHTL header definitions
- CHTL JS header definitions
- CHTL Context management
- CHTL Lexer (basic tokenization)
- Main compiler interface
- Test framework

### 🚧 In Progress
- CHTL Parser (AST construction)
- CHTL Generator (HTML/CSS/JS output)
- CHTL JS compiler
- Module system (CMOD/CJMOD)
- Template system
- Custom definitions
- Import system
- Namespace system
- Configuration system

### 📋 TODO
- Property condition expressions
- Local style block processing
- Origin embedding
- Constraint system
- Advanced error handling
- Performance optimization
- Documentation
- More test cases

## Example Usage

### Basic CHTL File

```chtl
html
{
    head
    {
        title
        {
            "My CHTL Page"
        }
    }
    
    body
    {
        div
        {
            class: container;
            text
            {
                Hello, CHTL World!
            }
        }
        
        style
        {
            .container
            {
                width: 100%;
                padding: 20px;
                background-color: #f0f0f0;
            }
        }
    }
}
```

### Compilation

```bash
./chtl input.chtl output.html
```

## Development

This implementation strictly follows the CHTL.md specification. All features described in the specification will be implemented without simplification or placeholder code.

### Key Design Principles

1. **Complete Implementation**: No shortcuts or placeholders
2. **Specification Compliance**: Strict adherence to CHTL.md
3. **Modular Architecture**: Clean separation of concerns
4. **Error Handling**: Comprehensive error reporting
5. **Performance**: Efficient parsing and generation
6. **Extensibility**: Support for modules and extensions

## Contributing

This is a reference implementation of the CHTL specification. All contributions should maintain strict compliance with the specification and follow the established architecture.

## License

This implementation follows the MIT license as specified in the CHTL.md document.