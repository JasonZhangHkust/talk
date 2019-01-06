#ifndef __APPPROTOCOL_H
#define __APPPROTOCOL_H

#include <string>
#include <vector>
#include <memory>
#include <list>

using namespace std;
namespace talk {
    typedef struct ipcProtocol {
        uint32_t iType;
        string sPayload;

        int size() {
            return 4 + sPayload.size();
        };
    } ipcProtocol;

    class AppProtocol {
    public:
        //format the struct to protocol buffer
        static void format(ipcProtocol &tMsg, string &sBuf);

        //parse the protocol buffer to struct
        static int parse(const char *pBuff, size_t iLength, list<ipcProtocol> &lMsgs);

    private:
    };
}


#endif
