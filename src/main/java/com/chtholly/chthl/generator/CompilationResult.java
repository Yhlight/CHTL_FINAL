package com.chtholly.chthl.generator;

/**
 * A data class to hold the results of the compilation process,
 * separating the generated HTML and CSS.
 */
public class CompilationResult {
    private final String html;
    private final String css;

    public CompilationResult(String html, String css) {
        this.html = html;
        this.css = css;
    }

    public String getHtml() {
        return html;
    }

    public String getCss() {
        return css;
    }
}
