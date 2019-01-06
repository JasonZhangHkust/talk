#ifndef __TRANSCEIVER_H
#define __TRANSCEIVER_H

#include <cstdint>
#include <string>
#include "AppProtocol.h"
using namespace std;

namespace talk {
    class Transceiver {
    public:
        Transceiver(int fd);

        ~Transceiver();

        void reInit(int fd);

        int send(const void *buf, uint32_t len, uint32_t flag);

        int recv(void *buf, uint32_t len, uint32_t flag);

        void close();

        bool isValid();

        int handleRequest(list<ipcProtocol> &lMsgs);

        int handleResponse();

        int getFD() { return _iFD; };

        void writeToSendBuffer(const string &msg);

    private:
        int _iFD;
        string _sSendBuf;
        string _sRecvBuf;

    };
}

#endif //SOCKET_CLIENT_TRANSCEIVER_H
