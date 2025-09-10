package com.chtholly.chthl;

import com.chtholly.chthl.ast.ConfigurationNode;
import java.util.Map;

public class CHTLConfig {
    public int indexInitialCount = 0;
    public boolean debugMode = false;
    // Add other config fields here as they are implemented

    public CHTLConfig() {
        // Default values are set
    }

    public void load(ConfigurationNode configNode) {
        if (configNode == null) return;
        Map<String, Object> settings = configNode.settings;

        if (settings.containsKey("INDEX_INITIAL_COUNT")) {
            Object value = settings.get("INDEX_INITIAL_COUNT");
            if (value instanceof Integer) {
                this.indexInitialCount = (Integer) value;
            }
        }
        if (settings.containsKey("DEBUG_MODE")) {
            Object value = settings.get("DEBUG_MODE");
            if (value instanceof Boolean) {
                this.debugMode = (Boolean) value;
            }
        }
    }
}
