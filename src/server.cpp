#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
#include "FileManager.h"

using namespace std;

#define MAX_LEN 1024 * 1024
#define COMMAND_LEN 512
#define EXIT "exit"
#define INFO "/home/aprekek/Study/5_sem/OS/OS/Lab_4/info.txt"

struct handlerData
{
    int socketFD;
    char *pathToProgramm;
};

// struct executeData
// {
//     char *buffer;
//     char *pathToFile;
// };

int chating(int socketFD, char *pathToProgramm);
void *connectionHandler(void *_data);
// void *executeHandler(void *_data);
char **getArgumentForChildProgramm(char *progName, const string &agruments, int &argc);

int main(int argc, char *argv[])
{
    if (argc < 3)
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
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if ((bind(socketFD, (sockaddr *)&serverAddr, sizeof(serverAddr))) != 0)
    {
        cout << "Socket bind faild!\n";
        exit(1);
    }

    if (listen(socketFD, 10) != 0)
    {
        cout << "Listen failed!\n";
        exit(1);
    }

    cout << "Waiting for connection...\n";

    chating(socketFD, argv[2]);

    close(socketFD);
    return 0;
}

int chating(int socketFD, char *pathToProgramm)
{

    int newSocketFD;
    int addrLenght = sizeof(sockaddr_in);

    handlerData *data;
    // vector<pthread_t> threadsIDs;

    while ((newSocketFD = accept(socketFD, nullptr, (socklen_t *)&addrLenght)))
    {
        cout << "Connection accepted!\n";

        pthread_t threadId;
        data = new handlerData;

        data->socketFD = newSocketFD;
        data->pathToProgramm = pathToProgramm;

        if (pthread_create(&threadId, nullptr, connectionHandler, (void *)data) < 0)
            perror("Cannot create thread!\n");

        pthread_detach(threadId);
        // threadsIDs.push_back(threadId);
    }

    // for (size_t i = 0; i < threadsIDs.size(); ++i)
    //     pthread_join(threadsIDs[i], nullptr);

    return 0;
}

void *connectionHandler(void *_data)
{
    char *command = new char[COMMAND_LEN];
    char *tempBuff = new char[MAX_LEN];
    string respBuff;

    string errorMSG = "Cannot open file\n";
    string sucsessMSG = "Srote sucess\n";
    string closeMessage = "Server closed!\n";

    handlerData data = *(handlerData *)_data;
    char *path = (char *)data.pathToProgramm;

    int newSocketFD = data.socketFD;
    int rSize;

    int argc;
    char **argv;
    // executeData *exeData;

    int infoFD;

    while ((rSize = read(newSocketFD, command, COMMAND_LEN)) > 0)
    {
        if (strcmp(command, EXIT) == 0)
        {
            if (write(newSocketFD, closeMessage.c_str(), closeMessage.length()) < 1)
                cout << "Send failed!\n";
            break;
        }

        infoFD = open(INFO, O_RDWR | O_TRUNC | O_APPEND);
        if (infoFD == -1)
            if (write(newSocketFD, errorMSG.c_str(), errorMSG.length()) < 1)
            {
                cout << "Send failed!\n";
                exit(1);
            }

        dup2(infoFD, STDOUT_FILENO);

        argv = getArgumentForChildProgramm(path, command, argc);
        executeRequest(argc, argv);

        fflush(nullptr);
        close(infoFD);

        infoFD = open(INFO, O_RDWR);
        if (infoFD == -1)
            if (write(newSocketFD, errorMSG.c_str(), errorMSG.length()) < 1)
            {
                cout << "Send failed!\n";
                exit(1);
            }

        FILE *f = fdopen(infoFD, "r");
        if (f == nullptr)
        {
            cout << "Cannot open file to store response!\n";
            break;
        }

        while (fgets(tempBuff, MAX_LEN, f))
        {
            respBuff += tempBuff;
        }
        close(infoFD);

        send(newSocketFD, respBuff.c_str(), respBuff.length(), 0);

        fflush(nullptr);

        dup2(0, STDOUT_FILENO);

        // pthread_t thrId;
        // exeData = new executeData;
        // exeData->buffer = buff;
        // exeData->pathToFile = path;

        // if (pthread_create(&thrId, nullptr, executeHandler, (void *)exeData) < 0)
        //     perror("Cannot create thread!\n");

        // pthread_join(thrId, nullptr);
        // delete exeData;

        bzero(command, COMMAND_LEN);
        respBuff.clear();
    }

    if (rSize > -1)
        cout << "Client disconected!\n";
    else
        cout << "Receve failed!\n";

    delete (handlerData *)_data;

    return 0;
}

// void *executeHandler(void *_data)
// {
//     executeData *data = (executeData *)_data;

//     if (execv(data->pathToFile,
//               getArgumentForChildProgramm(data->pathToFile, data->buffer)) == -1)
//         perror("execvp");

//     return 0;
// }

char **getArgumentForChildProgramm(char *progName, const string &agruments, int &argc)
{
    vector<string> argvVector;
    size_t curPosition = 0;
    size_t spacePosition;
    string substr;

    argvVector.push_back(progName);
    do
    {
        spacePosition = agruments.find(' ', curPosition);
        substr = agruments.substr(curPosition, spacePosition - curPosition);
        if (!substr.empty())
            argvVector.push_back(substr);
        curPosition = spacePosition + 1;
    } while (spacePosition != string::npos);

    char **argv = new char *[argvVector.size() + 1];
    for (size_t i = 0; i < argvVector.size(); ++i)
    {
        argv[i] = new char[argvVector[i].size() + 1];
        strcpy(argv[i], argvVector[i].c_str());
    }
    argv[argvVector.size()] = 0;

    argc = argvVector.size();

    return argv;
}
