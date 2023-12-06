#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void Response(const char* response) {
    std::cout << "Received from server: " << response << std::endl;
}
int main()
{
    int clientSocket, port = 8080;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server\n";
        return -1;
    }
    const char* request = "Requesting BFS data";
    send(clientSocket, request, strlen(request), 0);
    
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    Response(buffer);

    close(clientSocket);
    return 0;
}

