#ifndef TICKETSYSTEM_BPLUSTREE_H
#define TICKETSYSTEM_BPLUSTREE_H

#include "MemoryRiver.h"
#include "Vector.h"
#include <cstring>
#include <iostream>

template<typename Key, typename Value, int M = 100>
class BPlusTree {
private:
    static const int MIN_SIZE = (M - 1) / 2;

    struct Node {
        bool leaf = false;
        int size = 0;
        int parent = -1;
        int left = -1;
        int right = -1;
        int children[M + 1];
        Key keys[M + 1];
        Value values[M + 1];

        Node() {
            for (int i = 0; i <= M; ++i) {
                children[i] = -1;
            }
        }
    };

    int root;
    MemoryRiver<Node, 2> tree;
    std::string file_name;

    void split(int pos, Node& node);
    void balance(int pos, Node& node);
    void insert_parent(int left_pos, const Key& mid_key, int right_pos);
    bool borrow_from_left(int pos, Node& node, int parent_pos, Node& parent, int idx);
    bool borrow_from_right(int pos, Node& node, int parent_pos, Node& parent, int idx);
    void merge(int left_pos, int right_pos, int parent_pos, int idx);

public:
    BPlusTree(const std::string& file_name);
    ~BPlusTree();

    void insert(const Key& key, const Value& value);
    bool remove(const Key& key, const Value& value);
    void find(const Key& key, sjtu::vector<Value>& res);
    bool find_range(const Key& low, const Key& high,sjtu::vector<Value>& res);
    void clear();
    bool empty();
};

#endif //TICKETSYSTEM_BPLUSTREE_H