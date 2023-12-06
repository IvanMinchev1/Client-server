#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

std::mutex mtx;

class Graph {
public:
    int vertices;
    std::vector<std::vector<int>> adjList;

    Graph(int V) : vertices(V), adjList(V) {}

    void addEdge(int u, int v) {
        adjList[u].push_back(v);
    }
};

void parallelBFS(Graph& graph, int start, std::vector<bool>& visited, std::vector<int>& distances) {
    std::queue<int> q;
    q.push(start);
    visited[start] = true;
    distances[start] = 0;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        // Process the current node (e.g., print it or perform other operations)

        {
            std::lock_guard<std::mutex> lock(mtx);  // Lock to avoid race conditions
            std::cout << "Visiting node: " << current << std::endl;
        }

        for (int neighbor : graph.adjList[current]) {
            if (!visited[neighbor]) {
                q.push(neighbor);
                visited[neighbor] = true;
                distances[neighbor] = distances[current] + 1;
            }
        }
    }
}

int main() {
    // Create a sample graph
    Graph graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 5);

    const int numThreads = 2;  // Adjust the number of threads as needed

    std::vector<std::thread> threads;
    std::vector<bool> visited(graph.vertices, false);
    std::vector<int> distances(graph.vertices, -1);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(parallelBFS, std::ref(graph), i, std::ref(visited), std::ref(distances));
    }

    for (std::thread& t : threads) {
        t.join();
    }

    return 0;
}

