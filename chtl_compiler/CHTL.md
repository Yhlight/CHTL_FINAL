# CHTL Language Specification

CHTL (Component-based Hyper-Text Language) is a markup language designed for building modern user interfaces with a focus on componentization, reusability, and clean syntax. It compiles directly to HTML, CSS, and JavaScript.

## Table of Contents

1.  [Core Concepts](#core-concepts)
2.  [Syntax](#syntax)
    -   [Elements](#elements)
    -   [Attributes](#attributes)
    -   [Text Nodes](#text-nodes)
    -   [Comments](#comments)
3.  [Templates](#templates)
    -   [`[Template]` Blocks](#template-blocks)
    -   [`[Custom]` Blocks](#custom-blocks)
    -   [Element Templates (`@Element`)](#element-templates-element)
    -   [Style Templates (`@Style`)](#style-templates-style)
    -   [Variable Templates (`@Var`)](#variable-templates-var)
4.  [Element Specialization](#element-specialization)
    -   [Modify (`modify`)](#modify-modify)
    -   [Insert (`insert`)](#insert-insert)
    -   [Delete (`delete`)](#delete-delete)
5.  [Styling](#styling)
    -   [The `style {}` Block](#the-style--block)
    -   [Rule Hoisting](#rule-hoisting)
    -   [The `&` Contextual Selector](#the--contextual-selector)
6.  [Scripting](#scripting)
    -   [The `script {}` Block](#the-script--block)
    -   [CHTL-JS Syntax (`{{...}}`)](#chtl-js-syntax-)
7.  [Modules](#modules)
    -   [The `use` Statement](#the-use-statement)

---

*This document is a living specification based on the current implementation of the CHTL compiler.*

## Core Concepts

CHTL is built around the idea of **components**. A component is a self-contained, reusable piece of UI, defined as a template. These templates can be composed, customized, and reused throughout an application.

The syntax is designed to be minimal and declarative, using curly braces `{}` to denote scope and nesting, similar to CSS or Rust.

## Syntax

### Elements

HTML elements are created using their tag name followed by a set of curly braces. Child elements are nested within these braces.

```chtl
div {
    p {
        // A paragraph inside a div
    }
    span {
        // A span inside a div
    }
}
```

## Element Specialization

When using an `@Element` template, you can customize or "specialize" its contents for a specific instance. This is done by providing a block of rules when you use the template.

Given the following template:

```chtl
[Custom] @Element PageLayout {
    div class: "page" {
        header {
            h1 { text { "Default Title" } }
        }
        main {
            p { text { "Default content." } }
        }
        footer {
            p { text { "Copyright 2025" } }
        }
    }
}
```

### Modify (`modify`)

The `modify` specialization is not yet fully implemented in the parser and generator. (This is a note for the developer - the syntax exists in the grammar but the end-to-end functionality is not fully wired up).

A hypothetical syntax would look like this:
```chtl
// Hypothetical usage
@Element PageLayout {
    header {
        h1 { text { "My Custom Page" } }
    }
}
```

### Insert (`insert`)

The `insert` rule allows you to add new elements relative to the existing elements within the template.

**Syntax:** `insert [before|after|replace|at top|at bottom] <selector> { ... new elements ... }`

```chtl
@Element PageLayout {
    // Insert a subtitle after the h1 in the header
    insert after h1 {
        h2 { text { "A new subtitle" } }
    }

    // Add a new section to the bottom of the main content area
    insert at bottom main {
        hr {}
        p { text { "Another paragraph." } }
    }
}
```

### Delete (`delete`)

The `delete` rule allows you to remove elements from the template instance.

**Syntax:** `delete <selector>;`

The selector can be a simple tag name, which will delete the first matching element.

```chtl
// Use the layout but remove the footer
@Element PageLayout {
    delete footer;
}
```

## Modules

CHTL supports splitting your code across multiple files using a simple module system. This allows you to organize components, templates, and variables into logical, reusable units.

### The `use` Statement

To import definitions from another file, use the `use` statement at the top level of your file. The path is relative to the file containing the `use` statement.

**Syntax:** `use "<path>";`

By convention, imported files containing reusable components have a `.cmod` (CHTL Module) extension, but this is not enforced by the compiler.

**Example:**

`components.cmod`:
```chtl
[Custom] @Element MyButton {
    button class: "btn" {
        text { "Click Me" }
    }
}
```

`main.chtl`:
```chtl
// Import the definitions from components.cmod
use "./components.cmod";

// Now we can use the MyButton component
div {
    @Element MyButton {}
}
```

## Styling

CHTL provides a powerful `style {}` block that can be placed inside any element to define its CSS styles.

### The `style {}` Block

You can define standard CSS properties, use `@Style` and `@Var` templates, and define full CSS rulesets.

```chtl
div {
    style {
        // Standard properties
        width: "100%";
        height: "50px";

        // Use a style template
        @Style FlexCenter;

        // Define a CSS ruleset for a child element
        .child-div {
            color: Var.Colors.primary;
        }
    }
    div class: "child-div" {}
}
```

### Rule Hoisting

Any full CSS ruleset (like `.child-div { ... }` above) defined inside a `style {}` block is automatically "hoisted" into a single `<style>` tag in the `<head>` of the final HTML document. This keeps your CSS organized with your components while ensuring correct and efficient styling.

### The `&` Contextual Selector

Within a hoisted CSS ruleset, you can use the `&` character as a placeholder for the parent element's selector. The compiler will automatically generate a unique class for the parent element and replace `&` with that class name.

```chtl
div { // This div will get a unique class, e.g., "chtl-unique-123"
    style {
        & {
            border: "1px solid black";
        }
        &:hover {
            background-color: "#eee";
        }
    }
}
```

This would compile to CSS like:

```css
.chtl-unique-123 { border: 1px solid black; }
.chtl-unique-123:hover { background-color: #eee; }
```

## Scripting

CHTL allows for simple client-side scripting using a special `script {}` block.

### The `script {}` Block

All content inside `script {}` blocks is collected and placed into a single `<script>` tag at the end of the `<body>` in the final HTML document.

### CHTL-JS Syntax (`{{...}}`)

Inside a `script {}` block, you can use a special double-curly-brace syntax `{{...}}` as a shorthand for `document.querySelector(...)`. This provides a convenient way to select elements for manipulation.

```chtl
div id: "app" {}

script {
    const myApp = {{#app}}; // Compiles to: const myApp = document.querySelector("#app");
    myApp.textContent = "Hello from CHTL-JS!";
}
```

This compiles to:

```html
<div>
    <p>
        <!-- A paragraph inside a div -->
    </p>
    <span>
        <!-- A span inside a div -->
    </span>
</div>
```

### Attributes

Attributes are specified directly after the element's tag name, before the opening brace. They use a `key: value` syntax. The value must be a string literal enclosed in single or double quotes.

```chtl
div id: "main-container" class: 'content' {
    // ...
}
```

This compiles to:

```html
<div id="main-container" class="content">
    <!-- ... -->
</div>
```

### Text Nodes

To include literal text within an element, use the special `text {}` node. The content inside the braces is treated as a raw string.

```chtl
p {
    text { "Hello, World!" }
}
```

This compiles to:

```html
<p>Hello, World!</p>
```

### Comments

CHTL supports both C-style block comments (`/* ... */`) and line comments (`// ...`). Comments are ignored by the compiler and do not appear in the final output.

```chtl
// This is a line comment.
div {
    /*
        This is a
        multi-line
        block comment.
    */
}
```

## Templates

Templates are the core of CHTL's component system. They allow you to define reusable chunks of markup, styles, or variables.

### `[Template]` vs. `[Custom]` Blocks

There are two kinds of definition blocks:

*   `[Template]`: Defines a standard, reusable template. These are intended to be generic and foundational.
*   `[Custom]`: Defines a user-specific or application-specific template. These often build upon or specialize the base templates.

Functionally, they behave similarly in the current version of the compiler, but the distinction is made for clarity and potential future tooling.

### Element Templates (`@Element`)

Element templates allow you to define reusable component structures.

```chtl
[Template] @Element Card {
    div class: "card" {
        div class: "card-header" {
            // Header content goes here
        }
        div class: "card-body" {
            // Body content goes here
        }
    }
}
```

To use the template, you reference it with the `@Element` keyword followed by its name.

```chtl
// Use the Card template
@Element Card {}
```

### Style Templates (`@Style`)

Style templates allow you to define reusable groups of CSS properties.

```chtl
[Template] @Style FlexCenter {
    display: "flex";
    justify-content: "center";
    align-items: "center";
}
```

These are used inside a `style {}` block:

```chtl
div {
    style {
        @Style FlexCenter;
        flex-direction: "column"; // Add more properties
    }
}
```

### Variable Templates (`@Var`)

Variable templates allow you to group related constant values, such as colors or sizes.

```chtl
[Template] @Var Colors {
    primary: "#007bff";
    secondary: "#6c757d";
}
```

These are used within style properties by referencing `Var.GroupName.key`:

```chtl
div {
    style {
        background-color: Var.Colors.primary;
    }
}
```
