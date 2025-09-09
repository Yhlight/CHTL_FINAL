# CHTL - A Hypertext Language Based on Rust

CHTL is a hypertext language that provides a more developer-friendly way to write HTML code, using MIT open source license.

## Features

- **Modern Syntax**: Clean, readable syntax similar to CSS but for HTML structure
- **Local Style Blocks**: Inline CSS styling with automatic class/ID generation
- **Template System**: Reusable components with style groups, element templates, and variable groups
- **Custom System**: Advanced customization with specialization and inheritance
- **Import System**: Module support for HTML, CSS, JS, and CHTL files
- **Namespace Support**: Prevent module pollution with namespaces
- **CHTL JS Extension**: JavaScript enhancements with virtual objects, routing, and more
- **Configuration**: Customizable keywords and behavior

## Installation

### Prerequisites

- Rust 1.70+ (install from [rustup.rs](https://rustup.rs/))
- Cargo (comes with Rust)

### Build from Source

```bash
# Clone the repository
git clone <repository-url>
cd chtl

# Build the project
cargo build --release

# Install globally
cargo install --path .
```

## Quick Start

### 1. Initialize a New Project

```bash
chtl init my-project
cd my-project
```

### 2. Write CHTL Code

Create a file `src/main.chtl`:

```chtl
html
{
    head
    {
        title
        {
            text: "Hello CHTL";
        }
    }
    
    body
    {
        h1
        {
            text: "Welcome to CHTL!";
        }
        
        div
        {
            class: "container";
            style
            {
                width: 100%;
                height: 200px;
                background-color: #f0f0f0;
            }
            
            text
            {
                This is a styled container
            }
        }
    }
}
```

### 3. Compile to HTML

```bash
chtl compile src/main.chtl -o dist/index.html
```

### 4. Start Development Server

```bash
chtl serve
```

## Syntax Examples

### Basic Elements

```chtl
div
{
    id: "main";
    class: "container";
    
    text
    {
        Hello World
    }
}
```

### Local Style Blocks

```chtl
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: red;
        
        .box  // Auto-generates class="box"
        {
            border: 1px solid black;
        }
        
        &:hover  // References the element
        {
            background-color: blue;
        }
    }
}
```

### Templates

```chtl
[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    background-color: #007bff;
    color: white;
    cursor: pointer;
}

[Template] @Element Button
{
    button
    {
        style
        {
            @Style ButtonStyle;
        }
        
        text
        {
            Click me
        }
    }
}
```

### Custom Components

```chtl
[Custom] @Element Card
{
    div
    {
        class: "card";
        style
        {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 16px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
    }
}

// Usage
body
{
    @Element Card
    {
        // Customize the card
        div
        {
            style
            {
                background-color: #f9f9f9;
            }
        }
    }
}
```

### CHTL JS Features

```chtl
div
{
    id: "app";
    
    script
    {
        // Virtual objects
        vir buttonHandler = listen {
            click: () => {
                console.log("Button clicked!");
            }
        };
        
        // Enhanced selectors
        {{#app}}->addEventListener('load', () => {
            console.log("App loaded");
        });
        
        // Animation
        const anim = animate {
            target: {{#app}},
            duration: 1000,
            begin: {
                opacity: 0
            },
            end: {
                opacity: 1
            }
        };
    }
}
```

## Project Structure

```
my-project/
├── src/
│   ├── main.chtl          # Main CHTL file
│   ├── components/        # Reusable components
│   └── styles/           # Global styles
├── dist/                 # Compiled output
├── chtl.config.chtl      # Configuration
└── package.json          # Node.js integration
```

## Configuration

Create a `chtl.config.chtl` file:

```chtl
[Configuration]
{
    HTML5_DOCTYPE = true;
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
    
    [Name]
    {
        CUSTOM_STYLE = [@Style, @style, @CSS];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        // ... more customizations
    }
}
```

## CLI Commands

- `chtl init <directory>` - Initialize a new project
- `chtl compile <input> -o <output>` - Compile CHTL to HTML
- `chtl build` - Build the entire project
- `chtl serve` - Start development server
- `chtl format <input>` - Format CHTL code
- `chtl lint <input>` - Lint CHTL code
- `chtl doc <input> -o <output>` - Generate documentation

## Advanced Features

### Modules and Imports

```chtl
[Import] @Chtl from "./components/Button.chtl";
[Import] @Style from "./styles/theme.css";
[Import] @JavaScript from "./scripts/app.js";
```

### Namespaces

```chtl
[Namespace] ui
{
    [Custom] @Element Button
    {
        // Button implementation
    }
}

// Usage
body
{
    @Element Button from ui;
}
```

### Constraints

```chtl
div
{
    except span, [Custom] @Element Box;  // Prohibit certain elements
}
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## License

MIT License - see LICENSE file for details.

## Roadmap

- [ ] VSCode extension with syntax highlighting
- [ ] Advanced CSS features (animations, transitions)
- [ ] TypeScript integration
- [ ] Hot reload in development
- [ ] Plugin system
- [ ] Performance optimizations
- [ ] Documentation generator
- [ ] Testing framework

## Examples

See the `examples/` directory for more examples:

- `hello.chtl` - Basic CHTL syntax
- `template.chtl` - Template system usage
- `custom.chtl` - Custom components
- `chtl-js.chtl` - CHTL JS features

## Support

- GitHub Issues: Report bugs and request features
- Documentation: [Link to docs]
- Community: [Link to community]

---

**CHTL** - Making HTML development more enjoyable! 🚀