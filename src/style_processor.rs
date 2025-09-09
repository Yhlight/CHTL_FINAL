//! Advanced style processing for CHTL

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use std::collections::HashMap;

/// Style processor for handling complex CSS features
pub struct StyleProcessor {
    context: StyleContext,
}

/// Style processing context
#[derive(Debug, Clone)]
pub struct StyleContext {
    pub element_properties: HashMap<String, String>,
    pub parent_properties: HashMap<String, String>,
    pub global_styles: HashMap<String, StyleRule>,
}

/// Style rule with selectors and properties
#[derive(Debug, Clone)]
pub struct StyleRule {
    pub selectors: Vec<StyleSelector>,
    pub properties: HashMap<String, StyleValue>,
    pub conditions: Vec<StyleCondition>,
}

/// Style condition for conditional values
#[derive(Debug, Clone)]
pub struct StyleCondition {
    pub condition: String,
    pub value: String,
    pub is_optional: bool,
    pub operator: ConditionOperator,
}

/// Condition operators
#[derive(Debug, Clone, PartialEq)]
pub enum ConditionOperator {
    GreaterThan,
    LessThan,
    GreaterThanOrEqual,
    LessThanOrEqual,
    Equal,
    NotEqual,
    And,
    Or,
}

/// Style value with support for conditions and chains
#[derive(Debug, Clone)]
pub enum StyleValue {
    Simple(String),
    Conditional(Vec<StyleCondition>),
    Chain(Vec<StyleValue>),
    Expression(StyleExpression),
}

/// Style expression for complex calculations
#[derive(Debug, Clone)]
pub struct StyleExpression {
    pub expression: String,
    pub variables: HashMap<String, String>,
    pub functions: Vec<StyleFunction>,
}

/// Style function for calculations
#[derive(Debug, Clone)]
pub struct StyleFunction {
    pub name: String,
    pub parameters: Vec<String>,
    pub body: String,
}

impl StyleProcessor {
    pub fn new() -> Self {
        Self {
            context: StyleContext {
                element_properties: HashMap::new(),
                parent_properties: HashMap::new(),
                global_styles: HashMap::new(),
            },
        }
    }
    
    /// Process a style node and generate CSS
    pub fn process_style(&mut self, style: &StyleNode) -> Result<ProcessedStyle> {
        let mut processed = ProcessedStyle {
            rules: Vec::new(),
            variables: HashMap::new(),
            functions: Vec::new(),
        };
        
        // Process properties
        for (property, value) in &style.properties {
            let processed_value = self.process_style_value(value)?;
            processed.rules.push(StyleRule {
                selectors: vec![],
                properties: HashMap::from([(property.clone(), processed_value)]),
                conditions: vec![],
            });
        }
        
        // Process selectors
        for selector in &style.selectors {
            let rule = self.process_selector(selector)?;
            processed.rules.push(rule);
        }
        
        // Process conditions
        for condition in &style.conditions {
            let rule = self.process_condition(condition)?;
            processed.rules.push(rule);
        }
        
        Ok(processed)
    }
    
    /// Process a style value with all its complexities
    fn process_style_value(&self, value: &StyleValue) -> Result<StyleValue> {
        match value {
            StyleValue::Simple(s) => Ok(StyleValue::Simple(s.clone())),
            StyleValue::Conditional(conditions) => {
                let processed_conditions = self.process_conditions(conditions)?;
                Ok(StyleValue::Conditional(processed_conditions))
            }
            StyleValue::Chain(values) => {
                let processed_values = values.iter()
                    .map(|v| self.process_style_value(v))
                    .collect::<Result<Vec<_>>>()?;
                Ok(StyleValue::Chain(processed_values))
            }
            StyleValue::Expression(expr) => {
                let processed_expr = self.process_expression(expr)?;
                Ok(StyleValue::Expression(processed_expr))
            }
        }
    }
    
    /// Process conditional values with full operator support
    fn process_conditions(&self, conditions: &[StyleCondition]) -> Result<Vec<StyleCondition>> {
        let mut processed = Vec::new();
        
        for condition in conditions {
            let processed_condition = StyleCondition {
                condition: self.evaluate_condition(&condition.condition)?,
                value: condition.value.clone(),
                is_optional: condition.is_optional,
                operator: condition.operator.clone(),
            };
            processed.push(processed_condition);
        }
        
        Ok(processed)
    }
    
    /// Evaluate a condition expression
    fn evaluate_condition(&self, condition: &str) -> Result<String> {
        // Parse condition expression
        let tokens = self.tokenize_expression(condition)?;
        let ast = self.parse_condition_expression(&tokens)?;
        let result = self.evaluate_condition_ast(&ast)?;
        Ok(result)
    }
    
    /// Tokenize a condition expression
    fn tokenize_expression(&self, expr: &str) -> Result<Vec<ExpressionToken>> {
        let mut tokens = Vec::new();
        let mut chars = expr.chars().peekable();
        
        while let Some(ch) = chars.next() {
            match ch {
                ' ' | '\t' | '\n' => continue,
                '>' => {
                    if chars.peek() == Some(&'=') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator(">=".to_string()));
                    } else {
                        tokens.push(ExpressionToken::Operator(">".to_string()));
                    }
                }
                '<' => {
                    if chars.peek() == Some(&'=') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator("<=".to_string()));
                    } else {
                        tokens.push(ExpressionToken::Operator("<".to_string()));
                    }
                }
                '=' => {
                    if chars.peek() == Some(&'=') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator("==".to_string()));
                    } else {
                        tokens.push(ExpressionToken::Operator("=".to_string()));
                    }
                }
                '!' => {
                    if chars.peek() == Some(&'=') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator("!=".to_string()));
                    } else {
                        return Err(ChtlError::semantic("Invalid operator: !"));
                    }
                }
                '&' => {
                    if chars.peek() == Some(&'&') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator("&&".to_string()));
                    } else {
                        return Err(ChtlError::semantic("Invalid operator: &"));
                    }
                }
                '|' => {
                    if chars.peek() == Some(&'|') {
                        chars.next();
                        tokens.push(ExpressionToken::Operator("||".to_string()));
                    } else {
                        return Err(ChtlError::semantic("Invalid operator: |"));
                    }
                }
                '+' | '-' | '*' | '/' | '%' => {
                    tokens.push(ExpressionToken::Operator(ch.to_string()));
                }
                '(' => tokens.push(ExpressionToken::LeftParen),
                ')' => tokens.push(ExpressionToken::RightParen),
                '?' => tokens.push(ExpressionToken::Question),
                ':' => tokens.push(ExpressionToken::Colon),
                ',' => tokens.push(ExpressionToken::Comma),
                c if c.is_alphabetic() || c == '_' => {
                    let mut identifier = String::from(c);
                    while let Some(&next) = chars.peek() {
                        if next.is_alphanumeric() || next == '_' || next == '-' {
                            identifier.push(chars.next().unwrap());
                        } else {
                            break;
                        }
                    }
                    tokens.push(ExpressionToken::Identifier(identifier));
                }
                c if c.is_numeric() || c == '.' => {
                    let mut number = String::from(c);
                    while let Some(&next) = chars.peek() {
                        if next.is_numeric() || next == '.' {
                            number.push(chars.next().unwrap());
                        } else {
                            break;
                        }
                    }
                    tokens.push(ExpressionToken::Number(number));
                }
                '"' | '\'' => {
                    let quote = ch;
                    let mut string = String::new();
                    while let Some(&next) = chars.peek() {
                        if next == quote {
                            chars.next();
                            break;
                        } else if next == '\\' {
                            chars.next();
                            if let Some(escaped) = chars.next() {
                                string.push(match escaped {
                                    'n' => '\n',
                                    't' => '\t',
                                    'r' => '\r',
                                    '\\' => '\\',
                                    '"' => '"',
                                    '\'' => '\'',
                                    _ => escaped,
                                });
                            }
                        } else {
                            string.push(chars.next().unwrap());
                        }
                    }
                    tokens.push(ExpressionToken::String(string));
                }
                _ => return Err(ChtlError::semantic(&format!("Unexpected character: {}", ch))),
            }
        }
        
        Ok(tokens)
    }
    
    /// Parse condition expression into AST
    fn parse_condition_expression(&self, tokens: &[ExpressionToken]) -> Result<ConditionAst> {
        let mut parser = ConditionParser::new(tokens);
        parser.parse()
    }
    
    /// Evaluate condition AST
    fn evaluate_condition_ast(&self, ast: &ConditionAst) -> Result<String> {
        match ast {
            ConditionAst::Number(n) => Ok(n.clone()),
            ConditionAst::String(s) => Ok(s.clone()),
            ConditionAst::Identifier(id) => {
                // Look up variable in context
                if let Some(value) = self.context.element_properties.get(id) {
                    Ok(value.clone())
                } else if let Some(value) = self.context.parent_properties.get(id) {
                    Ok(value.clone())
                } else {
                    Err(ChtlError::semantic(&format!("Undefined variable: {}", id)))
                }
            }
            ConditionAst::BinaryOp { left, op, right } => {
                let left_val = self.evaluate_condition_ast(left)?;
                let right_val = self.evaluate_condition_ast(right)?;
                self.evaluate_binary_operation(&left_val, op, &right_val)
            }
            ConditionAst::TernaryOp { condition, true_expr, false_expr } => {
                let cond_val = self.evaluate_condition_ast(condition)?;
                let cond_bool = self.parse_boolean(&cond_val)?;
                if cond_bool {
                    self.evaluate_condition_ast(true_expr)
                } else {
                    self.evaluate_condition_ast(false_expr)
                }
            }
            ConditionAst::FunctionCall { name, args } => {
                self.evaluate_function_call(name, args)
            }
        }
    }
    
    /// Evaluate binary operation
    fn evaluate_binary_operation(&self, left: &str, op: &str, right: &str) -> Result<String> {
        match op {
            "+" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num + right_num).to_string())
            }
            "-" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num - right_num).to_string())
            }
            "*" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num * right_num).to_string())
            }
            "/" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                if right_num == 0.0 {
                    return Err(ChtlError::semantic("Division by zero"));
                }
                Ok((left_num / right_num).to_string())
            }
            "%" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                if right_num == 0.0 {
                    return Err(ChtlError::semantic("Modulo by zero"));
                }
                Ok((left_num % right_num).to_string())
            }
            ">" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num > right_num).to_string())
            }
            "<" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num < right_num).to_string())
            }
            ">=" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num >= right_num).to_string())
            }
            "<=" => {
                let left_num = self.parse_number(left)?;
                let right_num = self.parse_number(right)?;
                Ok((left_num <= right_num).to_string())
            }
            "==" => Ok((left == right).to_string()),
            "!=" => Ok((left != right).to_string()),
            "&&" => {
                let left_bool = self.parse_boolean(left)?;
                let right_bool = self.parse_boolean(right)?;
                Ok((left_bool && right_bool).to_string())
            }
            "||" => {
                let left_bool = self.parse_boolean(left)?;
                let right_bool = self.parse_boolean(right)?;
                Ok((left_bool || right_bool).to_string())
            }
            _ => Err(ChtlError::semantic(&format!("Unknown operator: {}", op))),
        }
    }
    
    /// Parse number from string
    fn parse_number(&self, s: &str) -> Result<f64> {
        s.parse::<f64>()
            .map_err(|_| ChtlError::semantic(&format!("Invalid number: {}", s)))
    }
    
    /// Parse boolean from string
    fn parse_boolean(&self, s: &str) -> Result<bool> {
        match s.to_lowercase().as_str() {
            "true" | "1" | "yes" | "on" => Ok(true),
            "false" | "0" | "no" | "off" => Ok(false),
            _ => Err(ChtlError::semantic(&format!("Invalid boolean: {}", s))),
        }
    }
    
    /// Evaluate function call
    fn evaluate_function_call(&self, name: &str, args: &[ConditionAst]) -> Result<String> {
        match name {
            "min" => {
                if args.is_empty() {
                    return Err(ChtlError::semantic("min() requires at least one argument"));
                }
                let mut min_val = f64::INFINITY;
                for arg in args {
                    let val = self.evaluate_condition_ast(arg)?;
                    let num = self.parse_number(&val)?;
                    min_val = min_val.min(num);
                }
                Ok(min_val.to_string())
            }
            "max" => {
                if args.is_empty() {
                    return Err(ChtlError::semantic("max() requires at least one argument"));
                }
                let mut max_val = f64::NEG_INFINITY;
                for arg in args {
                    let val = self.evaluate_condition_ast(arg)?;
                    let num = self.parse_number(&val)?;
                    max_val = max_val.max(num);
                }
                Ok(max_val.to_string())
            }
            "abs" => {
                if args.len() != 1 {
                    return Err(ChtlError::semantic("abs() requires exactly one argument"));
                }
                let val = self.evaluate_condition_ast(&args[0])?;
                let num = self.parse_number(&val)?;
                Ok(num.abs().to_string())
            }
            "round" => {
                if args.len() != 1 {
                    return Err(ChtlError::semantic("round() requires exactly one argument"));
                }
                let val = self.evaluate_condition_ast(&args[0])?;
                let num = self.parse_number(&val)?;
                Ok(num.round().to_string())
            }
            "ceil" => {
                if args.len() != 1 {
                    return Err(ChtlError::semantic("ceil() requires exactly one argument"));
                }
                let val = self.evaluate_condition_ast(&args[0])?;
                let num = self.parse_number(&val)?;
                Ok(num.ceil().to_string())
            }
            "floor" => {
                if args.len() != 1 {
                    return Err(ChtlError::semantic("floor() requires exactly one argument"));
                }
                let val = self.evaluate_condition_ast(&args[0])?;
                let num = self.parse_number(&val)?;
                Ok(num.floor().to_string())
            }
            _ => Err(ChtlError::semantic(&format!("Unknown function: {}", name))),
        }
    }
    
    /// Process selector
    fn process_selector(&self, selector: &StyleSelector) -> Result<StyleRule> {
        match selector {
            StyleSelector::Class(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::Class(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
            StyleSelector::Id(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::Id(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
            StyleSelector::Tag(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::Tag(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
            StyleSelector::PseudoClass(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::PseudoClass(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
            StyleSelector::PseudoElement(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::PseudoElement(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
            StyleSelector::Contextual(name) => {
                Ok(StyleRule {
                    selectors: vec![StyleSelector::Contextual(name.clone())],
                    properties: HashMap::new(),
                    conditions: vec![],
                })
            }
        }
    }
    
    /// Process condition
    fn process_condition(&self, condition: &StyleCondition) -> Result<StyleRule> {
        let evaluated_condition = self.evaluate_condition(&condition.condition)?;
        let condition_bool = self.parse_boolean(&evaluated_condition)?;
        
        if condition_bool {
            Ok(StyleRule {
                selectors: vec![],
                properties: HashMap::from([("condition".to_string(), StyleValue::Simple(condition.value.clone()))]),
                conditions: vec![],
            })
        } else {
            Ok(StyleRule {
                selectors: vec![],
                properties: HashMap::new(),
                conditions: vec![],
            })
        }
    }
    
    /// Process expression
    fn process_expression(&self, expr: &StyleExpression) -> Result<StyleExpression> {
        let mut processed_vars = HashMap::new();
        for (name, value) in &expr.variables {
            let processed_value = self.evaluate_condition(value)?;
            processed_vars.insert(name.clone(), processed_value);
        }
        
        Ok(StyleExpression {
            expression: expr.expression.clone(),
            variables: processed_vars,
            functions: expr.functions.clone(),
        })
    }
}

/// Processed style result
#[derive(Debug, Clone)]
pub struct ProcessedStyle {
    pub rules: Vec<StyleRule>,
    pub variables: HashMap<String, String>,
    pub functions: Vec<StyleFunction>,
}

/// Expression tokens
#[derive(Debug, Clone, PartialEq)]
pub enum ExpressionToken {
    Number(String),
    String(String),
    Identifier(String),
    Operator(String),
    LeftParen,
    RightParen,
    Question,
    Colon,
    Comma,
}

/// Condition AST
#[derive(Debug, Clone)]
pub enum ConditionAst {
    Number(String),
    String(String),
    Identifier(String),
    BinaryOp {
        left: Box<ConditionAst>,
        op: String,
        right: Box<ConditionAst>,
    },
    TernaryOp {
        condition: Box<ConditionAst>,
        true_expr: Box<ConditionAst>,
        false_expr: Box<ConditionAst>,
    },
    FunctionCall {
        name: String,
        args: Vec<ConditionAst>,
    },
}

/// Condition parser
struct ConditionParser {
    tokens: Vec<ExpressionToken>,
    current: usize,
}

impl ConditionParser {
    fn new(tokens: Vec<ExpressionToken>) -> Self {
        Self { tokens, current: 0 }
    }
    
    fn parse(&mut self) -> Result<ConditionAst> {
        self.parse_ternary()
    }
    
    fn parse_ternary(&mut self) -> Result<ConditionAst> {
        let condition = self.parse_logical_or()?;
        
        if self.check(ExpressionToken::Question) {
            self.advance();
            let true_expr = self.parse_ternary()?;
            if !self.check(ExpressionToken::Colon) {
                return Err(ChtlError::semantic("Expected ':' in ternary operator"));
            }
            self.advance();
            let false_expr = self.parse_ternary()?;
            return Ok(ConditionAst::TernaryOp {
                condition: Box::new(condition),
                true_expr: Box::new(true_expr),
                false_expr: Box::new(false_expr),
            });
        }
        
        Ok(condition)
    }
    
    fn parse_logical_or(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_logical_and()?;
        
        while self.check(ExpressionToken::Operator("||".to_string())) {
            self.advance();
            let right = self.parse_logical_and()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op: "||".to_string(),
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_logical_and(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_equality()?;
        
        while self.check(ExpressionToken::Operator("&&".to_string())) {
            self.advance();
            let right = self.parse_equality()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op: "&&".to_string(),
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_equality(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_comparison()?;
        
        while self.check(ExpressionToken::Operator("==".to_string())) || 
              self.check(ExpressionToken::Operator("!=".to_string())) {
            let op = if self.check(ExpressionToken::Operator("==".to_string())) {
                "==".to_string()
            } else {
                "!=".to_string()
            };
            self.advance();
            let right = self.parse_comparison()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op,
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_comparison(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_addition()?;
        
        while self.check(ExpressionToken::Operator(">".to_string())) ||
              self.check(ExpressionToken::Operator(">=".to_string())) ||
              self.check(ExpressionToken::Operator("<".to_string())) ||
              self.check(ExpressionToken::Operator("<=".to_string())) {
            let op = match self.peek().unwrap() {
                ExpressionToken::Operator(op) => op.clone(),
                _ => unreachable!(),
            };
            self.advance();
            let right = self.parse_addition()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op,
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_addition(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_multiplication()?;
        
        while self.check(ExpressionToken::Operator("+".to_string())) ||
              self.check(ExpressionToken::Operator("-".to_string())) {
            let op = match self.peek().unwrap() {
                ExpressionToken::Operator(op) => op.clone(),
                _ => unreachable!(),
            };
            self.advance();
            let right = self.parse_multiplication()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op,
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_multiplication(&mut self) -> Result<ConditionAst> {
        let mut left = self.parse_unary()?;
        
        while self.check(ExpressionToken::Operator("*".to_string())) ||
              self.check(ExpressionToken::Operator("/".to_string())) ||
              self.check(ExpressionToken::Operator("%".to_string())) {
            let op = match self.peek().unwrap() {
                ExpressionToken::Operator(op) => op.clone(),
                _ => unreachable!(),
            };
            self.advance();
            let right = self.parse_unary()?;
            left = ConditionAst::BinaryOp {
                left: Box::new(left),
                op,
                right: Box::new(right),
            };
        }
        
        Ok(left)
    }
    
    fn parse_unary(&mut self) -> Result<ConditionAst> {
        if self.check(ExpressionToken::Operator("-".to_string())) {
            self.advance();
            let right = self.parse_primary()?;
            return Ok(ConditionAst::BinaryOp {
                left: Box::new(ConditionAst::Number("0".to_string())),
                op: "-".to_string(),
                right: Box::new(right),
            });
        }
        
        self.parse_primary()
    }
    
    fn parse_primary(&mut self) -> Result<ConditionAst> {
        match self.peek() {
            Some(ExpressionToken::Number(n)) => {
                self.advance();
                Ok(ConditionAst::Number(n.clone()))
            }
            Some(ExpressionToken::String(s)) => {
                self.advance();
                Ok(ConditionAst::String(s.clone()))
            }
            Some(ExpressionToken::Identifier(id)) => {
                self.advance();
                if self.check(ExpressionToken::LeftParen) {
                    self.advance();
                    let mut args = Vec::new();
                    if !self.check(ExpressionToken::RightParen) {
                        loop {
                            args.push(self.parse_ternary()?);
                            if self.check(ExpressionToken::Comma) {
                                self.advance();
                            } else {
                                break;
                            }
                        }
                    }
                    if !self.check(ExpressionToken::RightParen) {
                        return Err(ChtlError::semantic("Expected ')' after function arguments"));
                    }
                    self.advance();
                    Ok(ConditionAst::FunctionCall {
                        name: id.clone(),
                        args,
                    })
                } else {
                    Ok(ConditionAst::Identifier(id.clone()))
                }
            }
            Some(ExpressionToken::LeftParen) => {
                self.advance();
                let expr = self.parse_ternary()?;
                if !self.check(ExpressionToken::RightParen) {
                    return Err(ChtlError::semantic("Expected ')' after expression"));
                }
                self.advance();
                Ok(expr)
            }
            _ => Err(ChtlError::semantic("Expected expression")),
        }
    }
    
    fn check(&self, token: ExpressionToken) -> bool {
        if self.current >= self.tokens.len() {
            false
        } else {
            self.tokens[self.current] == token
        }
    }
    
    fn advance(&mut self) {
        if self.current < self.tokens.len() {
            self.current += 1;
        }
    }
    
    fn peek(&self) -> Option<&ExpressionToken> {
        if self.current < self.tokens.len() {
            Some(&self.tokens[self.current])
        } else {
            None
        }
    }
}