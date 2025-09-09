use chtl::{TemplateSystem, StyleGroupTemplate, ElementTemplate, VariableGroupTemplate, TemplateParameter, ParameterType, CSSValue, StyleBlock, ElementNode, TextNode, TextType, Node};
use std::collections::HashMap;

fn main() {
    // Create a template system
    let mut template_system = TemplateSystem::new();
    
    // Create a style group template
    let mut style_template = StyleGroupTemplate {
        name: "button-style".to_string(),
        base_style: {
            let mut style = StyleBlock::new();
            style.set_class_name("btn".to_string());
            style.add_property("padding".to_string(), CSSValue::Literal("10px".to_string()));
            style.add_property("border-radius".to_string(), CSSValue::Literal("4px".to_string()));
            style.add_property("background-color".to_string(), CSSValue::Variable("primary-color".to_string()));
            style.add_property("color".to_string(), CSSValue::Variable("text-color".to_string()));
            style
        },
        variables: HashMap::new(),
        parameters: vec![
            TemplateParameter {
                name: "primary-color".to_string(),
                param_type: ParameterType::Color,
                default_value: Some(CSSValue::Literal("blue".to_string())),
                required: false,
            },
            TemplateParameter {
                name: "text-color".to_string(),
                param_type: ParameterType::Color,
                default_value: Some(CSSValue::Literal("white".to_string())),
                required: false,
            },
        ],
        location: Default::default(),
    };
    
    // Add the style template
    template_system.add_style_template(style_template);
    
    // Create an element template
    let element_template = ElementTemplate {
        name: "button-element".to_string(),
        base_element: ElementNode {
            tag: "button".to_string(),
            attributes: HashMap::new(),
            children: vec![
                Node::Text(TextNode::new("${button-text}".to_string(), TextType::Literal)),
            ],
            self_closing: false,
            location: Default::default(),
        },
        variables: HashMap::new(),
        parameters: vec![
            TemplateParameter {
                name: "button-text".to_string(),
                param_type: ParameterType::String,
                default_value: Some(CSSValue::Literal("Click me".to_string())),
                required: true,
            },
        ],
        location: Default::default(),
    };
    
    // Add the element template
    template_system.add_element_template(element_template);
    
    // Create a variable group template
    let variable_template = VariableGroupTemplate {
        name: "theme-variables".to_string(),
        variables: HashMap::new(),
        parameters: vec![
            TemplateParameter {
                name: "primary-color".to_string(),
                param_type: ParameterType::Color,
                default_value: Some(CSSValue::Literal("#007bff".to_string())),
                required: false,
            },
            TemplateParameter {
                name: "secondary-color".to_string(),
                param_type: ParameterType::Color,
                default_value: Some(CSSValue::Literal("#6c757d".to_string())),
                required: false,
            },
        ],
        location: Default::default(),
    };
    
    // Add the variable template
    template_system.add_variable_template(variable_template);
    
    // Test style template instance
    let mut style_params = HashMap::new();
    style_params.insert("primary-color".to_string(), CSSValue::Literal("red".to_string()));
    style_params.insert("text-color".to_string(), CSSValue::Literal("white".to_string()));
    
    match template_system.create_instance("button-style", style_params) {
        Ok(instance) => {
            println!("Style template instance created successfully!");
            println!("Template: {}", instance.template_name);
            println!("Resolved content: {:?}", instance.resolved_content);
        }
        Err(e) => {
            println!("Error creating style template instance: {:?}", e);
        }
    }
    
    // Test element template instance
    let mut element_params = HashMap::new();
    element_params.insert("button-text".to_string(), CSSValue::Literal("Submit".to_string()));
    
    match template_system.create_instance("button-element", element_params) {
        Ok(instance) => {
            println!("\nElement template instance created successfully!");
            println!("Template: {}", instance.template_name);
            println!("Resolved content: {:?}", instance.resolved_content);
        }
        Err(e) => {
            println!("Error creating element template instance: {:?}", e);
        }
    }
    
    // Test variable template instance
    let mut variable_params = HashMap::new();
    variable_params.insert("primary-color".to_string(), CSSValue::Literal("#28a745".to_string()));
    
    match template_system.create_instance("theme-variables", variable_params) {
        Ok(instance) => {
            println!("\nVariable template instance created successfully!");
            println!("Template: {}", instance.template_name);
            println!("Resolved content: {:?}", instance.resolved_content);
        }
        Err(e) => {
            println!("Error creating variable template instance: {:?}", e);
        }
    }
}