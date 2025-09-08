// This module will contain the main compiler pipeline dispatcher.
use crate::{ast, chtl_js, generator, parser};

pub fn compile(source: &str) -> Result<String, String> {
    // 1. Parse the main CHTL source code
    let chtl_ast = parser::parse(source).map_err(|e| e.to_string())?;

    // 2. Walk the CHTL AST to find and collect all script and style content
    let mut script_content = String::new();
    // In the future, we'll collect style content here too.
    // let mut style_content = String::new();

    extract_special_blocks(&chtl_ast.children, &mut script_content);

    // 3. Compile the collected CHTL JS content
    let generated_js = if !script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&script_content).map_err(|e| e.to_string())?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    // 4. Generate the base HTML from the CHTL AST
    let mut generated_html = generator::generate(&chtl_ast);

    // 5. Merge the generated JS into the HTML
    if !generated_js.is_empty() {
        let script_tag = format!("<script>{}</script>", generated_js);
        // Simple injection strategy: append before </body> or at the end of the file.
        if let Some(pos) = generated_html.rfind("</body>") {
            generated_html.insert_str(pos, &script_tag);
        } else {
            generated_html.push_str(&script_tag);
        }
    }

    Ok(generated_html)
}

fn extract_special_blocks(nodes: &[ast::Node], script_content: &mut String) {
    for node in nodes {
        match node {
            ast::Node::Element(element) => {
                // Recursively search in children
                extract_special_blocks(&element.children, script_content);
            }
            ast::Node::ScriptBlock(content) => {
                script_content.push_str(content.trim());
                script_content.push('\n'); // Add a newline to separate scripts
            }
            // We'll handle StyleBlock later
            _ => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_end_to_end_compilation() {
        let source = r#"
            body {
                div {}
                script {
                    const el = "div";
                }
            }
        "#;

        // The script content will be passed through the CHTL JS compiler as "other_code".
        // The extractor also adds a newline.
        let expected_html = "<body><div></div>\n<script>const el = \"div\";\n</script></body>\n";
        let result = compile(source).unwrap();

        assert_eq!(result, expected_html);
    }
}
