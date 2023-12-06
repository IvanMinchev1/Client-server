#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

std::mutex mutex;
std::condition_variable cheks;

std::vector<std::vector<int>> graph;
std::vector<bool> visited;

void bfs(int start) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::queue<int> q;
    q.push(start);

    while (!q.empty()) {
        int cur = q.front();
        q.pop();

        std::unique_lock<std::mutex> lock(mutex);
        if (!visited[cur]) {
            std::cout << "Visiting node: " << cur << std::endl;
            visited[cur] = true;

            for (int neighbor : graph[cur]) {
                if (!visited[neighbor]) {
                    q.push(neighbor);
                }
            }
        }
        lock.unlock();
        cheks.notify_all();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Thread " << std::this_thread::get_id() << " execution time: " << duration.count() << " ms" << std::endl;
}

int main() {
    graph = {{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    visited = std::vector<bool>(graph.size(), false);
    int serverSocket, newSocket, port = 8080;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket\n";
        return -1;
    }
    listen(serverSocket, 5);
    std::cout << "Server waiting for client on port " << port << std::endl;
    newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (newSocket == -1) {
        std::cerr << "Error accepting connection\n";
        return -1;
    }
    
    
    std::vector<std::thread> threads;
    for (int i = 0; i < graph.size(); ++i) {
        threads.emplace_back(bfs, i);
    }
    for (auto &thread : threads) {
        thread.join();
    }
    close(newSocket);
    close(serverSocket);
    return 0;
}


