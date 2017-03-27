#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <queue>

template <class T>
class Queue{
public:
    void push(const T &v) {
        q.push(v);
    }

    void push(T&& v) {
        q.push(v);
    }

    T pop() {
        T value = q.front();
        q.pop();
        return value;
    }

    size_t size() const {
        return q.size();
    }

    bool empty() const {
        return q.empty();
    }

private:
    std::queue<T> q;
};

#endif

