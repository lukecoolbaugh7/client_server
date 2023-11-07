#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "pack109final.hpp"
#include "hashset.hpp"
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void complete_request(Request req)
{
    int sockfd;
    int portno = 9999;
    int n;

    struct hostent *server;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("localhost");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    vec reqVec = pack109final::serialize(req);

    char reqBuffer[100000];

    for (int i = 0; i < reqVec.size(); i++)
    {
        reqBuffer[i] = reqVec.at(i);
    }

    n = write(sockfd, reqBuffer, reqVec.size());
    close(sockfd);
}

void sending(File file)
{
    int sockfd;
    int portno = 9999;
    int n;

    struct hostent *server;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("localhost");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    vec sendVec = pack109final::serialize(file);

    char sendBuffer[100000];
    for (int i = 0; i < sendVec.size(); i++)
    {
        sendBuffer[i] = sendVec.at(i);
    }

    n = write(sockfd, sendBuffer, sendVec.size());
    close(sockfd);
}

int main(int argc, char **argv)
{
    //CITE: the structural foundation of this code was modeled after : https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm
    if (strcmp(argv[1], "--hostname") != 0)
    {
        perror("invalid argument");
        exit(1);
    }
    int sockfd;
    int sockfdpc;
    int newsockfd;
    int newsockfdpc;
    int portno;
    int clilen;
    int clilenpc;
    hashset *storage = new hashset(20);
    char buffer[100000];
    int key = 42;
    struct hostent *client;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    struct sockaddr_in serv_addrpc;
    struct sockaddr_in cli_addrpc;

    int n;
    int pid;

    char *address = NULL;
    char *num = NULL;
    if (argv[2] != NULL)
    {
        char *token = strtok(argv[2], ":");
        int count = 0;

        while (token != NULL)
        {
            count++;
            if (count == 1)
            {
                address = token;
            }
            if (count == 2)
            {
                num = token;
            }
            token = strtok(NULL, ":");
        }
    }
    else
    {
        perror("not enough arguments");
        exit(1);
    }
    char addressArr[100];
    strcpy(addressArr, address);
    struct hostent *address1 = gethostbyname(addressArr);

    //first call to socket function
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    sockfdpc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfdpc < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    //initialize socket structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    bzero((char *)&serv_addrpc, sizeof(serv_addrpc));
    portno = atoi(num);

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    serv_addrpc.sin_family = AF_INET;

    serv_addrpc.sin_addr.s_addr = INADDR_ANY;
    serv_addrpc.sin_port = htons(9999);

    //now bind host address using bind() call

    if (::bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    if (::bind(sockfdpc, (struct sockaddr *)&serv_addrpc, sizeof(serv_addrpc)) < 0)
    {
        perror("ERROR on binding (parent child connection)");
        exit(1);
    }

    //now start listening for clients, here process will go into sleep mode and wait for the incoming connection
    printf("listening!\n");

    if (listen(sockfd, 5) < 0)
    {
        perror("error listening");
        exit(1);
    }
    clilen = sizeof(cli_addr);

    while (1)
    {

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        else
        {
            printf("new connection: localhost:%d\n", cli_addr.sin_port);
        }

        //create child process
        pid = fork();

        if (pid < 0)
        {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0)
        {

            bzero(buffer, 100000);
            n = recv(newsockfd, buffer, 100000, 0);
            if (n < 0)
            {
                perror("Error reading from socket");
                exit(1);
            }

            for (int i = 0; i < n; i++)
            {
                buffer[i] = buffer[i] ^ key;
            }

            if (buffer[9] == 0x02)
            {

                vec newVec;
                for (int i = 0; i < n; i++)
                {
                    newVec.push_back(buffer[i]);
                }

                File newFile = pack109final::deserialize_file(newVec);
                sending(newFile);

                n = write(newsockfd, "complete", 8);
            }
            else
            {

                vec newVec;
                for (int i = 0; i < n; i++)
                {
                    newVec.push_back(buffer[i]);
                }

                Request newRequest = pack109final::deserialize_request(newVec);
                complete_request(newRequest);
                bzero(buffer, 100000);

                //looking for file

                File foundFile = storage->get(newRequest.name);

                //serialize
                vec retFile = pack109final::serialize(foundFile);

                //encrypt
                for (int i = 0; i < retFile.size(); i++)
                {
                    retFile.at(i) = retFile.at(i) ^ key;
                }
                char bufferSend[100000];
                std::copy(retFile.begin(), retFile.end(), bufferSend);
                n = write(newsockfd, bufferSend, retFile.size());

                if (n < 0)
                {
                    perror("ERROR writing to socket");
                    exit(1);
                }
                printf("sending !\n");
                bzero(bufferSend, 100000);
            }
            printf("terminating connection\n");
            exit(0);
        }
        else
        {

            listen(sockfdpc, 5);
            clilenpc = sizeof(cli_addrpc);
            newsockfdpc = accept(sockfdpc, (struct sockaddr *)&cli_addrpc, (socklen_t *)&clilenpc);
            if (newsockfd < 0)
            {
                perror("Error on accept between parent and child");
            }

            char bufferpc[100000];
            bzero(bufferpc, 100000);
            n = recv(newsockfdpc, bufferpc, 100000, 0);

            vec vecpc;
            for (int i = 0; i < n; i++)
            {
                vecpc.push_back(bufferpc[i]);
            }

            if (bufferpc[9] == 0x02)
            {
                File filepc = pack109final::deserialize_file(vecpc);
                storage->insert(filepc.name, filepc);
            }
        }
    }
}
