#ifndef TICKETSYSTEM_BPLUSTREE_HPP
#define TICKETSYSTEM_BPLUSTREE_HPP

#include "../include/MemoryRiver.h"
#include "../include/Vector.h"
#include "../include/BPlusTree.hpp"
#include "../include/UserManager.h"
#include "../include/TrainManager.h"
#include "../include/OrderManager.h"
#include <cstring>
#include <string>


template<typename Key, typename Value, int M>
BPlusTree<Key, Value, M>::BPlusTree(const std::string& fn) : file_name(fn), tree(fn) {
    tree.initialise();
    tree.get_info(root, 1);
}

template<typename Key, typename Value, int M>
BPlusTree<Key, Value, M>::~BPlusTree() {
    tree.write_info(root, 1);
    tree.close();
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::insert_parent(int left_pos, const Key& mid_key, int right_pos) {
    Node left;
    tree.read(left, left_pos);

    if (left.parent == -1) {
        Node new_root;
        new_root.size = 1;
        new_root.keys[0] = mid_key;
        new_root.children[0] = left_pos;
        new_root.children[1] = right_pos;
        root = tree.write(new_root);
        tree.write_info(root, 1);

        left.parent = root;
        tree.update(left, left_pos);
        Node right;
        tree.read(right, right_pos);
        right.parent = root;
        tree.update(right, right_pos);
        return;
    }

    int parent_pos = left.parent;
    Node parent;
    tree.read(parent, parent_pos);

    int idx = 0;
    while (idx < parent.size && parent.children[idx] != left_pos) {
        idx++;
    }

    for (int i = parent.size; i > idx; --i) {
        parent.keys[i] = parent.keys[i - 1];
        parent.children[i + 1] = parent.children[i];
    }
    parent.keys[idx] = mid_key;
    parent.children[idx + 1] = right_pos;
    parent.size++;
    tree.update(parent, parent_pos);

    Node right;
    tree.read(right, right_pos);
    right.parent = parent_pos;
    tree.update(right, right_pos);

    if (parent.size == M) {
        split(parent_pos, parent);
    }
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::split(int pos, Node& node) {
    int mid = node.size / 2;
    Node new_node;
    new_node.leaf = node.leaf;
    new_node.parent = node.parent;

    if (node.leaf) {
        new_node.size = node.size - mid;
        node.size = mid;
        for (int i = 0; i < new_node.size; ++i) {
            new_node.keys[i] = node.keys[i + mid];
            new_node.values[i] = node.values[i + mid];
        }
        new_node.right = node.right;
        new_node.left = pos;
        int new_pos = tree.write(new_node);

        if (node.right != -1) {
            Node next;
            tree.read(next, node.right);
            next.left = new_pos;
            tree.update(next, node.right);
        }
        node.right = new_pos;
        tree.update(node, pos);
        insert_parent(pos, new_node.keys[0], new_pos);
    } else {
        Key mid_key = node.keys[mid];
        new_node.size = node.size - mid - 1;
        for (int i = 0; i < new_node.size; ++i) {
            new_node.keys[i] = node.keys[mid + i + 1];
            new_node.children[i] = node.children[mid + i + 1];
        }
        new_node.children[new_node.size] = node.children[node.size];
        node.size = mid;
        int new_pos = tree.write(new_node);
        tree.update(node, pos);

        for (int i = 0; i <= new_node.size; ++i) {
            Node child;
            tree.read(child, new_node.children[i]);
            child.parent = new_pos;
            tree.update(child, new_node.children[i]);
        }
        insert_parent(pos, mid_key, new_pos);
    }
}

template<typename Key, typename Value, int M>
bool BPlusTree<Key, Value, M>::borrow_from_left(int pos, Node& node, int parent_pos, Node& parent, int idx) {
    if (idx == 0) return false;

    int left_pos = parent.children[idx - 1];
    Node left;
    tree.read(left, left_pos);
    if (left.size == MIN_SIZE) return false;

    for (int i = node.size; i > 0; --i) {
        node.keys[i] = node.keys[i - 1];
        if (node.leaf) node.values[i] = node.values[i - 1];
    }
    if (!node.leaf) {
        for (int i = node.size + 1; i > 0; --i) {
            node.children[i] = node.children[i - 1];
        }
    }

    if (node.leaf) {
        node.keys[0] = left.keys[left.size - 1];
        node.values[0] = left.values[left.size - 1];
        parent.keys[idx - 1] = node.keys[0];
    } else {
        node.keys[0] = parent.keys[idx - 1];
        parent.keys[idx - 1] = left.keys[left.size - 1];
        node.children[0] = left.children[left.size];
        Node child;
        tree.read(child, node.children[0]);
        child.parent = pos;
        tree.update(child, node.children[0]);
    }

    node.size++;
    left.size--;
    tree.update(node, pos);
    tree.update(left, left_pos);
    tree.update(parent, parent_pos);
    return true;
}

template<typename Key, typename Value, int M>
bool BPlusTree<Key, Value, M>::borrow_from_right(int pos, Node& node, int parent_pos, Node& parent, int idx) {
    if (idx >= parent.size) return false;

    int right_pos = parent.children[idx + 1];
    Node right;
    tree.read(right, right_pos);
    if (right.size == MIN_SIZE) return false;

    if (node.leaf) {
        node.keys[node.size] = right.keys[0];
        node.values[node.size] = right.values[0];
        for (int i = 0; i < right.size - 1; ++i) {
            right.keys[i] = right.keys[i + 1];
            right.values[i] = right.values[i + 1];
        }
        parent.keys[idx] = right.keys[0];
    } else {
        node.keys[node.size] = parent.keys[idx];
        node.children[node.size + 1] = right.children[0];
        Node child;
        tree.read(child, node.children[node.size + 1]);
        child.parent = pos;
        tree.update(child, node.children[node.size + 1]);
        parent.keys[idx] = right.keys[0];
        for (int i = 0; i < right.size; ++i) {
            right.keys[i] = right.keys[i + 1];
            right.children[i] = right.children[i + 1];
        }
        right.children[right.size - 1] = right.children[right.size];
    }

    node.size++;
    right.size--;
    tree.update(node, pos);
    tree.update(right, right_pos);
    tree.update(parent, parent_pos);
    return true;
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::merge(int left_pos, int right_pos, int parent_pos, int idx) {
    Node left, right, parent;
    tree.read(left, left_pos);
    tree.read(right, right_pos);
    tree.read(parent, parent_pos);

    if (left.leaf) {
        for (int i = 0; i < right.size; ++i) {
            left.keys[left.size + i] = right.keys[i];
            left.values[left.size + i] = right.values[i];
        }
        left.right = right.right;
        if (right.right != -1) {
            Node next;
            tree.read(next, right.right);
            next.left = left_pos;
            tree.update(next, right.right);
        }
    } else {
        left.keys[left.size] = parent.keys[idx];
        for (int i = 0; i < right.size; ++i) {
            left.keys[left.size + 1 + i] = right.keys[i];
        }
        for (int i = 0; i <= right.size; ++i) {
            left.children[left.size + 1 + i] = right.children[i];
            Node child;
            tree.read(child, left.children[left.size + 1 + i]);
            child.parent = left_pos;
            tree.update(child, left.children[left.size + 1 + i]);
        }
        left.size++;
    }

    left.size += right.size;
    for (int i = idx; i < parent.size - 1; ++i) {
        parent.keys[i] = parent.keys[i + 1];
        parent.children[i + 1] = parent.children[i + 2];
    }
    parent.size--;
    tree.update(left, left_pos);
    tree.update(parent, parent_pos);
    balance(parent_pos, parent);
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::balance(int pos, Node& node) {
    if (pos == root) {
        if (!node.leaf && node.size == 0) {
            root = node.children[0];
            tree.write_info(root, 1);
            Node new_root;
            tree.read(new_root, root);
            new_root.parent = -1;
            tree.update(new_root, root);
        }
        return;
    }
    if (node.size >= MIN_SIZE) return;

    int parent_pos = node.parent;
    Node parent;
    tree.read(parent, parent_pos);
    int idx = 0;
    while (idx <= parent.size && parent.children[idx] != pos) {
        idx++;
    }

    if (borrow_from_left(pos, node, parent_pos, parent, idx)) return;
    if (borrow_from_right(pos, node, parent_pos, parent, idx)) return;

    if (idx > 0) {
        merge(parent.children[idx - 1], pos, parent_pos, idx - 1);
    } else {
        merge(pos, parent.children[idx + 1], parent_pos, idx);
    }
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::insert(const Key& key, const Value& value) {
    if (root == 0) {
        Node new_node;
        new_node.leaf = true;
        new_node.size = 1;
        new_node.keys[0] = key;
        new_node.values[0] = value;
        root = tree.write(new_node);
        tree.write_info(root, 1);
        return;
    }

    int cur = root;
    Node node;
    while (true) {
        tree.read(node, cur);
        if (node.leaf) break;
        int idx = 0;
        while (idx < node.size && !(key < node.keys[idx])) {
            idx++;
        }
        cur = node.children[idx];
    }

    // 检查是否已存在
    for (int i = 0; i < node.size; ++i) {
        if (node.keys[i] == key && node.values[i] == value) {
            return;
        }
    }

    int pos = 0;
    for (int j = node.size - 1; j >= 0; --j) {
        if (node.keys[j] < key || (!(key < node.keys[j]) && node.values[j] < value)) {
            pos = j + 1;
            break;
        }
        node.keys[j + 1] = node.keys[j];
        node.values[j + 1] = node.values[j];
    }
    node.keys[pos] = key;
    node.values[pos] = value;
    node.size++;
    tree.update(node, cur);

    if (node.size == M) {
        split(cur, node);
    }
}

template<typename Key, typename Value, int M>
bool BPlusTree<Key, Value, M>::remove(const Key& key) {
    if (root == 0) return false;

    int cur = root;
    Node node;
    while (true) {
        tree.read(node, cur);
        if (node.leaf) break;
        int idx = 0;
        while (idx < node.size && !(key < node.keys[idx])) {
            idx++;
        }
        cur = node.children[idx];
    }

    for (int i = 0; i < node.size; ++i) {
        if (node.keys[i] == key) {
            for (int j = i; j < node.size - 1; ++j) {
                node.keys[j] = node.keys[j + 1];
                node.values[j] = node.values[j + 1];
            }
            node.size--;
            tree.update(node, cur);
            balance(cur, node);
            return true;
        }
    }
    return false;
}

template<typename Key, typename Value, int M>
bool BPlusTree<Key, Value, M>::find(const Key& key, Value& result) {
    if (root == 0) return false;

    int cur = root;
    Node node;
    while (true) {
        tree.read(node, cur);
        if (node.leaf) break;
        int idx = 0;
        while (idx < node.size && !(key < node.keys[idx])) {
            idx++;
        }
        cur = node.children[idx];
    }

    for (int i = 0; i < node.size; ++i) {
        if (node.keys[i] == key) {
            result = node.values[i];
            return true;
        }
    }
    return false;
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::find_range(const Key& low, const Key& high, sjtu::vector<Value>& result) {
    if (root == 0) {
        return;
    }

    int cur = root;
    Node node;
    while (true) {
        tree.read(node, cur);
        if (node.leaf) break;
        int idx = 0;
        while (idx < node.size && !(low < node.keys[idx])) {
            idx++;
        }
        cur = node.children[idx];
    }

    while (cur != -1) {
        tree.read(node, cur);
        for (int i = 0; i < node.size; ++i) {
            if (!(node.keys[i] < low) && !(high < node.keys[i])) {
                result.push_back(node.values[i]);
            }
            else if (high < node.keys[i]) {
                return;
            }
        }
        cur = node.right;
    }
}

template<typename Key, typename Value, int M>
void BPlusTree<Key, Value, M>::clear() {
    root = 0;
    tree.write_info(root, 1);
}

template<typename Key, typename Value, int M>
bool BPlusTree<Key, Value, M>::empty() {
    return root == 0;
}

template class BPlusTree<OrderKey, int, 100>;
template class BPlusTree<PendingOrderKey, PendingOrder, 100>;
template class BPlusTree<TrainKey, int, 100>;
template class BPlusTree<DayKey, int, 100>;
template class BPlusTree<StationKey, Station, 100>;
template class BPlusTree<UserKey, int, 100>;
#endif //TICKETSYSTEM_BPLUSTREE_HPP