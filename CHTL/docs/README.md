# CHTL (C++ HyperText Language) Documentation

Welcome to the comprehensive documentation for CHTL, a modern hypertext language based on C++ that combines the power of HTML, CSS, and JavaScript with advanced templating and module systems.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Language Reference](#language-reference)
3. [Templates and Custom Elements](#templates-and-custom-elements)
4. [Module System](#module-system)
5. [CHTL JS Extensions](#chtl-js-extensions)
6. [Configuration](#configuration)
7. [CLI Tools](#cli-tools)
8. [VSCode Extension](#vscode-extension)
9. [Examples](#examples)
10. [API Reference](#api-reference)
11. [Contributing](#contributing)

## Getting Started

### Installation

1. Clone the CHTL repository:
```bash
git clone https://github.com/chtl-dev/chtl.git
cd chtl
```

2. Build the project:
```bash
mkdir build && cd build
cmake ..
make
```

3. Install the CLI tool:
```bash
sudo make install
```

### Quick Start

Create your first CHTL file (`hello.chtl`):

```chtl
html
{
    head
    {
        title: "Hello CHTL";
    }
    
    body
    {
        h1: "Welcome to CHTL!";
        p: "This is your first CHTL document.";
    }
}
```

Compile it:
```bash
chtl compile hello.chtl
```

## Language Reference

### Basic Syntax

CHTL uses a C++-inspired syntax with braces `{}` for blocks and semicolons `;` for statements.

#### Comments
```chtl
// Single-line comment
/* Multi-line
   comment */
-- Generator comment (converted to HTML comment)
```

#### Text Nodes
```chtl
text { "Hello, World!" }
text: "Hello, World!"  // CE equivalence
```

#### Elements
```chtl
div
{
    class: "container";
    id: "main";
    
    h1: "Title";
    p: "Content";
}
```

#### Attributes
```chtl
// Using colon (preferred)
class: "my-class";
id: "my-id";

// Using equals (equivalent)
class = "my-class";
id = "my-id";
```

### Advanced Features

#### Local Style Blocks
```chtl
div
{
    class: "box";
    
    style
    {
        width: 100px;
        height: 100px;
        background-color: red;
        
        // Automated class/ID
        .box { border: 1px solid black; }
        #main { margin: 10px; }
        
        // Context inference
        &:hover { background-color: blue; }
        
        // Attribute conditional expressions
        background-color: width > 50px ? "green" : "red";
    }
}
```

#### Local Script Blocks
```chtl
div
{
    class: "interactive";
    
    script
    {
        // Enhanced selectors
        {{.interactive}}->addEventListener('click', () => {
            console.log('Clicked!');
        });
        
        // Arrow operator
        {{#button}}->style.color = 'red';
    }
}
```

## Templates and Custom Elements

### Templates

#### Style Templates
```chtl
[Template] @Style PrimaryButton
{
    background-color: "#007bff";
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    
    &:hover { background-color: "#0056b3"; }
}
```

#### Element Templates
```chtl
[Template] @Element Card
{
    div
    {
        class: "card";
        style
        {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        
        h3: "Card Title";
        p: "Card content";
    }
}
```

#### Variable Templates
```chtl
[Template] @Var ThemeColors
{
    primary: "#007bff";
    secondary: "#6c757d";
    success: "#28a745";
    danger: "#dc3545";
}
```

### Custom Elements

#### Custom Styles
```chtl
[Custom] @Style MyButton
{
    .btn-primary {
        background-color: ThemeColors.primary;
        color: white;
    }
    
    .btn-secondary {
        background-color: ThemeColors.secondary;
        color: white;
    }
}
```

#### Custom Elements
```chtl
[Custom] @Element Modal
{
    div
    {
        class: "modal";
        style: "display: none;";
        
        div
        {
            class: "modal-content";
            
            button
            {
                class: "modal-close";
                text: "×";
            }
            
            h2: "Modal Title";
            p: "Modal content";
        }
    }
}
```

## Module System

### CMOD (CHTL Module)

```chtl
[Info]
{
    name: "MyModule";
    version: "1.0.0";
    description: "A sample CHTL module";
    author: "Developer";
    license: "MIT";
}

[Export]
{
    @Style PrimaryButton;
    @Element Card;
    @Var ThemeColors;
}

[Template] @Style PrimaryButton
{
    // Template definition
}

[Template] @Element Card
{
    // Element definition
}
```

### CJMOD (CHTL JS Module)

```chtl
[Info]
{
    name: "MyJSModule";
    version: "1.0.0";
    description: "A sample CHTL JS module";
}

fileloader
{
    load: "./utils.js", "./components.js";
}

util
{
    logMessage: (message) => {
        console.log(`[${new Date().toISOString()}] ${message}`);
    };
    
    formatDate: (date) => {
        return new Date(date).toLocaleDateString();
    };
}
```

### Hybrid Modules

```chtl
[Info]
{
    name: "HybridModule";
    version: "1.0.0";
    type: "hybrid";
}

// CMOD part
[Template] @Style GlobalStyles
{
    // Styles
}

// CJMOD part
fileloader
{
    load: "./scripts.js";
}

util
{
    // JavaScript utilities
}
```

## CHTL JS Extensions

### File Loading
```chtl
fileloader
{
    load: "./module1.js", "./module2.js";
    // or
    load: "./module1.js";
    load: "./module2.js";
}
```

### Event Handling
```chtl
listen
{
    document: {
        click: (event) => {
            console.log('Document clicked');
        };
    };
    
    ".button": {
        click: (event) => {
            event.preventDefault();
            console.log('Button clicked');
        };
    };
}
```

### Event Delegation
```chtl
delegate
{
    ".container": {
        click: ".button", (event) => {
            console.log('Delegated button click');
        };
    };
}
```

### Animations
```chtl
animate
{
    target: ".box";
    duration: 300;
    easing: "ease-in-out";
    begin: {
        opacity: 0;
        transform: "translateY(20px)";
    };
    end: {
        opacity: 1;
        transform: "translateY(0)";
    };
}
```

### Virtual Objects
```chtl
vir
{
    ModalManager: {
        open: (id) => {
            const modal = document.getElementById(id);
            if (modal) modal.style.display = "flex";
        };
        
        close: (id) => {
            const modal = document.getElementById(id);
            if (modal) modal.style.display = "none";
        };
    };
}
```

### Routing
```chtl
router
{
    mode: "hash";
    routes: {
        "/": "home";
        "/about": "about";
        "/contact": "contact";
    };
    
    onRouteChange: (route) => {
        console.log('Route changed to:', route);
    };
}
```

## Configuration

### Configuration Groups
```chtl
[Configuration] ProjectSettings
{
    projectName: "My CHTL Project";
    version: "1.0.0";
    author: "Developer";
    
    // Code style
    indentSize: 4;
    useTabs: false;
    lineEnding: "lf";
    
    // Features
    enableTemplates: true;
    enableCustoms: true;
    enableImports: true;
    
    // Compiler
    optimizationLevel: "medium";
    debugMode: true;
    warningsAsErrors: false;
}
```

### Keyword Customization
```chtl
[Configuration] MyKeywords
{
    keywords: "myElement", "myStyle", "myScript";
    enabled: true;
    priority: "high";
}
```

## CLI Tools

### Basic Commands

```bash
# Compile a CHTL file
chtl compile input.chtl -o output.html

# Watch for changes
chtl watch input.chtl -o dist/

# Initialize a new project
chtl init my-project -t basic

# Build the entire project
chtl build -c config.json -o dist/

# Clean build artifacts
chtl clean -d dist/

# Run tests
chtl test -p "*.test.chtl"

# Start development server
chtl serve -p 8080 -d src/
```

### Advanced Usage

```bash
# Compile with options
chtl compile input.chtl \
    --output output.html \
    --minify \
    --sourcemap \
    --watch

# Build with configuration
chtl build \
    --config chtl.config.json \
    --output dist/ \
    --minify

# Watch with custom interval
chtl watch input.chtl \
    --output dist/ \
    --interval 500
```

## VSCode Extension

### Features
- Syntax highlighting
- IntelliSense and auto-completion
- Code formatting
- Real-time linting
- Live preview
- Integrated compilation
- Snippet support

### Installation
1. Install from VS Code marketplace
2. Open a `.chtl` file to activate
3. Use `Ctrl+Shift+C` to compile
4. Use `Ctrl+Shift+P` to preview

### Configuration
```json
{
  "chtl.compiler.path": "chtl",
  "chtl.formatting.enabled": true,
  "chtl.formatting.indentSize": 2,
  "chtl.preview.autoOpen": true,
  "chtl.linting.enabled": true
}
```

## Examples

### Complete Website
```chtl
// Complete website example
[Template] @Style GlobalStyles
{
    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
    
    body {
        font-family: Arial, sans-serif;
        line-height: 1.6;
        color: #333;
    }
}

[Template] @Element Header
{
    header
    {
        class: "site-header";
        style
        {
            background-color: "#007bff";
            color: white;
            padding: 1rem 0;
        }
        
        div
        {
            class: "container";
            
            h1: "My Website";
            nav
            {
                ul
                {
                    li { a: "Home"; }
                    li { a: "About"; }
                    li { a: "Contact"; }
                }
            }
        }
    }
}

[Template] @Element Footer
{
    footer
    {
        class: "site-footer";
        style
        {
            background-color: "#6c757d";
            color: white;
            text-align: center;
            padding: 1rem 0;
        }
        
        p: "© 2024 My Website. All rights reserved.";
    }
}

html
{
    head
    {
        title: "My CHTL Website";
        meta: "charset=UTF-8";
        meta: "name=viewport, content=width=device-width, initial-scale=1.0";
        
        style
        {
            @Style GlobalStyles;
        }
    }
    
    body
    {
        @Element Header;
        
        main
        {
            class: "main-content";
            style
            {
                max-width: 1200px;
                margin: 0 auto;
                padding: 2rem;
            }
            
            h2: "Welcome to CHTL";
            p: "This website is built with CHTL, a modern hypertext language.";
            
            div
            {
                class: "features";
                style
                {
                    display: grid;
                    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                    gap: 2rem;
                    margin-top: 2rem;
                }
                
                div
                {
                    class: "feature-card";
                    style
                    {
                        border: 1px solid #ddd;
                        border-radius: 8px;
                        padding: 1.5rem;
                        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    }
                    
                    h3: "Templates";
                    p: "Reusable components for consistent design.";
                }
                
                div
                {
                    class: "feature-card";
                    style
                    {
                        border: 1px solid #ddd;
                        border-radius: 8px;
                        padding: 1.5rem;
                        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    }
                    
                    h3: "Modules";
                    p: "Organize code with powerful module system.";
                }
                
                div
                {
                    class: "feature-card";
                    style
                    {
                        border: 1px solid #ddd;
                        border-radius: 8px;
                        padding: 1.5rem;
                        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    }
                    
                    h3: "JavaScript";
                    p: "Enhanced JavaScript with CHTL JS extensions.";
                }
            }
        }
        
        @Element Footer;
        
        script
        {
            // Enhanced selectors
            {{.feature-card}}->addEventListener('click', () => {
                console.log('Feature card clicked');
            });
            
            // Virtual objects
            vir
            {
                Utils: {
                    showNotification: (message) => {
                        alert(message);
                    };
                };
            };
            
            // Show welcome message
            Utils->showNotification('Welcome to CHTL!');
        }
    }
}
```

## API Reference

### Core Classes

#### CHTLLexer
```cpp
class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    std::vector<Token> tokenize();
};
```

#### CHTLParser
```cpp
class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::shared_ptr<BaseNode> parse();
};
```

#### CHTLGenerator
```cpp
class CHTLGenerator {
public:
    CHTLGenerator();
    std::string generate(std::shared_ptr<BaseNode> ast);
};
```

### Module System

#### CMODModule
```cpp
class CMODModule {
public:
    void setInfo(const CMODInfo& info);
    void addExport(const ExportItem& item);
    std::vector<ExportItem> getAllExports() const;
};
```

#### CJMODModule
```cpp
class CJMODModule {
public:
    void addFunction(const CJMODFunction& func);
    std::vector<CJMODFunction> getAllFunctions() const;
};
```

## Contributing

We welcome contributions to CHTL! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

### Code Style

- Use C++17 features
- Follow the existing code style
- Add comprehensive tests
- Update documentation

## License

CHTL is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Support

- Documentation: [https://chtl.dev/docs](https://chtl.dev/docs)
- Issues: [https://github.com/chtl-dev/chtl/issues](https://github.com/chtl-dev/chtl/issues)
- Discussions: [https://github.com/chtl-dev/chtl/discussions](https://github.com/chtl-dev/chtl/discussions)