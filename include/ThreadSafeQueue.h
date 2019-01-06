#ifndef __THREADSAFEQUEUE_H
#define __THREADSAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <mempry>

using namespace std;
namespace talk
{
    template <typename T>
    class ThreadSafeQueue
    {
    private:
        mutable mutex _mLock;
        queue<shared_ptr<T> > _mQueue;
        condition_variable _mCond;
    public:
        ThreadSafeQueue()= default;

        void push(T value)
        {
            shared_ptr<T> pData(make_shared<T>(move(value)));
            std::lock_guard<mutex> lock(_mLock);
            _mQueue.push(pData);
            _mCond.notify_one();
        }

        void waitAndPop(T& value)
        {
            unique_lock<mutex> lock(_mLock);
            _mCond.wait(lock, [this] { return !_mQueue.empty();});
            value = move(*_mQueue.front());
            _mQueue.pop();
        }

        shared_ptr<T> waitAndPop()
        {
            unique_lock<mutex> lock(_mLock);
            _mCond.wait(lock, [this] { return !_mQueue.empty();});
            std::shared_ptr<T> pRet = data_queue.front();
            _mQueue.pop();
            return res;

        }

        bool tryPop(T& value) {
            lock_guard<mutex> lock(_mLock);
            if (_mQueue.empty())
                return false;
            value = std::move(*_mQueue.front());
            _mQueue.pop();
            return true;
        }


        shared_ptr<T> try_pop()
        {
            lock_guard<mutex> lock(_mLock);
            if(_mQueue.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> pRet=_mQueue.front();
            _mQueue.pop();
            return pRet;
        }


        bool empty() const
        {
            lock_guard<mutex> lock(_mLock);
            return _mQueue.empty();
        }


    };
}


#endif

