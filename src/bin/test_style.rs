use chtl::{StyleBlock, CSSValue, StyleSystem, ContextRule, AttributeCondition, AttributeOperator};

fn main() {
    // Create a style system
    let mut style_system = StyleSystem::new();
    
    // Create a style block
    let mut style_block = StyleBlock::new();
    style_block.set_class_name("my-button".to_string());
    
    // Add some properties
    style_block.add_property("background-color".to_string(), CSSValue::Literal("blue".to_string()));
    style_block.add_property("color".to_string(), CSSValue::Literal("white".to_string()));
    style_block.add_property("padding".to_string(), CSSValue::Literal("10px".to_string()));
    
    // Add a context rule for hover
    let mut hover_rule = ContextRule {
        selector: "hover".to_string(),
        properties: vec![
            ("background-color".to_string(), CSSValue::Literal("darkblue".to_string())),
        ],
        priority: 1,
    };
    style_block.add_context_rule(hover_rule);
    
    // Add an attribute condition
    let mut disabled_condition = AttributeCondition {
        attribute: "disabled".to_string(),
        operator: AttributeOperator::Equals,
        value: "true".to_string(),
        properties: vec![
            ("opacity".to_string(), CSSValue::Literal("0.5".to_string())),
            ("cursor".to_string(), CSSValue::Literal("not-allowed".to_string())),
        ],
    };
    style_block.add_attribute_condition(disabled_condition);
    
    // Add the style block to the system
    style_system.add_style_block("my-button".to_string(), style_block);
    
    // Generate CSS
    let css = style_system.to_css();
    println!("Generated CSS:");
    println!("{}", css);
}