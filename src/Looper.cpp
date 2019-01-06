#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <arpa/inet.h>
#include "Looper.h"

namespace talk
{
    Looper::Looper() : _bQuit(false) {}

    Looper::~Looper() {
        if (!_bQuit) {
            quit();

            if (_tLooperThread.joinable()) {
                _tLooperThread.join();
            }
        }
    }

    void Looper::initialize(int fd, shared_ptr<Transceiver> pTrans) {
        _tEpoller.create(1);

        _iLooperFD = fd;

        if (_iLooperFD == -1) {
            std::cout << "Please provide valid FD" << std::endl;
        }

        _tEpoller.add(_iLooperFD, _iLooperFD, EPOLLIN | EPOLLOUT);
        this->_pTrans = pTrans;

    }

    void Looper::start() {
        _tLooperThread = std::thread(&Looper::run, this);
    }

    void Looper::run() {
        while (!_bQuit) {
            try {
                int iEventNum = _tEpoller.wait(1000);

                for (int n = 0; n < iEventNum; ++n) {
                    const epoll_event &ev = _tEpoller.get(n);
                    if (ev.events & EPOLLIN) {
                        handleRead();
                    } else if (ev.events & EPOLLOUT) {
                        handleSend();
                    }
                }
            }
            catch (std::exception &ex) {
                std::cout << "Event Error" << std::endl;
            }
            catch (...) {}
        }
        close(_iLooperFD);
    }

    void Looper::quit() {
        std::lock_guard<std::mutex> guard(_mLock);
        _bQuit = true;
        _mCond.notify_all();

    }

    void Looper::handleSend() {
        if (_pTrans->isValid()) {
            while (_pTrans->handleResponse() > 0);
        }
    }


    void Looper::handleRead() {
        if (_pTrans->isValid()) {
            list<ipcProtocol> lMsg;
            _pTrans->handleRequest(lMsg);
            for (auto tMsg : lMsg) {
                cout << "\x1b[35mParse Msg from Server " << tMsg.sPayload << "\x1b[0m" << endl;
            }
        }
    }

    void Looper::notify(shared_ptr<Transceiver> pTrans) {
        if (pTrans->isValid()) {
            _tEpoller.modify(pTrans->getFD(), pTrans->getFD(), EPOLLOUT | EPOLLIN);
        }
    }

    inline uint64_t Looper::getNowUs() {
        struct timeval tTtimeVal;
        gettimeofday(&tTtimeVal, NULL);

        return tTtimeVal.tv_sec * 1000000 + tTtimeVal.tv_usec;
    }

    void Looper::readLooperFD() {
        uint32_t length = 0;
        char *buffer = 0;

        readRawBytes(_iLooperFD, sizeof(length), (void *) (&length));
        length = ntohl(length);
        std::cout << "Receive from server " << std::to_string(length) << std::endl;
        buffer = new char[length];
        readRawBytes(_iLooperFD, length, (void *) buffer);
        for (int i = 0; i < length; i++) {
            printf("%X", buffer[i]);
        }
        handleRead(std::string((char *) buffer, length));
        delete[] buffer;
    }

    void Looper::handleRead(std::string buff) {
        std::string output = buff.empty() ? "Empty" : buff;
        std::cout << "Receive Message " << output << std::endl;
    }

    void Looper::readRawBytes(int socket, unsigned int x, void *buffer) {
        int bytesRead = 0;
        int result;
        while (bytesRead < x) {

            result = read(socket, (char *) buffer + bytesRead, x - bytesRead);
            if (result < 1) {
                std::cout << "Read error code " << result << std::endl;
            }

            bytesRead += result;
        }
    }
}