package CHTL_JS.Node;

import java.util.Map;

// Represents a CHTL JS `router { ... }` block.
public class RouterNode implements CHTLJSBaseNode {
    private final Map<String, String> routes;

    public RouterNode() {
        this.routes = new java.util.HashMap<>();
    }

    public void addRoute(String url, String pageSelector) {
        this.routes.put(url, pageSelector);
    }

    public Map<String, String> getRoutes() { return routes; }
}
