#include "Transceiver.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include "errno.h"

namespace talk {
    Transceiver::Transceiver(int iFD) {
        _iFD = iFD;
        _sSendBuf.reserve(1024);
    }

    Transceiver::~Transceiver() {
        this->close();
    }

    bool Transceiver::isValid() {
        return _iFD != -1;
    }

    void Transceiver::close() {
        if (!isValid()) return;

        ::close(_iFD);


        _iFD = -1;

        _sSendBuf.clear();

        _sRecvBuf.clear();

    }

    int Transceiver::handleResponse() {
        if (!isValid()) return -1;
        int iBytesSent = 0;
        do {
            iBytesSent = 0;
            if (!_sSendBuf.empty()) {
                size_t length = _sSendBuf.length();
                iBytesSent = this->send(_sSendBuf.c_str(), _sSendBuf.length(), 0);
                if (iBytesSent > 0) {
                    if (iBytesSent == length) {
                        _sSendBuf.clear();
                    } else {
                        _sSendBuf.erase(_sSendBuf.begin(), _sSendBuf.begin() + min((size_t) iBytesSent, length));
                    }
                }
            }
        } while (iBytesSent > 0);

        return iBytesSent;
    }

    int Transceiver::send(const void *buf, uint32_t len, uint32_t flag) {
        if (!isValid()) return -1;

        int iByteSent = ::send(_iFD, buf, len, flag);

        if (iByteSent < 0 && errno != EAGAIN) {
            return 0;
        }

        return iByteSent;
    }

    int Transceiver::recv(void *buf, uint32_t len, uint32_t flag) {
        if (!isValid()) return -1;

        int iByteRead = ::recv(_iFD, buf, len, flag);
        if (iByteRead == 0 || (iByteRead < 0 && errno != EAGAIN)) {
            return 0;
        }

        return iByteRead;
    }

    int Transceiver::handleRequest(list<ipcProtocol> &lMsgs) {
        if (!isValid()) return -1;

        int iBytesRead = 0;

        lMsgs.clear();

        char buff[8192] = {0};

        do {
            if ((iBytesRead = this->recv(buff, sizeof(buff), 0)) > 0) {
                _sRecvBuf.append(buff, iBytesRead);
            }
        } while (iBytesRead);
        if (!_sRecvBuf.empty()) {
            try {
                size_t pos = AppProtocol::parse(_sRecvBuf.c_str(), _sRecvBuf.length(), lMsgs);

                if (pos > 0) {
                    //cout << "Erase prev " << min(pos, _sRecvBuf.length());
                    _sRecvBuf.erase(_sRecvBuf.begin(), _sRecvBuf.begin() + min(pos, _sRecvBuf.length()));

                    if (_sRecvBuf.capacity() - _sRecvBuf.length() > 102400) {
                        _sRecvBuf.reserve(max(_sRecvBuf.length(), (size_t) 1024));
                    }
                }
            }
            catch (exception &ex) {
            }
            catch (...) {
            }
        }
        return lMsgs.empty() ? 0 : 1;
    }

    void Transceiver::writeToSendBuffer(const string &msg) {
        _sSendBuf.append(msg);
    }
}