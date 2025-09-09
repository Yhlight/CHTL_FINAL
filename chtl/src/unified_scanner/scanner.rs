#[derive(Debug, PartialEq, Clone)]
pub enum SourceFragment {
    Chtl(String),
    ChtlJs(String),
    Css(String),
    Js(String),
}

pub struct UnifiedScanner<'a> {
    input: &'a str,
}

impl<'a> UnifiedScanner<'a> {
    pub fn new(input: &'a str) -> Self {
        UnifiedScanner { input }
    }

    pub fn scan(&self) -> Vec<SourceFragment> {
        let mut fragments = Vec::new();
        let mut last_pos = 0;
        let mut main_scanner = self.input;

        while let Some(script_pos) = main_scanner.find("script") {
            // Check if it's followed by a brace
            if main_scanner[script_pos..].split_whitespace().next() == Some("script") {
                 if let Some(brace_pos) = main_scanner[script_pos..].find('{') {
                    // Everything before "script" is CHTL
                    let chtl_part = &main_scanner[..script_pos];
                    if !chtl_part.trim().is_empty() {
                        fragments.push(SourceFragment::Chtl(chtl_part.to_string()));
                    }

                    // Find the matching closing brace
                    let mut brace_level = 0;
                    let mut end_brace_pos = 0;
                    for (i, c) in main_scanner[script_pos + brace_pos..].char_indices() {
                        if c == '{' {
                            brace_level += 1;
                        } else if c == '}' {
                            brace_level -= 1;
                            if brace_level == 0 {
                                end_brace_pos = script_pos + brace_pos + i;
                                break;
                            }
                        }
                    }

                    if end_brace_pos > 0 {
                        // The content inside the braces is CHTL JS
                        let js_part = &main_scanner[script_pos + brace_pos + 1..end_brace_pos];
                        fragments.push(SourceFragment::ChtlJs(js_part.to_string()));

                        // Move scanner past this block
                        main_scanner = &main_scanner[end_brace_pos + 1..];
                        last_pos = end_brace_pos + 1;
                    } else {
                        // Unmatched brace, treat rest as CHTL
                        break;
                    }
                 } else {
                     // "script" not followed by "{", treat as normal CHTL
                     break;
                 }
            } else {
                // "script" is part of another word, advance past it
                main_scanner = &main_scanner[script_pos + 1..];
            }
        }

        // Add the final remaining part as CHTL
        if !main_scanner.is_empty() {
             fragments.push(SourceFragment::Chtl(main_scanner.to_string()));
        }

        fragments
    }
}
