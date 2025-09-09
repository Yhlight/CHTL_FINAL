package CSS;

import Scanner.SourceFragment;

// Placeholder for the CSS Compiler. In a real implementation, this would
// parse, validate, and potentially optimize the CSS. For now, it's a pass-through.
public class CssCompiler {
    public String compile(SourceFragment fragment) {
        if (fragment.type() != Scanner.FragmentType.GLOBAL_CSS) {
            throw new IllegalArgumentException("CssCompiler can only handle GLOBAL_CSS fragments.");
        }
        return fragment.content();
    }
}
