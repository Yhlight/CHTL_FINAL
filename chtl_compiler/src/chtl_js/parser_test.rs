use crate::chtl_js::lexer::Lexer;
use crate::chtl_js::parser::Parser;
use crate::chtl_js::node::{Node, Value};
use std::collections::HashMap;

#[test]
fn test_parse_chtl_js_function_call() {
    let input = r#"
        listen {
            target: {{.box}},
            events: [ "click", "mouseenter" ],
            options: {
                capture: true,
            }
        }
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert!(parser.errors.is_empty(), "Parser has errors: {:?}", parser.errors);
    assert_eq!(program.len(), 1);

    if let Node::FunctionCall(fc) = &program[0] {
        assert_eq!(fc.name, "listen");

        // Test target
        assert!(fc.arguments.contains_key("target"));
        if let Some(Value::Identifier(id)) = fc.arguments.get("target") {
            assert_eq!(id, "{{.box}}"); // Lexer will treat this as one ident
        } else { panic!("target was not an identifier"); }

        // Test events
        assert!(fc.arguments.contains_key("events"));
        if let Some(Value::Array(arr)) = fc.arguments.get("events") {
            assert_eq!(arr.len(), 2);
            assert_eq!(arr[0], Value::String("click".to_string()));
            assert_eq!(arr[1], Value::String("mouseenter".to_string()));
        } else { panic!("events was not an array"); }

        // Test options
        assert!(fc.arguments.contains_key("options"));
        if let Some(Value::Object(obj)) = fc.arguments.get("options") {
            assert!(obj.contains_key("capture"));
            if let Some(Value::Identifier(val)) = obj.get("capture") {
                assert_eq!(val, "true");
            } else { panic!("capture was not an identifier(bool)"); }
        } else { panic!("options was not an object"); }

    } else {
        panic!("Node was not a FunctionCall");
    }
}
