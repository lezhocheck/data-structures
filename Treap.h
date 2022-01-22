#pragma once
#include <iostream>
#include <ctime>
#include <random>
#include <memory>
#include <functional>

template<typename T>
class Treap {
public:
    Treap() : rnd(time(nullptr)) {}
    Treap(const Treap& other) = delete;
    Treap(Treap&& other) :
        rnd(time(nullptr)),
        root(move(other.root)) { }

    void insert(const T& value) {
        auto res = split(move(root), std::hash<T>{}(value));
        NodePtr node(new Node(value, rnd));
        root = move(merge(move(res.first),
            merge(move(node), move(res.second))));
    }

    void erase(const T& value) {
        auto hash = std::hash<T>{}(value);
        auto firstSplit = split(move(root), hash);
        auto secondSplit = split(move(firstSplit.first), static_cast<int64_t>(hash) - 1);
        root = move(merge(move(firstSplit.second), move(secondSplit.first)));
    }

    size_t size() const {
        return root->count;
    }

    void clear() {
        clear(root);
    }

    std::vector<T> toVector() const {
        std::vector<T> result;
        inorder(result, root);
        return result;
    }

    Treap& operator=(const Treap& other) = delete;

    Treap& operator=(Treap&& other) {
        root = move(other.root);
        return *this;
    }
private:
    std::mt19937 rnd;

    struct Node {
        T value;
        size_t hashedKey;
        int64_t priority;
        uint64_t count;
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;

        Node(const T& x, std::mt19937 gen) {
            value = x;
            hashedKey = std::hash<T>{}(x);
            priority = gen();
            count = 1;
        }
    };

    using NodePtr = std::unique_ptr<Node>;

    NodePtr root;

    uint64_t getCount(const NodePtr& node) const {
        return node ? node->count : 0;
    }

    void updateCount(NodePtr& node) {
        node->count = getCount(node->left) +
                getCount(node->right) + 1;
    }

    NodePtr merge(NodePtr left, NodePtr right) {
        if (!left) {
            return right;
        }
        if (!right) {
            return left;
        }
        if (left->priority > right->priority) {
            left->right = move(merge(move(left->right), move(right)));
            updateCount(left);
            return left;
        } else {
            right->left = move(merge(move(left), move(right->left)));
            updateCount(right);
            return right;
        }
    }

    std::pair<NodePtr, NodePtr> split(NodePtr node, size_t x) {
        if (!node) {
            return {nullptr, nullptr};
        }
        if (node->hashedKey <= x) {
            auto res = split(move(node->right), x);
            node->right = move(res.first);
            updateCount(node);
            return {move(node), move(res.second)};
        } else {
            auto res = split(move(node->left), x);
            node->left = move(res.second);
            updateCount(node);
            return {move(res.first), move(node)};
        }
    }

    void inorder(std::vector<T>& vector, const NodePtr& nodePtr) const {
        if (!nodePtr) {
            return;
        }
        inorder(vector, nodePtr->left);
        vector.push_back(nodePtr->value);
        inorder(vector, nodePtr->right);
    }

    void clear(NodePtr& nodePtr) {
        if (!nodePtr) {
            return;
        }
        clear(nodePtr->left);
        clear(nodePtr->right);
        nodePtr.reset();
    }
};
