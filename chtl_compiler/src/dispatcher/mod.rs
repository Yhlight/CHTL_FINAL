// This module will contain the main compiler pipeline dispatcher.
use crate::{ast, chtl_js, context::Context, generator, parser};

struct CompilationArtifacts<'a> {
    script_content: String,
    style_content: String,
    context: Context<'a>,
}

pub fn compile(source: &str) -> Result<String, String> {
    // 1. Parse the main CHTL source code
    let chtl_ast = parser::parse(source).map_err(|e| e.to_string())?;

    // 2. Build the compilation context from definitions
    let mut context = Context::new();
    for def in &chtl_ast.definitions {
        match def {
            ast::TopLevelDefinition::Template(template_def) => match template_def {
                ast::TemplateDefinition::Style(st) => {
                    context.style_templates.insert(st.name, st);
                }
                ast::TemplateDefinition::Element(et) => {
                    context.element_templates.insert(et.name, et);
                }
                ast::TemplateDefinition::Var(vt) => {
                    context.var_templates.insert(vt.name, vt);
                }
            }
        }
    }

    // 3. Walk the CHTL AST to extract script and style content
    let mut artifacts = CompilationArtifacts {
        script_content: String::new(),
        style_content: String::new(),
        context, // This context is not used by extract_blocks, but passed for future use
    };
    extract_blocks(&chtl_ast.children, &mut artifacts.script_content, &mut artifacts.style_content);

    // 4. Compile the collected CHTL JS content
    let generated_js = if !artifacts.script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&artifacts.script_content).map_err(|e| e.to_string())?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    // 5. Generate the base HTML from the CHTL AST
    let mut generated_html = generator::generate(&chtl_ast, &artifacts.context);

    // 6. Merge the generated code into the HTML
    if !artifacts.style_content.is_empty() {
        let style_tag = format!("<style>{}</style>", artifacts.style_content);
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

// This function only extracts hoistable content. It doesn't need the full context.
fn extract_blocks(nodes: &[ast::Node], script_content: &mut String, style_content: &mut String) {
    for node in nodes {
        if let ast::Node::Element(element) = node {
            // Recurse on children
            extract_blocks(&element.children, script_content, style_content);
        } else if let ast::Node::ScriptBlock(content) = node {
            script_content.push_str(content.trim());
            script_content.push('\n');
        } else if let ast::Node::StyleBlock(contents) = node {
            for sc in contents {
                if let ast::StyleContent::Ruleset(ruleset) = sc {
                    // Note: & replacement is not handled here yet. This is a simplification.
                    style_content.push_str(ruleset.selector.trim());
                    style_content.push_str(" { ");
                    for prop in &ruleset.properties {
                        style_content.push_str(&format!("{}:{};", prop.key, prop.value));
                    }
                    style_content.push_str(" }\n");
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_element_template_end_to_end() {
        let source = r#"
            [Template] @Element Box {
                div {
                    class: "box";
                    text { "This is a box." }
                }
            }
            body { @Element Box; }
        "#;
        let expected_html = "<body><div class=\"box\">This is a box.</div>\n</body>\n";
        let result = compile(source).unwrap();
        assert_eq!(result, expected_html);
    }

    #[test]
    fn test_style_template_end_to_end() {
        let source = r#"
            [Template] @Style MyStyle { color: red; }
            p { style { @Style MyStyle; } }
        "#;
        let expected_html = "<p style=\"color:red;\"></p>\n";
        let result = compile(source).unwrap();
        assert_eq!(result, expected_html);
    }

    #[test]
    fn test_style_inheritance_end_to_end() {
        let source = r#"
            [Template] @Style Base {
                font-family: sans-serif;
                color: black;
            }
            [Template] @Style Theme {
                @Style Base;
                color: blue; /* Override */
            }
            p {
                style { @Style Theme; }
            }
        "#;
        let expected_html = "<p style=\"font-family:sans-serif;color:black;color:blue;\"></p>\n";
        let result = compile(source).unwrap();
        assert_eq!(result, expected_html);
    }
}
