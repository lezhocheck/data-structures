#pragma once
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <algorithm>

template<typename Set>
class DisjointSetUnion {
public:
    DisjointSetUnion() { }

    DisjointSetUnion(const DisjointSetUnion& other) {
        base = other.base;
    }

    DisjointSetUnion(DisjointSetUnion&& other) {
        base = std::move(other.base);
    }

    void makeSet(const Set& set) {
        base[set] = {set, 0};
    }

    Set findSet(const Set& set) const {
        Node parentNode = base.at(set);
        if (set == parentNode.parent) {
            return set;
        }
        parentNode.parent = findSet(parentNode.parent);
        return parentNode.parent;
    }

    void unionSets(const Set& first, const Set& second) {
        Set firstParent = findSet(first);
        Set secondParent = findSet(second);
        if (firstParent != secondParent) {
            if (base[firstParent].rank < base[secondParent].rank) {
                std::swap(firstParent, secondParent);
            }
            base[secondParent].parent = firstParent;
            if (base[firstParent].rank == base[secondParent].rank) {
                ++base[firstParent].rank;
            }
        }
    }

    size_t size() {
        return base.size();
    }

    void clear() {
        base.clear();
    }

    DisjointSetUnion operator=(const DisjointSetUnion& other) {
        base = other.base;
    }

    DisjointSetUnion operator=(DisjointSetUnion&& other) {
        base = std::move(other.base);
    }

private:
    struct Node{
        Set parent;
        uint64_t rank;
    };

    std::unordered_map<Set, Node> base;
};