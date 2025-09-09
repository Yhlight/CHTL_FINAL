package JS;

import Scanner.SourceFragment;

// Placeholder for the JS Compiler. In a real implementation, this would
// parse, validate, and potentially optimize/transpile the JS. For now, it's a pass-through.
public class JsCompiler {
    public String compile(SourceFragment fragment) {
        if (fragment.type() != Scanner.FragmentType.GLOBAL_JS) {
            throw new IllegalArgumentException("JsCompiler can only handle GLOBAL_JS fragments.");
        }
        return fragment.content();
    }
}
