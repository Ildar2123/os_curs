#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>

#define MAX_LEN 1024 * 1024
#define EXIT "exit"

using namespace std;

int chating(int socketFD);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Слишком мало аргументов\n";
        exit(1);
    }

    int port = atoi(argv[1]);

    int socketFD;
    sockaddr_in serverAddr;

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1)
    {
        cout << "Socket creation faild!\n";
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    if (connect(socketFD, (sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        cout << "Cannot connect to the server!\n";
        exit(1);
    }

    chating(socketFD);

    close(socketFD);
    return 0;
}

int chating(int socketFD)
{
    char *recBuff;
    string sendBuff;

    while (1)
    {
        int fileDesc;
        recBuff = new char[MAX_LEN];

        cout << "Enter message: ";
        getline(cin, sendBuff);

        if (write(socketFD, sendBuff.c_str(), sendBuff.length()) < 1)
        {
            cout << "Message send error!\n";
            break;
        }

        if (recv(socketFD, recBuff, MAX_LEN, 0) < 1)
        {
            cout << "Message recieve error!\n";
            break;
        }

        if (strcmp(sendBuff.c_str(), EXIT) == 0)
            break;

        cout << recBuff << endl;

        delete recBuff;
        sendBuff.clear();
    }

    return 0;
}