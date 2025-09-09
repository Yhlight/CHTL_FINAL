use crate::ast::{CustomElementGroup, CustomStyleGroup, CustomVarGroup};
use crate::ast::{TemplateElementGroup, TemplateStyleGroup, TemplateVarGroup};
use std::cell::RefCell;
use std::collections::HashMap;

#[derive(Debug, Default)]
pub struct Context<'a> {
    // Template definitions
    pub style_templates: HashMap<&'a str, &'a TemplateStyleGroup<'a>>,
    pub element_templates: HashMap<&'a str, &'a TemplateElementGroup<'a>>,
    pub var_templates: HashMap<&'a str, &'a TemplateVarGroup<'a>>,
    pub custom_style_templates: HashMap<&'a str, &'a CustomStyleGroup<'a>>,
    pub custom_element_templates: HashMap<&'a str, &'a CustomElementGroup<'a>>,
    pub custom_var_templates: HashMap<&'a str, &'a CustomVarGroup<'a>>,

    // Compilation state
    pub hoisted_styles: RefCell<String>,
    pub unique_class_counter: RefCell<u32>,
}

impl<'a> Context<'a> {
    pub fn new() -> Self {
        Self::default()
    }
}
