package com.chtholly.chthl.ast;

import java.util.Map;

public class ConfigurationNode implements Node {
    public final Map<String, Object> settings;

    public ConfigurationNode(Map<String, Object> settings) {
        this.settings = settings;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        // Configuration nodes are typically processed by the compiler driver
        // before the main generation visitor runs.
        return visitor.visitConfigurationNode(this);
    }

    @Override
    public Node clone() {
        // Values are expected to be immutable (String, Boolean, Integer)
        return new ConfigurationNode(new java.util.HashMap<>(settings));
    }
}
