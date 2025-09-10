# CHTL Project Analysis

This document contains a detailed analysis of the `CHTL.md` specification. It will be used as the guiding reference for the implementation of the CHTL project.

## 1. About CHTL
- **Core Concept:** A C++ based hypertext language, aiming to be a more developer-friendly alternative to HTML.
- **License:** MIT.

## 2. Comments
- `//`: Single-line comment. Not present in generated HTML.
- `/**/`: Multi-line comment. Not present in generated HTML.
- `--`: Generator-aware comment. Will be converted to the appropriate comment format (e.g., `<!-- -->` in HTML) based on context.

## 3. Text Nodes
- **Block Syntax:** `text { "content" }`. Can be used outside elements.
- **Attribute Syntax:** `text: "content";`. Can only be used inside elements.

## 4. Literals
- **Unquoted Literals:** Strings without quotes are supported for text and attribute values (e.g., `color: red;`).
- **Quoted Literals:** Standard double (`"`) and single (`'`) quotes are also supported.

## 5. CE Equivalence (Colon = Equal)
- **Concept:** `:` and `=` are treated as equivalent for assignments.
- **Recommendation:** Use `=` in recommended scenarios (to be defined, but likely for clarity).

## 6. Element Nodes
- **Syntax:** `element_name { ... }`.
- **Support:** All standard HTML tags are supported, regardless of their type (single, double, block, inline, etc.).
- **Nesting:** Elements can be nested to represent the HTML structure.

## 7. Attributes
- **Syntax:** `attribute_name: "value";`.
- **Example:** `id: box; class: welcome;`.

## 8. Local Style Blocks (`style {}`)
- **Concept:** An enhancement over `<style>`, allowing CSS-like declarations directly inside an element's definition.
- **Functionality:**
    - **Inline Styles:** Direct properties become inline styles.
    - **Global Styles:** Class, ID, pseudo-class, and pseudo-element selectors are automatically extracted and placed into a global style block.
- **Note:** Global `<style>` blocks are not enhanced with these features.

### 8.1. Inline Styles
- **Syntax:** Properties defined directly within `style {}` are applied as inline styles to the parent element.
- **Example:** `div { style { width: 100px; } }` becomes `<div style="width: 100px;"></div>`.

### 8.2. Automatic Class/ID
- **Concept:** Using a class (`.box`) or ID (`#box`) selector inside a local `style {}` block automatically adds that class or ID to the element's attributes.
- **Behavior:** The style rule itself is moved to a global style block.

### 8.3. Context Deduction (`&`)
- **Concept:** The `&` symbol refers to the current element's selector context.
- **Usage:** Primarily for simplifying pseudo-class (`&:hover`) and pseudo-element (`&::before`) selectors. CHTL will resolve `&` to the element's class or ID (class has priority).

### 8.4. Attribute Operations
- **Concept:** Attribute values support direct arithmetic operations: `+`, `-`, `*`, `/`, `%` (modulo), `**` (power).
- **Scope:** Supported in both local and global `style` blocks.
- **Comparison:** Eliminates the need for `calc()` in many cases.

### 8.5. Referencing Attributes
- **Concept:** Reference the attribute value of another element.
- **Syntax:** `CSS_selector.property`.
- **Supported Selectors (for performance):**
    - `box` (auto-inference: tag -> id -> class)
    - `.box` (class)
    - `#box` (id)
    - `button` (tag)
    - `.box button` (descendant)
    - `button[0]` (precise index access)
- **Scope:** Not supported in global `style` blocks.

### 8.6. Attribute Conditional Expressions
- **Concept:** Ternary-like expressions based on the element's own style properties.
- **Syntax:** `condition ? true_option : false_option`.
- **Features:**
    - **Chaining:** Multiple conditions can be chained with commas.
    - **Optional `false` option:** The `: false_option` part can be omitted.
    - **Operators:** Supports arithmetic (`+`, `-`, etc.) and logical (`&&`, `||`) operators.
- **Scope:** Supported in global `style` blocks.

### 8.7. Referenced Attribute Conditional Expressions
- **Concept:** Conditional expressions can reference attributes from other elements.
- **Syntax:** Same as referencing attributes: `CSS_selector.property`.
- **Scope:** Not supported in global `style` blocks.

## 9. Templates (`[Template]`)
- **Concept:** Reusable blocks of code.
- **Types:**
    - `@Style`: Style groups.
    - `@Element`: Element groups.
    - `@Var`: Variable groups (simple value substitution, not CSS variables).
- **Usage:** `@Type TemplateName;` or `@Type TemplateName { ... }` for specialization.

### 9.1. Combination Inheritance
- **Concept:** A template can include another template of the same type to inherit its contents.
- **Resolution:** Properties are overridden based on order of appearance.

### 9.2. Explicit Inheritance
- **Syntax:** `inherit @Type TemplateName;`.
- **Functionality:** Same as combination inheritance, but more explicit.

## 10. Customization (`[Custom]`)
- **Concept:** An extension of templates that allows for more flexible specialization.
- **Key Difference:** Customizations can be modified at the point of use.
- **Inheritance:** Templates and Customizations of the same type can inherit from each other.

### 10.1. Custom Style Groups
- **Valueless Properties:** Can declare properties without values, which must be provided when the custom style is used.
- **Specialization (Deletion):** Can delete properties (`delete prop1, prop2;`) or inherited styles (`delete @Style TemplateName;`) when using the custom style.

### 10.2. Custom Elements
- **Specialization:**
    - **Add Styles:** Add or override styles for specific child elements.
    - **Index Access:** Target specific elements by index (`div[1]`).
    - **Insert Elements:** `insert after/before/replace/at top/at bottom selector { ... }`.
    - **Delete Elements:** `delete selector;`.
    - **Delete Inheritance:** `delete @Element TemplateName;`.

### 10.3. Custom Variable Groups
- **Specialization:** Override variable values at the point of use. `ThemeColor(tableColor = rgb(145, 155, 200))`.

### 10.4. Fully Qualified Names
- **Concept:** Use the full name (`[Custom] @Element Box`) to resolve naming conflicts.

## 11. Raw Embedding (`[Origin]`)
- **Concept:** A mechanism to include raw code that will not be processed by CHTL.
- **Syntax:** `[Origin] @Type { ... }`.
- **Types:** `@Html`, `@Style`, `@JavaScript`.
- **Named Blocks:** Can be named (`[Origin] @Html box { ... }`) and reused (`[Origin] @Html box;`).
- **Custom Types:** Supports custom origin types like `@Vue`.

## 12. Importing (`[Import]`)
- **Concept:** Import other files.
- **Types:**
    - `@Html`, `@Style`, `@JavaScript`: Imports external files as named raw blocks.
    - `@Chtl`: Imports another CHTL file.
        - **Precise Import:** Import a specific named entity.
        - **Type Import:** Import all entities of a specific type.
        - **Wildcard Import:** Import all templates, customs, or origins.
        - **Full Import:** Import the entire file (`[Import] @Chtl from ...`).
- **Path Equivalence:** `.` and `/` are equivalent in paths.

## 13. Namespaces (`[Namespace]`)
- **Concept:** Group related entities to avoid naming collisions.
- **Usage:**
    - `[Namespace] space { ... }`
    - Imported files without a namespace are assigned one based on the filename by default.
    - Access members using `from`: `@Element Box from space.room2;`.
- **Behavior:** Same-named namespaces are merged.

## 14. Constraints (`except`)
- **Concept:** Restrict the types of elements or entities that can be used within a scope.
- **Types:**
    - **Precise:** `except span, [Custom] @Element Box;`
    - **Type:** `except @Html;`, `except [Template] @Var;`
    - **Global:** Applied within a namespace to the entire namespace.

## 15. Configuration (`[Configuration]`)
- **Concept:** A block to customize compiler behavior.
- **Features:**
    - Set global settings like `INDEX_INITIAL_COUNT`.
    - **`[Name]` Block:** Rename keywords.
    - **Named Configs:** `@Config Basic`. Only unnamed configs are active.
    - **Import/Use Configs:** `[Import] @Config ...` and `use @Config Basic;`.
    - **Disable Features:** `DISABLE_STYLE_AUTO_ADD_CLASS`, `DISABLE_DEFAULT_NAMESPACE`, etc.
    - **`[OriginType]` Block:** Define custom origin types.

## 16. `use` Statement
- **Concept:** A top-of-file directive.
- **Usage:**
    - `use html5;` to generate the HTML5 doctype.
    - `use @Config Basic;` to apply a named configuration.

## 17. CHTL JS
- **Concept:** A separate, independent language that compiles to JavaScript. It is NOT a superset of JS.
- **File Extension:** `.cjjs`.

### 17.1. CHTL JS Features
- **`fileloader {}`:** AMD-style module loader.
- **Local `script {}`:** JS block inside an element definition, with access to element context.
- **Enhanced Selector `{{...}}`:** A way to create DOM objects. `{{.box}}`, `{{#box}}`, `{{button[0]}}`.
- **`->` operator:** Equivalent to `.`, used to clarify that a CHTL JS feature is being used.
- **`listen {}`:** Declarative event listener binding.
- **`delegate {}`:** Declarative event delegation.
- **`animate {}`:** Wrapper for `requestAnimationFrame`.
- **`vir` (Virtual Objects):** A compile-time construct to access metadata of CHTL JS functions.
- **`router {}`:** A basic SPA router.
- **Dynamic Attribute Expressions:** `width: {{box}}->width > 2 ? 100px : 50px;`.

## 18. Module System
- **Concept:** A system for distributing and reusing CHTL and CHTL JS code.
- **CMOD:** A package format for CHTL components. Has a strict directory structure (`src`, `info`).
- **CJMOD:** A package format for CHTL JS extensions, written in C++.
- **Hybrid Modules:** CMOD + CJMOD combined.
- **Official Modules:** `chtl::Chtholly`, `chtl::Yuigahama`.

## 19. Project Structure and Build Process
- **Suggested Structure:** A detailed folder layout is provided for the compiler project itself.
- **Build Flow:**
    1.  **CHTLUnifiedScanner:** Splits the source file into CHTL, CHTL JS, CSS, and JS fragments.
    2.  **CompilerDispatcher:** Routes fragments to the correct compiler.
    3.  **Compilers:** Hand-written CHTL/CHTL JS compilers, and external (e.g., ANTLR-based) CSS/JS compilers.
    4.  **CodeMerger:** Combines the compiled outputs.
    5.  **Final Output:** Generates the final HTML file.

## 20. CJMOD API
- **Concept:** A C++ API for developers to create their own CJMOD modules, extending CHTL JS syntax.
- **Components:** `Syntax`, `Arg`, `CJMODScanner`, `CJMODGenerator`, `AtomArg`, `CHTLJSFunction`.

## 21. Unified Scanner Details
- **Core Task:** To accurately separate the mixed-language source code.
- **Key Technique:** A placeholder mechanism. JS code is replaced with `_JS_CODE_PLACEHOLDER_`. The CHTL JS compiler ignores these placeholders and the surrounding symbols (`{}`, `()`), allowing it to process only the pure CHTL JS syntax. The placeholders are restored during the final code merge.

## 22. CLI & VSCode IDE
- **CLI:** A standard CLI and an advanced, rendered version are planned.
- **VSCode IDE:** A detailed list of requirements for an IDE extension, including syntax highlighting, formatting, code completion, preview, and module integration.
