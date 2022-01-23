#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>

template<typename T>
class Stack {
public:
    Stack() { }
    Stack(size_t size) : stackBase(std::vector<StackNode>(size)) { }

    Stack(const Stack& other) = delete;
    Stack(Stack&& other) :
        stackBase(std::move(other.stackBase)),
        backPtr(std::move(other.backPtr)) { }

    Stack& operator=(const Stack& other) = delete;
    Stack& operator=(Stack&& other) {
        stackBase = std::move(other.stackBase);
        backPtr = std::move(other.backPtr);
    }

    void push(const T& value) {
        StackNode node;
        node.value = value;
        if (empty()) {
            node.minimum = value;
            node.maximum = value;
        } else {
            node.minimum = std::min(value, stackBase[backPtr - 1].minimum);
            node.maximum = std::max(value, stackBase[backPtr - 1].maximum);
        }

        if (backPtr >= stackBase.size()) {
            stackBase.push_back(node);
        } else {
            stackBase[backPtr] = node;
        }
        ++backPtr;
    }

    void push(T&& value) {
        StackNode node;
        node.value = std::move(value);
        if (empty()) {
            node.minimum = node.value;
            node.maximum = node.value;
        } else {
            node.minimum = std::min(node.value, stackBase[backPtr - 1].minimum);
            node.maximum = std::max(node.value, stackBase[backPtr - 1].maximum);
        }

        if (backPtr >= stackBase.size()) {
            stackBase.push_back(node);
        } else {
            stackBase[backPtr] = node;
        }
        ++backPtr;
    }

    T& pop() {
        if (backPtr == 0) {
            throw std::invalid_argument("stack is empty");
        }
        --backPtr;
        return stackBase[backPtr].value;
    }

    const T& top() const {
        if (backPtr == 0) {
            throw std::invalid_argument("stack is empty");
        }
        return stackBase[backPtr - 1].value;
    }

    const T& min() const {
        if (backPtr == 0) {
            throw std::invalid_argument("stack is empty");
        }
        return stackBase[backPtr - 1].minimum;
    }

    const T& max() const {
        if (backPtr == 0) {
            throw std::invalid_argument("stack is empty");
        }
        return stackBase[backPtr - 1].maximum;
    }

    size_t size() const {
        return backPtr;
    }

    bool empty() const {
        return backPtr == 0;
    }

    void clear() {
        backPtr = 0;
    }

private:
    struct StackNode {
        T value;
        T minimum;
        T maximum;
    };
    std::vector<StackNode> stackBase;
    size_t backPtr;
};