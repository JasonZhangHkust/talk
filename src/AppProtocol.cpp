#include "AppProtocol.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
using namespace std;

namespace talk {
    int AppProtocol::parse(const char *pBuff, size_t iLength, list<ipcProtocol> &lMsgs) {
        size_t pos = 0;
        while (pos < iLength) {
            uint32_t iLen = iLength - pos;
            cout << "iLen " << iLen << endl;

            for (int i = 0; i < iLength; i++) {
                printf("%X", pBuff[i]);
            }
            cout << endl;
            if (iLen < 4) {
                // not enough buffer
                break;
            }
            uint32_t iHeaderLen;
            iHeaderLen = ntohl(*(uint32_t *) (pBuff + pos));
            if (iLen < iHeaderLen) {
                break;
            } else {
                ipcProtocol tMsg;
                // read msg type
                tMsg.iType = ntohl(*(uint32_t *) (pBuff + pos + 4));
                // read msg payload
                tMsg.sPayload = string(pBuff + pos + 8, iHeaderLen - 8);
                pos += iHeaderLen;
                lMsgs.push_back(tMsg);
                //cout << "\npos " << pos << endl;
            }

        }

        return pos;


    }

    void AppProtocol::format(ipcProtocol &tMsg, string &sBuf) {
        uint32_t iHeaderLen = htonl(4 + tMsg.size());
        uint32_t iType = htonl(tMsg.iType);
        sBuf.clear();
        sBuf.reserve(4 + tMsg.size());
        sBuf.append((const char *) &iHeaderLen, 4);
        sBuf.append((const char *) &iType, 4);
        sBuf.append(tMsg.sPayload);

    }
}