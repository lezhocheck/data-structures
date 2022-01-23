#pragma once
#include "Stack.h"

template<typename T>
class Queue {
public:
    Queue() { }
    Queue(size_t size) : frontStack(size), backStack(size) { }

    Queue(const Queue& other) = delete;
    Queue(Queue&& other) :
        frontStack(std::move(other.frontStack)),
        backStack(std::move(other.backStack)) { }

    Queue& operator=(const Queue& other) = delete;
    Queue& operator=(Queue&& other) {
        frontStack = std::move(other.frontStack);
        backStack = std::move(other.backStack);
    }

    void push(const T& value) {
        backStack.push(value);
    }

    void push(T&& value) {
        backStack.push(std::move(value));
    }

    const T& pop() {
        shift();
        const T& temp = frontStack.pop();
        return temp;
    }

    const T& front() {
        shift();
        const T& temp = frontStack.top();
        return temp;
    }

    const T& min() {
        shift();
        const T& temp = frontStack.min();
        return temp;
    }

    const T& max() {
        shift();
        const T& temp = frontStack.max();
        return temp;
    }

    size_t size() const {
        return frontStack.size() + backStack.size();
    }

    bool empty() const {
        size_t size = frontStack.size() + backStack.size();
        return size == 0;
    }

    void clear() {
       frontStack.clear();
       backStack.clear();
    }

private:
    Stack<T> frontStack;
    Stack<T> backStack;

    void shift() {
        if (frontStack.empty()) {
            while (!backStack.empty()) {
                frontStack.push(std::move(backStack.pop()));
            }
        }
        if (frontStack.empty()) {
            throw std::invalid_argument("queue is empty");
        }
    }
};