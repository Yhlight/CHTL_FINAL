package CHTL_JS.Node;

import java.util.Map;
import java.util.List;

// Represents a CHTL JS `animate { ... }` block.
public class AnimateNode implements CHTLJSBaseNode {
    // This is a simplified representation. A full implementation would
    // have structured fields for duration, easing, keyframes (when), etc.
    private final Map<String, String> animationProperties;

    public AnimateNode() {
        this.animationProperties = new java.util.HashMap<>();
    }

    public void addProperty(String key, String value) {
        this.animationProperties.put(key, value);
    }

    public Map<String, String> getProperties() {
        return animationProperties;
    }
}
