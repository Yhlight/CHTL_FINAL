# CHTL Project Analysis

This document breaks down the features and components of the CHTL project as described in `CHTL.md`. It will be used as a reference and checklist during implementation.

## 1. Core CHTL Language Features

### 1.1. Syntax Basics
- [ ] Comments: `//` (single-line), `/**/` (multi-line).
- [ ] Generator Comments: `--` (processed by the generator).
- [ ] Text Nodes: `text { "content" }` or `text: "content"`.
- [ ] Literals: Unquoted, single-quoted (`''`), double-quoted (`""`).
- [ ] CE Equality: `:` is equivalent to `=`.
- [ ] Element Nodes: Standard HTML tags in a nested block structure.
- [ ] Attributes: `attribute_name: "value";`.

### 1.2. Local Style Blocks (`style {}`)
- [ ] Inline Styles: CSS properties directly inside `style {}`.
- [ ] Automatic Class/ID: Automatically add `class` or `id` to the element when selectors like `.class` or `#id` are used.
- [ ] Context `&` Selector: `&` refers to the current element's class/id.
- [ ] Attribute Conditional Expressions: `property: condition ? value_if_true : value_if_false;`.
    - [ ] Chaining with commas.
    - [ ] Optional `else` part.
    - [ ] Logical operators `&&` and `||`.
    - [ ] Cross-element attribute access using CSS-like selectors (`.box.width`).

### 1.3. Reusability Constructs

#### 1.3.1. Templates (`[Template]`)
- [ ] Style Group Template: `[Template] @Style TemplateName { ... }`
- [ ] Element Template: `[Template] @Element TemplateName { ... }`
- [ ] Variable Group Template: `[Template] @Var TemplateName { ... }`
- [ ] Usage: `@Style TemplateName;`, `@Element TemplateName;`, `TemplateName(variable)`.
- [ ] Inheritance: Implicitly by inclusion or explicitly with `inherit`.

#### 1.3.2. Customizations (`[Custom]`)
- [ ] Custom Style Group: `[Custom] @Style Name { ... }`
    - [ ] Valueless properties (to be filled upon use).
    - [ ] Specialization: Deleting properties (`delete prop;`), deleting inheritance (`delete @Style TemplateName;`).
- [ ] Custom Element: `[Custom] @Element Name { ... }`
    - [ ] Specialization:
        - Adding styles to child elements.
        - Index-based access `div[1]`.
        - Inserting elements (`insert after div[0] { ... }`).
        - Deleting elements (`delete span;`).
        - Deleting inheritance (`delete @Element TemplateName;`).
- [ ] Custom Variable Group: `[Custom] @Var Name { ... }`
    - [ ] Specialization on use: `ThemeColor(tableColor = rgb(...))`.
- [ ] Full Name Syntax: `[Custom] @Element Box;` for explicit access.

### 1.4. Interoperability and Configuration

- [ ] Original Code Embedding: `[Origin] @Html/@Style/@JavaScript { ... }`.
    - [ ] Named origin blocks.
    - [ ] Custom origin types (`@Vue`).
- [ ] Imports: `[Import] ... from ... as ...`
    - [ ] Import HTML/CSS/JS as named origin blocks.
    - [ ] Import CHTL parts (elements, styles, etc.) from other `.chtl` files (precise, type-based, wildcard).
    - [ ] Path aliasing: `.` is equivalent to `/`.
- [ ] Namespaces: `[Namespace] name { ... }`.
    - [ ] Nested namespaces.
    - [ ] Access via `from space.room`.
- [ ] Constraints: `except keyword;`.
    - [ ] Precise (`except span;`).
    - [ ] Type-based (`except @Html;`).
    - [ ] Global (within a namespace).
- [ ] Configuration: `[Configuration] { ... }`.
    - [ ] System settings (`INDEX_INITIAL_COUNT`).
    - [ ] Keyword renaming (`[Name] { ... }`).
    - [ ] Named configuration groups.
    - [ ] Importing configurations.
- [ ] `use` statement: `use html5;`, `use @Config Basic;`.

## 2. CHTL JS Features

- [ ] File Suffix: `.cjjs`.
- [ ] File Loader: `fileloader { load: ... }`.
- [ ] Local Script Blocks: `script {}` inside elements.
- [ ] Enhanced Selectors: `{{css-selector}}` to get DOM objects.
- [ ] `->` Operator: Equivalent to `.`.
- [ ] Enhanced Listeners: `{{selector}}->listen { event: callback, ... }`.
- [ ] Event Delegation: `{{parent}}->delegate { target: ..., event: callback, ... }`.
- [ ] Animation: `animate { target: ..., duration: ..., ... }`.
- [ ] Virtual Objects: `vir test = ...`.
- [ ] Routing: `router { url: ..., page: ... }`.
- [ ] Dynamic Attribute Conditional Expressions: `width: {{box}}->width > 2 ? 100px : 50px;`.

## 3. Module System

### 3.1. CMOD (CHTL Modules)
- [ ] Packaged as `.cmod` files.
- [ ] Strict directory structure: `ModuleName/src`, `ModuleName/info`.
- [ ] `info` file contains metadata (`[Info]`) and an optional export table (`[Export]`).
- [ ] Sub-module support.

### 3.2. CJMOD (CHTL JS Modules)
- [ ] C++ based extensions for CHTL JS.
- [ ] Strict directory structure.
- [ ] `info` file with metadata.

### 3.3. Hybrid Modules (CMOD + CJMOD)
- [ ] Combined structure.
- [ ] Separate `info` files for CMOD and CJMOD parts.

### 3.4. Module Resolution
- [ ] Search paths: Official directory, local `module` directory, current directory.
- [ ] Official module prefix: `chtl::`.

## 4. Compiler Architecture

- [ ] **CHTLUnifiedScanner**: Splits source code into CHTL, CHTL JS, CSS, and JS fragments.
- [ ] **CompilerDispatcher**: Routes fragments to the correct compiler.
- [ ] **CHTL Compiler**: Handles CHTL fragments.
- [ ] **CHTL JS Compiler**: Handles CHTL JS fragments.
- [ ] **CSS Compiler**: Handles CSS fragments.
- [ ] **JS Compiler**: Handles JS fragments.
- [ ] **CodeMerger**: Assembles final output from compiler results.

## 5. Tooling

### 5.1. CLI
- [ ] Standard command-line tool.
- [ ] Advanced command-line program with rendering.
- [ ] Default behavior is to generate a fragment, not a full HTML document. The `--default-struct` flag must be used to generate the full `<html>...</html>` shell.

### 5.2. VSCode IDE Extension
- [ ] Syntax Highlighting
- [ ] Code Formatting
- [ ] Code Completion
- [ ] Live Preview
- [ ] Built-in Compiler
- [ ] Module Support (unpacking, JSON query tables)
- [ ] Context Menu Actions (compile, open in browser, etc.)
