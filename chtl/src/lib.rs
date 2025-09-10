pub mod CHTL;
pub mod Util;
pub mod Test;

#[cfg(test)]
mod tests {
    use super::CHTL::CHTLParser::parse_chtl;
    use super::CHTL::CHTLGenerator::generate_html;
    use super::CHTL::CHTLNode::{Document, Node, ElementNode, AttributeNode, TextNode};

    #[test]
    fn test_end_to_end_pipeline() {
        let chtl_source = r#"
            html {
                head {
                    title: "My CHTL Page";
                }
                body {
                    div {
                        id: myDiv;
                        text {
                            "Hello, World!"
                        }
                    }
                    br {}
                }
            }
        "#;

        // 1. Parse the source code into an AST
        let ast_result = parse_chtl(chtl_source);
        assert!(ast_result.is_ok(), "Parsing failed: {:?}", ast_result.err());
        let ast = ast_result.unwrap();

        // Verify the AST structure
        let expected_ast_corrected = Document {
            children: vec![
                Node::Element(ElementNode {
                    tag_name: "html".to_string(),
                    attributes: vec![],
                    children: vec![
                        Node::Element(ElementNode {
                            tag_name: "head".to_string(),
                            attributes: vec![
                                AttributeNode { name: "title".to_string(), value: "My CHTL Page".to_string() }
                            ],
                            children: vec![],
                        }),
                        Node::Element(ElementNode {
                            tag_name: "body".to_string(),
                            attributes: vec![],
                            children: vec![
                                Node::Element(ElementNode {
                                    tag_name: "div".to_string(),
                                    attributes: vec![
                                        AttributeNode { name: "id".to_string(), value: "myDiv".to_string() }
                                    ],
                                    children: vec![
                                        Node::Text(TextNode { content: "Hello, World!".to_string() })
                                    ],
                                }),
                                Node::Element(ElementNode {
                                    tag_name: "br".to_string(),
                                    attributes: vec![],
                                    children: vec![],
                                }),
                            ],
                        }),
                    ],
                })
            ],
        };
        assert_eq!(ast, expected_ast_corrected);


        // 2. Generate HTML from the AST
        let html_output = generate_html(&ast);

        // 3. Define the expected HTML output
        let expected_html = r#"<html>
  <head title="My CHTL Page">
  </head>
  <body>
    <div id="myDiv">
      Hello, World!
    </div>
    <br />
  </body>
</html>
"#;
        // Compare by removing all whitespace
        let simplified_output = html_output.chars().filter(|c| !c.is_whitespace()).collect::<String>();
        let simplified_expected = expected_html.chars().filter(|c| !c.is_whitespace()).collect::<String>();

        assert_eq!(simplified_output, simplified_expected);
    }
}
