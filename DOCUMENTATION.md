# CHTL Language Documentation

Welcome to CHTL, a developer-friendly hypertext language that compiles to HTML and CSS.

## Basic Syntax

### Elements
Create HTML elements using a simple, clean syntax. Nest elements by placing them inside the curly braces `{}` of their parent.

```chtl
html {
    body {
        div { }
        span { }
    }
}
```

### Text
Add text to an element using the `text` keyword.

```chtl
p {
    text { "This is a paragraph." }
}
```
You can also use unquoted strings for simple text.
```chtl
p {
    text { This is also a paragraph. }
}
```

### Attributes
Assign standard HTML attributes using the `=` operator. End each attribute with a semicolon `;`.

```chtl
div {
    id = "main-container";
    class = "container dark-theme";
}
```

### Comments
-   `// ...`: A single-line comment that will be ignored by the compiler.
-   `-- ...`: A generator comment. This will be converted to a standard HTML comment `<!-- ... -->` in the final output.

```chtl
// This comment will not appear in the HTML.
body {
    -- This comment will appear in the HTML.
}
```

## Styling

In CHTL, styles are applied directly as properties on an element, using the `:` operator. This provides a powerful and co-located way to style your components.

```chtl
div {
    id = "box";
    // Style properties
    color: red;
    width: 200px;
    height: 100px;
}
```

### Attribute Arithmetic
You can perform math directly in your style values. CHTL supports `+`, `-`, `*`, `/`, `%` (modulo), and `**` (power). For complex calculations, the output will be wrapped in CSS's `calc()` function.

```chtl
div {
    padding: 10 * 2; // will compile to padding: 20;
    width: 100% - 20px; // will compile to width: calc(100% - 20px);
}
```

### Attribute Referencing
Reference the value of another property from a different element using its selector (`#id` or `.class`) and a dot `.`.

```chtl
div {
    id = "sidebar";
    width: 300px;
}

div {
    id = "main-content";
    // This div's width will be calculated based on the sidebar's width
    width: 100% - #sidebar.width;
}
```

### Conditional Expressions
Set property values conditionally using a ternary operator (`? :`). The condition can reference other properties of the same element.

```chtl
div {
    width: 250;
    // The background-color will be green because width > 200
    background-color: width > 200 ? "green" : "red";
}
```

## Advanced Features

### Raw Code Embedding (`[Origin]`)
To include a block of raw code that should not be processed by CHTL, use an `[Origin]` block. This is useful for embedding complex HTML, inline SVG, or scripts.

```chtl
body {
    h1 { text { "My Page" } }

    [Origin] @Html {
        <svg width="100" height="100">
            <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
        </svg>
    }
}
```

### Namespaces (`[Namespace]`)
Organize your code and prevent naming conflicts by wrapping it in a `[Namespace]` block. This becomes especially important when using the `[Import]` feature.

```chtl
[Namespace] MyComponents {
    div {
        class = "card";
        // ...
    }
}
```

## Command-Line Interface (CLI)

You can compile your `.chtl` files using the `chtl` command-line tool.

**Usage:**
```bash
chtl <inputfile> [-o <outputfile>]
```

-   `<inputfile>`: The path to your `.chtl` file.
-   `-o <outputfile>`: (Optional) The path to write the output HTML file. If omitted, the HTML will be printed to the standard output.

**Example:**
```bash
# Compile my_page.chtl and print to console
chtl my_page.chtl

# Compile my_page.chtl and save to public/index.html
chtl my_page.chtl -o public/index.html
```
