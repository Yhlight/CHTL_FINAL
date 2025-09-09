package CHTL.CHTLNode;

import java.util.HashMap;
import java.util.Map;

/**
 * Represents a [Configuration] block in the AST.
 * It holds key-value pairs of configuration settings.
 */
public class ConfigurationNode implements BaseNode {
    // For now, we'll store settings as simple key-value strings.
    // A more complex implementation could have structured data.
    private final Map<String, String> settings = new HashMap<>();
    private final String name; // for named configs: [Configuration] @Config MyConfig

    public ConfigurationNode(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void addSetting(String key, String value) {
        settings.put(key, value);
    }

    public Map<String, String> getSettings() {
        return settings;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitConfigurationNode(this);
    }

    @Override
    public BaseNode clone() {
        ConfigurationNode cloned = new ConfigurationNode(this.name);
        this.settings.forEach(cloned::addSetting);
        return cloned;
    }
}
