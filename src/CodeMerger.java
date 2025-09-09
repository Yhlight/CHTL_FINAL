import CHTL.Node.BaseNode;
import java.util.List;

import CHTL.Generator.CHTLGenerator;

// Merges the outputs of the different compilers into a single HTML file.
public class CodeMerger {

    /**
     * @param htmlAst The final AST of the HTML structure from the CHTL compiler.
     * @param cssStrings A list of all CSS code strings to be included.
     * @param jsStrings A list of all JavaScript code strings to be included.
     * @return The final, complete HTML string.
     */
    public String merge(List<BaseNode> htmlAst, List<String> cssStrings, List<String> jsStrings) {
        CHTLGenerator generator = new CHTLGenerator();
        String htmlBody = generator.generate(htmlAst);

        StringBuilder headContent = new StringBuilder();

        if (cssStrings != null && !cssStrings.isEmpty()) {
            headContent.append("<style>\n");
            for (String css : cssStrings) {
                headContent.append(css).append("\n");
            }
            headContent.append("</style>\n");
        }

        if (jsStrings != null && !jsStrings.isEmpty()) {
            for (String js : jsStrings) {
                headContent.append("<script>\n");
                headContent.append(js).append("\n");
                headContent.append("</script>\n");
            }
        }

        // Check if the generated HTML already has a structure
        if (htmlBody.contains("<head>")) {
            return htmlBody.replace("</head>", headContent.toString() + "</head>");
        } else {
            // If not, wrap it
            return "<html>\n<head>\n" + headContent.toString() + "</head>\n<body>\n" + htmlBody + "\n</body>\n</html>";
        }
    }
}
