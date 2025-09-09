package Util;

import java.util.*;

/**
 * A generic dependency graph that can be used to topologically sort nodes.
 * @param <T> The type of the nodes in the graph.
 */
public class DependencyGraph<T> {
    private final Map<T, Set<T>> adj = new HashMap<>();

    public void addNode(T node) {
        adj.putIfAbsent(node, new HashSet<>());
    }

    public void addEdge(T from, T to) {
        addNode(from);
        addNode(to);
        adj.get(from).add(to);
    }

    public List<T> topologicalSort() throws IllegalStateException {
        List<T> sorted = new ArrayList<>();
        Map<T, Integer> inDegree = new HashMap<>();

        for (T node : adj.keySet()) {
            inDegree.put(node, 0);
        }

        for (Set<T> neighbors : adj.values()) {
            for (T neighbor : neighbors) {
                inDegree.put(neighbor, inDegree.get(neighbor) + 1);
            }
        }

        Queue<T> queue = new LinkedList<>();
        for (Map.Entry<T, Integer> entry : inDegree.entrySet()) {
            if (entry.getValue() == 0) {
                queue.add(entry.getKey());
            }
        }

        while (!queue.isEmpty()) {
            T node = queue.poll();
            sorted.add(node);

            if (adj.containsKey(node)) {
                for (T neighbor : adj.get(node)) {
                    inDegree.put(neighbor, inDegree.get(neighbor) - 1);
                    if (inDegree.get(neighbor) == 0) {
                        queue.add(neighbor);
                    }
                }
            }
        }

        if (sorted.size() != adj.size()) {
            throw new IllegalStateException("Cycle detected in dependency graph.");
        }

        return sorted;
    }
}
