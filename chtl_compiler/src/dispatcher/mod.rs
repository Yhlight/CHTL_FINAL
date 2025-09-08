// This module will contain the main compiler pipeline dispatcher.
use crate::{ast, chtl_js, generator, parser};

struct CompilationArtifacts {
    script_content: String,
    style_content: String,
}

pub fn compile(source: &str) -> Result<String, String> {
    // 1. Parse the main CHTL source code
    let chtl_ast = parser::parse(source).map_err(|e| e.to_string())?;

    // 2. Walk the CHTL AST to extract script and style content
    let mut artifacts = CompilationArtifacts {
        script_content: String::new(),
        style_content: String::new(),
    };
    extract_blocks(&chtl_ast.children, &mut artifacts, None);

    // 3. Compile the collected CHTL JS content
    let generated_js = if !artifacts.script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&artifacts.script_content).map_err(|e| e.to_string())?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    // 4. Generate the base HTML from the CHTL AST
    let mut generated_html = generator::generate(&chtl_ast);

    // 5. Merge the generated code into the HTML
    // Merge styles
    if !artifacts.style_content.is_empty() {
        let style_tag = format!("<style>{}</style>", artifacts.style_content);
        if let Some(pos) = generated_html.find("</head>") {
            generated_html.insert_str(pos, &style_tag);
        } else {
            generated_html.insert_str(0, &style_tag);
        }
    }
    // Merge scripts
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

fn extract_blocks(nodes: &[ast::Node], artifacts: &mut CompilationArtifacts, parent_selector: Option<String>) {
    for node in nodes {
        match node {
            ast::Node::Element(element) => {
                // Determine the primary selector for this element to pass to children.
                // Spec: class has priority over id.
                let mut primary_selector = None;

                // Check for explicit class
                if let Some(attr) = element.attributes.iter().find(|a| a.key == "class") {
                    primary_selector = Some(format!(".{}", attr.value.split_whitespace().next().unwrap_or("")));
                }

                // If no class, check for auto-generated class from style block
                if primary_selector.is_none() {
                     for child_node in &element.children {
                        if let ast::Node::StyleBlock(contents) = child_node {
                            for sc in contents {
                                if let ast::StyleContent::Ruleset(r) = sc {
                                    if r.selector.starts_with('.') {
                                        primary_selector = Some(r.selector.split_whitespace().next().unwrap_or("").to_string());
                                        break; // Found first class selector
                                    }
                                }
                            }
                        }
                        if primary_selector.is_some() { break; }
                    }
                }

                // If still no selector, check for id (explicit or auto-gen)
                if primary_selector.is_none() {
                    if let Some(attr) = element.attributes.iter().find(|a| a.key == "id") {
                        primary_selector = Some(format!("#{}", attr.value));
                    } else {
                        for child_node in &element.children {
                            if let ast::Node::StyleBlock(contents) = child_node {
                                for sc in contents {
                                    if let ast::StyleContent::Ruleset(r) = sc {
                                        if r.selector.starts_with('#') {
                                            primary_selector = Some(r.selector.split_whitespace().next().unwrap_or("").to_string());
                                            break;
                                        }
                                    }
                                }
                            }
                             if primary_selector.is_some() { break; }
                        }
                    }
                }

                extract_blocks(&element.children, artifacts, primary_selector);
            }
            ast::Node::ScriptBlock(content) => {
                artifacts.script_content.push_str(content.trim());
                artifacts.script_content.push('\n');
            }
            ast::Node::StyleBlock(style_contents) => {
                for sc in style_contents {
                    if let ast::StyleContent::Ruleset(ruleset) = sc {
                        let mut selector = ruleset.selector.trim().to_string();
                        // Replace '&' with the parent selector if it exists
                        if let Some(ps) = &parent_selector {
                            selector = selector.replace("&", ps);
                        }

                        artifacts.style_content.push_str(&selector);
                        artifacts.style_content.push_str(" { ");
                        for prop in &ruleset.properties {
                            artifacts.style_content.push_str(&format!("{}:{};", prop.key, prop.value));
                        }
                        artifacts.style_content.push_str(" }\n");
                    }
                }
            }
            _ => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_context_selector_replacement() {
        let source = r#"
            head {}
            body {
                div {
                    class: "box";
                    style {
                        .nested { color: green; } /* hoisted */
                        &:hover { color: blue; } /* hoisted with context */
                    }
                }
            }
        "#;

        let result = compile(source).unwrap();

        // Check for hoisted styles
        assert!(result.contains(".nested { color:green; }"));
        assert!(result.contains(".box:hover { color:blue; }"));
    }
}
