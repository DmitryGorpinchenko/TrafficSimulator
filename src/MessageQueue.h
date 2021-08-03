#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <deque>
#include <limits>
#include <mutex>
#include <condition_variable>

template <typename T>
class MessageQueue
{
public:
    explicit MessageQueue(size_t max_size = std::numeric_limits<size_t>::max());

    void send(T &&msg);
    T receive();

    void waitForMessage(const T &msg);

private:
    std::condition_variable _cv;
    std::mutex _mutex;
    
    std::deque<T> _queue;
    size_t _max_size;
};

template <typename T>
MessageQueue<T>::MessageQueue(size_t max_size)
    : _max_size(max_size)
{}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    {
        std::lock_guard lock(_mutex);

        _queue.push_back(std::move(msg));
        while (_queue.size() > _max_size) {
            _queue.pop_front();
        }
    }
    _cv.notify_one();
}

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock lock(_mutex);
    _cv.wait(lock, [this]() { return !_queue.empty(); });

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::waitForMessage(const T &msg)
{
    while (receive() != msg)
        ;
}

#endif
