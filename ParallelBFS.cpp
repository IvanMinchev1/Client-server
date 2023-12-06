#include <iostream>
#include <vector>
#include <queue>
#include <execution>

class G 
{
private:
    int num;
    std::vector<std::vector<int>> nbr;
public:
    G(int vertices) : num(vertices), nbr(vertices) {}

    void addEdge(int v, int w) {
        nbr[v].push_back(w);
    }
    void parallelBFS(int head) {
        std::vector<bool> visited(num, false);
        std::queue<int> q;

        visited[head] = true;
        q.push(head);

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            std::cout << current << "->";

            std::for_each(std::execution::par, nbr[current].begin(), nbr[current].end(),
                [&](int neighbor) {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                });
        }
        std::cout<<std::endl;
    }
};

int main() 
{
    G graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 5);

    graph.parallelBFS(0);
    return 0;
}
