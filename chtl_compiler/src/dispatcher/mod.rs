// This module will contain the main compiler pipeline dispatcher.
use crate::{ast, chtl_js, context::Context, errors::ChtlError, generator, parser};
use bumpalo::Bump;
use std::collections::HashSet;
use std::fs;
use std::path::{Path, PathBuf};

/// Recursively processes a file and its imports, returning a flattened list of all parsed documents.
fn process_file_and_imports<'a>(
    path: &Path,
    arena: &'a Bump,
    processed_files: &mut HashSet<PathBuf>,
) -> Result<Vec<ast::Document<'a>>, ChtlError> {
    let canonical_path = path.canonicalize().map_err(|e| ChtlError::IoError {
        path: path.to_path_buf(),
        source: e,
    })?;
    if processed_files.contains(&canonical_path) {
        return Ok(Vec::new());
    }
    processed_files.insert(canonical_path);

    let source = fs::read_to_string(path).map_err(|e| ChtlError::IoError {
        path: path.to_path_buf(),
        source: e,
    })?;
    let source_in_arena: &'a str = arena.alloc_str(&source);
    let document = parser::parse(source_in_arena).map_err(|e| ChtlError::ParseError {
        path: path.to_path_buf(),
        message: e.to_string(),
    })?;

    let mut modules_to_process = Vec::new();
    for def in &document.definitions {
        if let ast::TopLevelDefinition::Use(use_stmt) = def {
            let parent_dir = path.parent().unwrap_or_else(|| Path::new(""));
            let module_path = parent_dir.join(use_stmt.path);
            modules_to_process.push(module_path);
        }
    }

    let mut all_docs = vec![document];
    for module_path in modules_to_process {
        let imported_docs = process_file_and_imports(&module_path, arena, processed_files)?;
        all_docs.extend(imported_docs);
    }

    Ok(all_docs)
}

fn extract_script_blocks(nodes: &[ast::Node], script_content: &mut String) {
    for node in nodes {
        match node {
            ast::Node::Element(element) => {
                extract_script_blocks(&element.children, script_content);
            }
            ast::Node::ScriptBlock(content) => {
                script_content.push_str(content.trim());
                script_content.push('\n');
            }
            _ => {}
        }
    }
}

pub fn compile(source_path: &Path) -> Result<String, ChtlError> {
    let arena = Bump::new();
    let mut processed_files = HashSet::new();
    let all_docs = process_file_and_imports(source_path, &arena, &mut processed_files)?;

    let mut context = Context::new();
    for doc in &all_docs {
        for def in &doc.definitions {
            match def {
                ast::TopLevelDefinition::Use(_) => {}
                ast::TopLevelDefinition::Template(template_def) => match template_def {
                    ast::TemplateDefinition::Style(st) => { context.style_templates.insert(st.name, st); },
                    ast::TemplateDefinition::Element(et) => { context.element_templates.insert(et.name, et); },
                    ast::TemplateDefinition::Var(vt) => { context.var_templates.insert(vt.name, vt); },
                },
                ast::TopLevelDefinition::Custom(custom_def) => match custom_def {
                    ast::CustomDefinition::Style(st) => { context.custom_style_templates.insert(st.name, st); },
                    ast::CustomDefinition::Element(et) => { context.custom_element_templates.insert(et.name, et); },
                    ast::CustomDefinition::Var(vt) => { context.custom_var_templates.insert(vt.name, vt); },
                },
            }
        }
    }

    let main_ast = all_docs.get(0).ok_or_else(|| ChtlError::Generic("Failed to process the main source file.".to_string()))?;

    // Extract script content
    let mut script_content = String::new();
    extract_script_blocks(&main_ast.children, &mut script_content);

    // Compile CHTL-JS
    let generated_js = if !script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&script_content).map_err(|e| ChtlError::ParseError {
            path: source_path.to_path_buf(),
            message: e.to_string(),
        })?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    // Generate HTML and Hoisted Styles
    let mut generated_html = generator::generate(main_ast, &context);
    let style_content = context.hoisted_styles.into_inner();

    // Inject styles and scripts
    if !style_content.is_empty() {
        let style_tag = format!("<style>{}</style>", style_content);
        if let Some(pos) = generated_html.find("</head>") {
            generated_html.insert_str(pos, &style_tag);
        } else {
            // A simple fallback if no <head> is present
            generated_html.insert_str(0, &style_tag);
        }
    }
    if !generated_js.is_empty() {
        let script_tag = format!("<script>{}</script>", generated_js);
        if let Some(pos) = generated_html.rfind("</body>") {
            generated_html.insert_str(pos, &script_tag);
        } else {
            generated_html.push_str(&script_tag);
        }
    }

    Ok(generated_html)
}


#[cfg(test)]
mod tests {
    use std::fs::File;
    use std::io::Write;
    use tempfile::tempdir;

    #[test]
    fn test_end_to_end_delete_specialization() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("main.chtl");
        let mut file = File::create(&file_path).unwrap();
        let source = r#"
            [Custom] @Element Box {
                div class: "container" {
                    p { text { "Hello" } }
                    span { text { "Delete me" } }
                }
            }

            @Element Box {
                delete span;
            }
        "#;
        file.write_all(source.as_bytes()).unwrap();

        let expected_html = "<div class=\"container\"><p>Hello</p>\n</div>\n";
        let result = super::compile(&file_path).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_end_to_end_insert_specialization() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("main.chtl");
        let mut file = File::create(&file_path).unwrap();
        let source = r#"
            [Custom] @Element Box {
                div class: "container" {
                    p { text { "Hello" } }
                }
            }

            @Element Box {
                insert after p {
                    hr {}
                }
            }
        "#;
        file.write_all(source.as_bytes()).unwrap();

        let expected_html = "<div class=\"container\"><p>Hello</p>\n<hr></hr>\n</div>\n";
        let result = super::compile(&file_path).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_module_import() {
        let dir = tempdir().unwrap();

        let module_path = dir.path().join("components.cmod");
        let mut module_file = File::create(&module_path).unwrap();
        let module_source = r#"
            [Custom] @Element MyButton {
                button class: "btn" {
                    text { "Click Me" }
                }
            }
        "#;
        module_file.write_all(module_source.as_bytes()).unwrap();

        let main_path = dir.path().join("main.chtl");
        let mut main_file = File::create(&main_path).unwrap();
        let main_source = r#"
            use "./components.cmod";

            @Element MyButton {}
        "#;
        main_file.write_all(main_source.as_bytes()).unwrap();

        let expected_html = "<button class=\"btn\">Click Me</button>\n";
        let result = super::compile(&main_path).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_end_to_end_modify_specialization() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("main.chtl");
        let mut file = File::create(&file_path).unwrap();
        let source = r#"
            [Custom] @Element Box {
                div {
                    p { text { "Original" } }
                }
            }

            @Element Box {
                modify div {
                    h1 { text { "Modified" } }
                }
            }
        "#;
        file.write_all(source.as_bytes()).unwrap();

        let expected_html = "<div><h1>Modified</h1>\n</div>\n";
        let result = super::compile(&file_path).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_contextual_selector() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("main.chtl");
        let mut file = File::create(&file_path).unwrap();
        let source = r##"
            div {
                style {
                    & {
                        border: "1px solid black";
                    }
                    &:hover {
                        background-color: "#eee";
                    }
                }
                p { text { "Hover me" } }
            }
        "##;
        file.write_all(source.as_bytes()).unwrap();

        let result = super::compile(&file_path).unwrap();

        // Check for the unique class on the div
        assert!(result.contains("<div class=\"chtl-1\">"));

        // Check for the correctly compiled and hoisted CSS
        // We check for substrings to be robust against minor whitespace changes.
        assert!(result.contains(".chtl-1 { border:1px solid black; }"));
        assert!(result.contains(".chtl-1:hover { background-color:#eee; }"));
    }
}
