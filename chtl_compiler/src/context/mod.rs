use std::collections::HashMap;
use crate::ast::{TemplateElementGroup, TemplateStyleGroup, TemplateVarGroup};

#[derive(Debug, Default)]
pub struct Context<'a> {
    pub style_templates: HashMap<&'a str, &'a TemplateStyleGroup<'a>>,
    pub element_templates: HashMap<&'a str, &'a TemplateElementGroup<'a>>,
    pub var_templates: HashMap<&'a str, &'a TemplateVarGroup<'a>>,
}

impl<'a> Context<'a> {
    pub fn new() -> Self {
        Self::default()
    }
}
