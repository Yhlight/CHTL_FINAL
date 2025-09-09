use crate::compiler_dispatcher::dispatcher::CompilationOutput;

pub struct CodeMerger;

impl CodeMerger {
    pub fn new() -> Self {
        CodeMerger
    }

    pub fn merge(&self, output: CompilationOutput) -> String {
        println!("--- Code Merger received output: ---");
        println!("HTML length: {}", output.html.len());
        println!("CSS length: {}", output.css.len());
        println!("JS length: {}", output.js.len());

        // This is a placeholder for the real merging logic.
        // It would intelligently combine the HTML, CSS, and JS.
        // For example, putting the CSS in a <style> tag in the <head>
        // and the JS in a <script> tag at the end of the <body>.

        // For now, we'll just concatenate them.
        format!("{}\n<style>{}</style>\n<script>{}</script>", output.html, output.css, output.js)
    }
}
