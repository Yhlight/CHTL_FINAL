# CHTL VSCode Extension - Implementation Plan

This document outlines the plan and architecture for the CHTL VSCode extension, based on the requirements in `CHTL.md`.

## 1. Core Language Features

### 1.1. Code Highlighting
- **Strategy:** Create a TextMate grammar file (`.tmLanguage.json`).
- **Implementation:**
    - Define scopes for all CHTL keywords (`div`, `text`, `style`, `[Origin]`, `[Namespace]`, etc.).
    - Define scopes for language constructs: element names, attributes, properties, string literals, numbers, comments.
    - Define scopes for operators (`:`, `=`, `+`, `?`, etc.).
    - This will provide basic syntax highlighting.

### 1.2. Code Formatting
- **Strategy:** Implement a `DocumentFormattingEditProvider`.
- **Implementation:**
    - The provider will be triggered by the "Format Document" command.
    - It will parse the entire document using the CHTL compiler's parser to build an AST.
    - It will then use a "pretty-printer" visitor pattern on the AST to generate a consistently formatted string with correct indentation and spacing.
    - This leverages the existing compiler infrastructure.

### 1.3. Code Completion
- **Strategy:** Implement a `CompletionItemProvider`.
- **Implementation:**
    - Provide completion for HTML tag names.
    - Provide completion for CHTL keywords (`text`, `style`, `[Origin]`, etc.).
    - Context-aware completion for CSS properties within a style context.
    - This will be based on simple text analysis initially, and can be enhanced with the AST later.

## 2. IDE Integration and Commands

### 2.1. Page Preview & Live Preview
- **Strategy:** Use a `WebviewPanel` to render the compiled HTML.
- **Implementation:**
    - Add a "CHTL: Preview" command.
    - When triggered, it will run the `chtl` compiler on the active file.
    - The generated HTML will be loaded into a webview panel in a separate editor tab.
    - For **Live Preview**, a file watcher will be used. On file save, the compiler will be re-run and the webview will be updated automatically.

### 2.2. Right-Click Context Menu Commands
- **Strategy:** Add commands to the `contributes.menus.editor/context` section of `package.json`.
- **Implementation:**
    - **Open in Browser:** Compile the file to a temporary HTML file and open it in the user's default browser.
    - **Open Documentation:** Open a webview or external browser link to the CHTL documentation.
    - **Export HTML/CSS/JS:** Run the compiler/scanner and write the specific output to a file.
    - **Compile:** A context menu item that runs the `chtl` CLI command on the current file.

## 3. Advanced Features

### 3.1. Built-in Compiler and Modules
- **Strategy:** Package the `chtl` executable and the official `CMOD` modules with the extension.
- **Implementation:**
    - The extension will manage the path to the bundled executable.
    - The extension will set an environment variable or pass a flag to the compiler to point to the bundled official modules directory.

### 3.2. Auto Module Unpacking and JSON Query Table
- **Strategy:** This is a complex feature for providing rich intellisense.
- **Implementation:**
    - On workspace load, the extension will scan for `.cmod` files in the module paths.
    - It will run a helper command (part of the `chtl` CLI or a separate script) to unpack the `.cmod` (zip) file into a temporary cache.
    - It will parse the `[Export]` block from the module's info file and generate a JSON file representing the available components.
    - The `CompletionItemProvider` will read these JSON files to provide completion for imported module components.

### 3.3. Module Syntax Hints & Solving `[]` Conflict
- **Strategy:** Use the JSON query tables generated in the previous step.
- **Implementation:**
    - **Hints:** When a user types `@Element ...`, the completion provider will suggest available elements from the JSON tables.
    - **`[]` Conflict:** The TextMate grammar will need to be carefully designed to distinguish CHTL's bracketed keywords (`[Origin]`) from array access in CHTL JS (`my_array[0]`). This will require context-aware grammar rules.
