use std::collections::HashMap;
use crate::ast::{TemplateElementGroup, TemplateStyleGroup, TemplateVarGroup};

use crate::ast::{CustomElementGroup, CustomStyleGroup, CustomVarGroup};

#[derive(Debug, Default)]
pub struct Context<'a> {
    pub style_templates: HashMap<&'a str, &'a TemplateStyleGroup<'a>>,
    pub element_templates: HashMap<&'a str, &'a TemplateElementGroup<'a>>,
    pub var_templates: HashMap<&'a str, &'a TemplateVarGroup<'a>>,

    pub custom_style_templates: HashMap<&'a str, &'a CustomStyleGroup<'a>>,
    pub custom_element_templates: HashMap<&'a str, &'a CustomElementGroup<'a>>,
    pub custom_var_templates: HashMap<&'a str, &'a CustomVarGroup<'a>>,
}

impl<'a> Context<'a> {
    pub fn new() -> Self {
        Self::default()
    }
}
