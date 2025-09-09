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

pub fn compile(source_path: &Path) -> Result<String, ChtlError> {
    // Create the arena for this compilation
    let arena = Bump::new();

    // 1. Recursively parse the entire dependency tree.
    let mut processed_files = HashSet::new();
    let all_docs = process_file_and_imports(source_path, &arena, &mut processed_files)?;

    // 2. Build the compilation context from all discovered definitions.
    let mut context = Context::new();
    for doc in &all_docs {
        for def in &doc.definitions {
            match def {
                ast::TopLevelDefinition::Use(_) => {}
                ast::TopLevelDefinition::Template(template_def) => {
                    match template_def {
                        ast::TemplateDefinition::Style(st) => { context.style_templates.insert(st.name, st); },
                        ast::TemplateDefinition::Element(et) => { context.element_templates.insert(et.name, et); },
                        ast::TemplateDefinition::Var(vt) => { context.var_templates.insert(vt.name, vt); },
                    };
                }
                ast::TopLevelDefinition::Custom(custom_def) => {
                     match custom_def {
                        ast::CustomDefinition::Style(st) => { context.custom_style_templates.insert(st.name, st); },
                        ast::CustomDefinition::Element(et) => { context.custom_element_templates.insert(et.name, et); },
                        ast::CustomDefinition::Var(vt) => { context.custom_var_templates.insert(vt.name, vt); },
                    };
                }
            }
        }
    }

    let main_ast = all_docs.get(0).ok_or_else(|| ChtlError::Generic("Failed to process the main source file.".to_string()))?;

    let mut script_content = String::new();
    let mut style_content = String::new();
    extract_hoisted_blocks(&main_ast.children, &mut script_content, &mut style_content, &context);

    let generated_js = if !script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&script_content).map_err(|e| ChtlError::ParseError {
            path: source_path.to_path_buf(),
            message: e.to_string(),
        })?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    let mut generated_html = generator::generate(main_ast, &context);

    if !style_content.is_empty() {
        let style_tag = format!("<style>{}</style>", style_content);
        if let Some(pos) = generated_html.find("</head>") {
            generated_html.insert_str(pos, &style_tag);
        } else {
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

fn get_css_value<'a>(value: &'a Option<ast::CssValue<'a>>, context: &Context<'a>) -> Option<String> {
    match value {
        Some(ast::CssValue::Literal(s)) => Some(s.to_string()),
        Some(ast::CssValue::Variable(usage)) => {
            if let Some(var_group) = context.var_templates.get(usage.group_name) {
                if let Some(variable) = var_group.variables.iter().find(|v| v.key == usage.var_name) {
                    return Some(variable.value.to_string());
                }
            }
            None
        }
        None => None,
    }
}

fn extract_hoisted_blocks(
    nodes: &[ast::Node],
    script_content: &mut String,
    style_content: &mut String,
    context: &Context,
) {
    for node in nodes {
        if let ast::Node::Element(element) = node {
            extract_hoisted_blocks(&element.children, script_content, style_content, context);
        } else if let ast::Node::ScriptBlock(content) = node {
            script_content.push_str(content.trim());
            script_content.push('\n');
        } else if let ast::Node::StyleBlock(contents) = node {
            for sc in contents {
                if let ast::StyleContent::Ruleset(ruleset) = sc {
                    style_content.push_str(ruleset.selector.trim());
                    style_content.push_str(" { ");
                    for prop in &ruleset.properties {
                        if let Some(value) = get_css_value(&prop.value, context) {
                            style_content.push_str(&format!("{}:{};", prop.key, value));
                        }
                    }
                    style_content.push_str(" }\n");
                }
            }
        }
    }
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

    // This test is commented out as it's for a feature not yet fully implemented.
    // #[test]
    // fn test_valueless_property_usage() { ... }
}
