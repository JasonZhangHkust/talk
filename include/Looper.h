#ifndef __CLIENT_LOOPER_H
#define __CLIENT_LOOPER_H

#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "Transceiver.h"
#include "Epoller.h"

namespace talk {
    class Looper {
    public:
        Looper();

        ~Looper();

        void run();

        void start();

        void initialize(int fd, shared_ptr<Transceiver> pTrans);

        void quit();

        void notify(shared_ptr<Transceiver> pTrans);

        void readRawBytes(int socket, unsigned int x, void *buffer);
        //unsigned long int == uint64_t

    private:
        std::thread _tLooperThread;
        std::mutex _mLock;
        std::condition_variable _mCond;
        bool _bQuit;
        int _iLooperFD;
        Epoller _tEpoller;

    private:
        uint64_t getNowUs();

        void readLooperFD();

        void handleRead(std::string buffer);

        void handleRead();

        void handleSend();

        shared_ptr<Transceiver> _pTrans;
    };
}

#endif
