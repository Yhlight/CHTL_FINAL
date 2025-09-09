pub struct UnifiedScanner<'a> {
    input: &'a str,
    position: usize,
}

impl<'a> UnifiedScanner<'a> {
    pub fn new(input: &'a str) -> Self {
        UnifiedScanner { input, position: 0 }
    }

    // This is a simplified first pass. A real implementation would be a state machine.
    pub fn scan_for_scripts(&mut self) -> Vec<String> {
        let mut scripts = Vec::new();
        let chars: Vec<char> = self.input.chars().collect();

        while self.position < chars.len() {
            // Find the next occurrence of "script"
            if let Some(i) = self.input[self.position..].find("script") {
                self.position += i;

                let mut open_brace_pos: Option<usize> = None;

                // Look for the opening brace after "script"
                let mut temp_pos = self.position + "script".len();
                while temp_pos < chars.len() {
                    if chars[temp_pos].is_whitespace() {
                        temp_pos += 1;
                        continue;
                    }
                    if chars[temp_pos] == '{' {
                        open_brace_pos = Some(temp_pos);
                    }
                    break;
                }

                if let Some(start) = open_brace_pos {
                    let mut brace_level = 1;
                    let mut content_start = start + 1;
                    let mut content_end = start;

                    let mut search_pos = content_start;
                    while search_pos < chars.len() {
                        if chars[search_pos] == '{' {
                            brace_level += 1;
                        } else if chars[search_pos] == '}' {
                            brace_level -= 1;
                            if brace_level == 0 {
                                content_end = search_pos;
                                break;
                            }
                        }
                        search_pos += 1;
                    }

                    if content_end > content_start {
                        let script_content = self.input[content_start..content_end].to_string();
                        scripts.push(script_content);
                        self.position = content_end + 1;
                    } else {
                        self.position = start + 1;
                    }

                } else {
                    // No brace found after "script", move on
                    self.position += "script".len();
                }
            } else {
                // No more occurrences of "script"
                break;
            }
        }

        scripts
    }
}
