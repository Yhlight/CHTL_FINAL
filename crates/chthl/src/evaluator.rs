//! This module defines the Evaluator, which walks the AST and evaluates
//! expressions, producing a final value represented by the `Object` enum.

use crate::ast::Expression;
use std::collections::HashMap;

pub type Environment = HashMap<String, Object>;

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

fn is_truthy(obj: Object) -> bool {
    match obj {
        Object::Boolean(b) => b,
        Object::Null => false,
        Object::Number(n) => n != 0.0,
        Object::String(s) => !s.is_empty(),
        Object::Unit(_, _) => true,
    }
}

pub fn eval(node: &Expression, env: &Environment) -> Result<Object, String> {
    match node {
        Expression::NumberLiteral(n) => Ok(Object::Number(*n)),
        Expression::StringLiteral(s) => Ok(Object::String(s.clone())),
        Expression::UnitLiteral(n, u) => Ok(Object::Unit(*n, u.clone())),
        Expression::Boolean(b) => Ok(Object::Boolean(*b)),
        Expression::Ident(name) => eval_identifier(name, env),
        Expression::Infix(infix_expr) => {
            let left = eval(&infix_expr.left, env)?;
            let right = eval(&infix_expr.right, env)?;
            eval_infix_expression(&infix_expr.operator, left, right)
        },
        Expression::Ternary(ternary_expr) => {
            let condition = eval(&ternary_expr.condition, env)?;
            if is_truthy(condition) {
                eval(&ternary_expr.consequence, env)
            } else {
                eval(&ternary_expr.alternative, env)
            }
        }
        _ => Err("Evaluation for this expression type is not yet implemented".to_string()),
    }
}

fn eval_identifier(name: &str, env: &Environment) -> Result<Object, String> {
    if let Some(val) = env.get(name) {
        Ok(val.clone())
    } else {
        Ok(Object::String(name.to_string()))
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

    #[test]
    fn test_identifier_evaluation() {
        let mut env = Environment::new();
        env.insert("width".to_string(), Object::Number(100.0));

        let input = Expression::Ident("width".to_string());
        let result = eval(&input, &env).unwrap();
        assert_eq!(result, Object::Number(100.0));

        let input2 = Expression::Ident("red".to_string());
        let result2 = eval(&input2, &env).unwrap();
        assert_eq!(result2, Object::String("red".to_string()));
    }

    #[test]
    fn test_ternary_evaluation() {
        let env = Environment::new();
        let ten = Box::new(Expression::NumberLiteral(10.0));
        let five = Box::new(Expression::NumberLiteral(5.0));
        let one = Box::new(Expression::NumberLiteral(1.0));
        let two = Box::new(Expression::NumberLiteral(2.0));

        let condition = Expression::Infix(crate::ast::InfixExpression{
            left: ten.clone(),
            operator: ">".to_string(),
            right: five.clone(),
        });

        let ternary = Expression::Ternary(crate::ast::TernaryExpression{
            condition: Box::new(condition),
            consequence: one.clone(),
            alternative: two.clone(),
        });

        let result = eval(&ternary, &env).unwrap();
        assert_eq!(result, Object::Number(1.0));
    }
}
