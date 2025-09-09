//! This module defines the Evaluator, which walks the AST and evaluates
//! expressions, producing a final value represented by the `Object` enum.

use crate::ast::Expression;
#[cfg(test)]
use crate::ast::Node;

#[derive(Debug, PartialEq, Clone)]
pub enum Object {
    Number(f64),
    String(String),
    Boolean(bool),
    Unit(f64, String),
    Null,
}

impl std::fmt::Display for Object {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Object::Number(n) => write!(f, "{}", n),
            Object::String(s) => write!(f, "{}", s),
            Object::Boolean(b) => write!(f, "{}", b),
            Object::Unit(n, u) => write!(f, "{}{}", n, u),
            Object::Null => write!(f, "null"),
        }
    }
}

pub fn eval(node: &Expression) -> Result<Object, String> {
    match node {
        Expression::NumberLiteral(n) => Ok(Object::Number(*n)),
        Expression::StringLiteral(s) => Ok(Object::String(s.clone())),
        Expression::UnitLiteral(n, u) => Ok(Object::Unit(*n, u.clone())),
        Expression::Boolean(b) => Ok(Object::Boolean(*b)),
        Expression::Infix(infix_expr) => {
            let left = eval(&infix_expr.left)?;
            let right = eval(&infix_expr.right)?;
            eval_infix_expression(&infix_expr.operator, left, right)
        }
        _ => Err("Evaluation for this expression type is not yet implemented".to_string()),
    }
}

fn eval_infix_expression(operator: &str, left: Object, right: Object) -> Result<Object, String> {
    match (left, right) {
        (Object::Number(l), Object::Number(r)) => {
            match operator {
                "+" => Ok(Object::Number(l + r)),
                "-" => Ok(Object::Number(l - r)),
                "*" => Ok(Object::Number(l * r)),
                "/" => Ok(Object::Number(l / r)),
                ">" => Ok(Object::Boolean(l > r)),
                "<" => Ok(Object::Boolean(l < r)),
                _ => Err(format!("Unknown operator for numbers: {}", operator)),
            }
        }
        (l, r) => Err(format!("Type mismatch: cannot apply operator '{}' to {} and {}", operator, l, r)),
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    fn eval_input(input: &str) -> Result<Object, String> {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        if !parser.errors.is_empty() {
            return Err(format!("Parser errors: {:?}", parser.errors));
        }

        if let Some(node) = program.nodes.get(0) {
            if let Node::Element(el) = node {
                if let Some(Node::Style(style)) = el.children.get(0) {
                    if let Some(prop) = style.inline_properties.get(0) {
                        return eval(&prop.value);
                    }
                }
            }
        }

        Err("Test setup failed: could not find expression in parsed program".to_string())
    }

    #[test]
    fn test_integer_arithmetic_evaluation() {
        let tests = vec![
            ("width: 5 + 10;", Object::Number(15.0)),
            ("width: 10 * 2 + 5;", Object::Number(25.0)),
            ("width: 50 / 2 * 2 + 10;", Object::Number(60.0)),
            // ("width: 2 * (5 + 10);", Object::Number(30.0)), // Parentheses need parser support
        ];

        for (input, expected) in tests {
            let full_input = format!("div {{ style {{ {} }} }}", input);
            match eval_input(&full_input) {
                Ok(obj) => assert_eq!(obj, expected, "Failed on input: {}", input),
                Err(e) => panic!("Evaluation failed for '{}': {}", input, e),
            }
        }
    }

    #[test]
    fn test_boolean_evaluation() {
         let tests = vec![
            ("width: 10 > 5;", Object::Boolean(true)),
            ("width: 5 < 10;", Object::Boolean(true)),
            ("width: 10 > 10;", Object::Boolean(false)),
         ];

        for (input, expected) in tests {
            let full_input = format!("div {{ style {{ {} }} }}", input);
            match eval_input(&full_input) {
                Ok(obj) => assert_eq!(obj, expected, "Failed on input: {}", input),
                Err(e) => panic!("Evaluation failed for '{}': {}", input, e),
            }
        }
    }
}
