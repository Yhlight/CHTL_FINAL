package CHTL.CHTLManage;

import CHTL.CHTLNode.ConfigurationNode;
import CHTL.CHTLLexer.TokenType;

import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

public class ConfigurationManager {

    private final Map<String, String> keywordRenames = new HashMap<>();

    public void processConfigNode(ConfigurationNode node) {
        keywordRenames.putAll(node.getSettings());
    }

    public void setKeyword(String key, String value) {
        keywordRenames.put(key, value);
    }

    public Map<String, TokenType> getActiveKeywords(Map<String, TokenType> defaultKeywords) {
        if (keywordRenames.isEmpty()) {
            return defaultKeywords;
        }

        Map<String, TokenType> activeKeywords = new HashMap<>(defaultKeywords);

        Map<TokenType, String> defaultTokenToString = defaultKeywords.entrySet()
            .stream()
            .collect(Collectors.toMap(Map.Entry::getValue, Map.Entry::getKey, (v1, v2) -> v1));

        for (Map.Entry<String, String> entry : keywordRenames.entrySet()) {
            String key = entry.getKey();
            String newKeyword = entry.getValue();
            String tokenTypeName = key.replace("KEYWORD_", "");
             try {
                TokenType tokenType = TokenType.valueOf(tokenTypeName);
                String oldKeyword = defaultTokenToString.get(tokenType);
                if (oldKeyword != null) {
                    activeKeywords.remove(oldKeyword);
                }
                activeKeywords.put(newKeyword, tokenType);
            } catch (IllegalArgumentException e) {
                // Ignore invalid keys
            }
        }

        return activeKeywords;
    }
}
