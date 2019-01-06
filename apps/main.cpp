#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/Looper.h"
#include "iostream"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "AppProtocol.h"
#include <algorithm>

using namespace talk;

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_un server = {0};

    char buffer[256];

    if (argc < 2) {
        fprintf(stderr, "usage %s <pathname>", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, argv[1]);

    if (connect(sockfd, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        close(sockfd);
        perror("connecting stream socket");
        exit(1);
    }

    std::cout << std::to_string(sockfd) << std::endl;
    Looper MyLooper;
    shared_ptr<Transceiver> pTrans(new Transceiver(sockfd));
    MyLooper.initialize(sockfd, pTrans);
    MyLooper.start();
    while (std::cin) {
        std::string message;
        ipcProtocol tMsg;
        tMsg.iType = 0;
        getline(std::cin, tMsg.sPayload);
        if(tMsg.sPayload.empty())
            continue;
        string sBuff = "";
        AppProtocol::format(tMsg, sBuff);
        cout << "User input protocol " << sBuff.size() << endl;
        for (int i = 0; i < sBuff.size(); i++) {
            printf("%X", sBuff[i]);
        }
        printf("\n");
        pTrans->writeToSendBuffer(sBuff);
        MyLooper.notify(pTrans);
    }

    return 0;
}