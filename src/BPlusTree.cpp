#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    /* your code here */
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);

public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out | std::ios::binary);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
        else {
            return;
        }
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        // file.close();


    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        //TODO
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char*> (&tmp),sizeof(int));
        /* your code here */
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        //TODO
        file.seekp((n - 1) * sizeof(int));
        file.write(reinterpret_cast<char*> (&tmp),sizeof(int));
        /* your code here */
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        if (!file.is_open()) return -1;
        file.seekp(0,std::ios::end);
        int index = file.tellp();
        file.write(reinterpret_cast<char*> (&t),sizeofT);
        return index;
        /* your code here */
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        file.seekp(index);
        file.write(reinterpret_cast<char*> (&t),sizeofT);
        /* your code here */
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        file.seekg(index);
        file.read(reinterpret_cast<char*> (&t),sizeofT);
        /* your code here */
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        /* your code here */
    }

    void close() {
        if (file.is_open()) {
            file.close();
        }
    }
};

struct Data {
    char index[65];
    int value;

    Data() = default;

    Data(char index_[65], int value_) {
        std::strcpy(index,index_);
        value = value_;
    }

    bool operator==(const Data& other) {
        if (value == other.value && std::strcmp(index,other.index) == 0) {
            return true;
        }
        return false;
    }

    bool operator<(const Data& other) {
        if (std::strcmp(index,other.index) < 0) {
            return true;
        }
        if (std::strcmp(index,other.index) > 0) {
            return false;
        }
        if (value < other.value) {
            return true;
        }
        return false;
    }

    bool operator<=(const Data& other) {
        return (*this < other || *this == other);
    }

    friend bool operator<(const Data & a, const Data & b) {
        if (std::strcmp(a.index,b.index) < 0) {
            return true;
        }
        if (std::strcmp(a.index,b.index) > 0) {
            return false;
        }
        if (a.value < b.value) {
            return true;
        }
        return false;
    }
};

const int M = 200;
const int MIN_SIZE = (M - 1) / 2;

struct Node {
    bool leaf = false; //是否为叶子节点
    int size = 0; // 关键字个数
    int parent = -1; //父节点
    int left = -1; // 左兄弟
    int right = -1; // 右兄弟
    int children[M + 1]; //孩子们
    Data data[M + 1]; //关键字

    Node() = default;

};

class BPlusTree {
private:
    int root;
    MemoryRiver<Node,2> tree;

public:
    BPlusTree(std::string file_name) : tree(file_name) {
        tree.initialise();
        tree.get_info(root, 1);
    }

    ~BPlusTree() {
        tree.write_info(root, 1);
        tree.close();
    }

    //
    void insert_parent(int left_pos, const Data& mid_data, int right_pos) {
        Node left;
        tree.read(left,left_pos);
        if (left.parent == -1) {
            Node new_root;
            new_root.size = 1;
            new_root.data[0] = mid_data;
            new_root.children[0] = left_pos;
            new_root.children[1] = right_pos;
            root = tree.write(new_root);
            tree.write_info(root, 1);
            left.parent = root;
            tree.update(left,left_pos);
            Node right;
            tree.read(right,right_pos);
            right.parent = root;
            tree.update(right,right_pos);
            return;
        }
        int parent_pos = left.parent;
        Node parent;
        tree.read(parent,parent_pos);
        int idx = 0;
        while (idx < parent.size && parent.children[idx] != left_pos) {
            idx++;
        }
        for (int i = parent.size; i > idx; --i) {
            parent.data[i] = parent.data[i - 1];
            parent.children[i + 1] = parent.children[i];
        }
        parent.data[idx] = mid_data;
        parent.children[idx + 1] = right_pos;
        parent.size++;
        tree.update(parent, parent_pos);
        Node right;
        tree.read(right,right_pos);
        right.parent = parent_pos;
        tree.update(right,right_pos);
        if (parent.size == M) {
            split(parent_pos, parent);
        }
    }

    void split(int pos, Node& node) {
        int mid = node.size / 2;
        Node new_node;
        new_node.leaf = node.leaf;
        new_node.parent = node.parent;
        if (node.leaf) {
            new_node.size = node.size - mid;
            node.size = mid;
            for (int i = 0; i < new_node.size; ++i) {
                new_node.data[i] = node.data[i + mid];
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
            tree.update(node,pos);
            insert_parent(pos, new_node.data[0], new_pos);
        }
        else {
            Data mid_data = node.data[mid];
            new_node.size = node.size - mid - 1;
            for (int i = 0; i < new_node.size; ++i) {
                new_node.data[i] = node.data[mid + i + 1];
                new_node.children[i] = node.children[mid + i + 1];
            }
            new_node.children[new_node.size] = node.children[node.size];
            node.size = mid;
            int new_pos = tree.write(new_node);
            tree.update(node, pos);
            for (int i = 0; i <= new_node.size; ++i) {
                Node child;
                tree.read(child,new_node.children[i]);
                child.parent = new_pos;
                tree.update(child,new_node.children[i]);
            }
            insert_parent(pos, mid_data, new_pos);
        }

    }

    bool borrow_from_left(int pos, Node& node, int parent_pos, Node& parent, int idx) {
        if (idx == 0) {
            return false;
        }
        int left_pos = parent.children[idx - 1];
        Node left;
        tree.read(left, left_pos);
        if (left.size == MIN_SIZE) {
            return false;
        }
        for (int i = node.size; i > 0; --i) {
            node.data[i] = node.data[i - 1];
        }
        if (!node.leaf) {
            for (int i = node.size + 1; i > 0; --i) {
                node.children[i] = node.children[i - 1];
            }
        }
        if (node.leaf) {
            node.data[0] = left.data[left.size - 1];
            parent.data[idx - 1] = node.data[0];
        }
        else {
            node.data[0] = parent.data[idx - 1];
            parent.data[idx - 1] = left.data[left.size - 1];
            node.children[0] = left.children[left.size];
            Node child;
            tree.read(child, node.children[0]);
            child.parent = pos;
            tree.update(child, node.children[0]);
        }
        node.size++;
        left.size--;
        tree.update(node,pos);
        tree.update(left, left_pos);
        tree.update(parent, parent_pos);
        return true;
    }

    bool borrow_from_right(int pos, Node& node, int parent_pos, Node& parent, int idx) {
        if (idx >= parent.size) {
            return false;
        }
        int right_pos = parent.children[idx + 1];
        Node right;
        tree.read(right, right_pos);
        if (right.size == MIN_SIZE) {
            return false;
        }
        if (node.leaf) {
            node.data[node.size] = right.data[0];
            for (int i = 0; i < right.size - 1; ++i) {
                right.data[i] = right.data[i + 1];
            }
            parent.data[idx] = right.data[0];
        }
        else {
            node.data[node.size] = parent.data[idx];
            node.children[node.size + 1] = right.children[0];
            Node child;
            tree.read(child, node.children[node.size + 1]);
            child.parent = pos;
            tree.update(child,node.children[node.size + 1]);
            parent.data[idx] = right.data[0];
            for (int i = 0; i < right.size; ++i) {
                right.data[i] = right.data[i + 1];
                right.children[i] = right.children[i + 1];
            }
            right.children[right.size - 1] = right.children[right.size];
        }
        node.size++;
        right.size--;
        tree.update(node,pos);
        tree.update(right,right_pos);
        tree.update(parent,parent_pos);
        return true;
    }

    void merge(int left_pos, int right_pos, int parent_pos, int idx) {
        Node left;
        tree.read(left, left_pos);
        Node right;
        tree.read(right, right_pos);
        Node parent;
        tree.read(parent, parent_pos);
        if (left.leaf) {
            for (int i = 0; i < right.size; ++i) {
                left.data[left.size + i] = right.data[i];
            }
            left.right = right.right;
            if (right.right != -1) {
                Node next;
                tree.read(next, right.right);
                next.left = left_pos;
                tree.update(next, right.right);
            }
        }
        else {
            left.data[left.size] = parent.data[idx];
            for (int i = 0; i < right.size; ++i) {
                left.data[left.size + 1 + i] = right.data[i];
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
            parent.data[i] = parent.data[i + 1];
            parent.children[i + 1] = parent.children[i + 2];
        }
        parent.size--;
        tree.update(left,left_pos);
        tree.update(parent, parent_pos);
        balance(parent_pos, parent);
    }

    void balance(int pos, Node& node) {
        if (pos == root) {
            if (!node.leaf && node.size == 0) {
                root = node.children[0];
                tree.write_info(root, 1);
                Node new_root;
                tree.read(new_root,root);
                new_root.parent = -1;
                tree.update(new_root,root);
            }
            return;
        }
        if (node.size >= MIN_SIZE) {
            return;
        }
        int parent_pos = node.parent;
        Node parent;
        tree.read(parent,parent_pos);
        int idx = 0;
        while (idx <= parent.size && parent.children[idx] != pos) {
            idx++;
        }
        if (borrow_from_left(pos, node, parent_pos, parent, idx)) {
            return;
        }
        if (borrow_from_right(pos, node, parent_pos, parent, idx)) {
            return;
        }
        if (idx > 0) {
            merge(parent.children[idx - 1], pos, parent_pos, idx - 1);
        }
        else {
            merge(pos, parent.children[idx + 1], parent_pos, idx);
        }
    }

    void insert(const Data& data) {
        if (root == 0) {
            Node new_node;
            new_node.leaf = true;
            new_node.size = 1;
            new_node.data[0] = data;
            root = tree.write(new_node);
            tree.write_info(root, 1);
            return;
        }
        int cur = root;
        Node Node;
        while (true) {
            tree.read(Node, cur);
            if (Node.leaf) {
                break;
            }
            int idx = 0;
            while (idx < Node.size && !(data < Node.data[idx])) {
                idx++;
            }
            cur = Node.children[idx];
        }
        for (int i = 0; i < Node.size; ++i) {
            if (Node.data[i] == data) {
                return;
            }
        }
        int pos = 0;
        for (int j = Node.size - 1; j >= 0; --j) {
            if (Node.data[j] < data) {
                pos = j + 1;
                break;
            }
            Node.data[j + 1] = Node.data[j];
        }
        Node.data[pos] = data;
        Node.size++;
        tree.update(Node, cur);
        if (Node.size == M) {
            split(cur, Node);
        }
    }

    void remove(const Data& data) {
        if (root == 0) {
            return;
        }
        int cur = root;
        Node Node;
        while (true) {
            tree.read(Node, cur);
            if (Node.leaf) {
                break;
            }
            int idx = 0;
            while (idx < Node.size && !(data < Node.data[idx])) {
                idx++;
            }
            cur = Node.children[idx];
        }
        for (int i = 0; i < Node.size; ++i) {
            if (Node.data[i] == data) {
                for (int j = i; j < Node.size - 1; ++j) {
                    Node.data[j] = Node.data[j + 1];
                }
                Node.size--;
                tree.update(Node,cur);
                balance(cur, Node);
                return;
            }
        }
    }

    void find(char key[65]) {
        if (root == 0) {
            std::cout << "null\n";
            return;
        }
        int cur = root;
        Node Node;
        while (true) {
            tree.read(Node, cur);
            if (Node.leaf) {
                break;
            }
            int idx = 0;
            while (idx < Node.size && std::strcmp(key, Node.data[idx].index) > 0) {
                idx++;
            }
            cur = Node.children[idx];
        }
        std::vector<int> res;
        bool found = false;
        bool flag = false;
        while (cur != -1 && !flag) {
            tree.read(Node, cur);
            for (int i = 0; i < Node.size; ++i) {
                int cmp = std::strcmp(key, Node.data[i].index);
                if (cmp == 0) {
                    res.push_back(Node.data[i].value);
                    found = true;
                }
                else if (cmp < 0) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                cur = Node.right;
            }
        }
        if (!found) {
            std::cout << "null\n";
        }
        else {
            for (int i = 0; i < res.size(); ++i) {
                std::cout << res[i];
                if (i != res.size() - 1) {
                    std::cout << " ";
                }
            }
            std::cout << '\n';
        }
    }

};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    BPlusTree BPT("BPT");
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "insert") {
            char index_[64];
            int value_;
            std::cin >> index_;
            std::cin >> value_;
            Data data = Data(index_, value_);
            BPT.insert(data);
        }
        else if (cmd == "delete") {
            char index_[64];
            int value_;
            std::cin >> index_;
            std::cin >> value_;
            Data data = Data(index_, value_);
            BPT.remove(data);
        }
        else if (cmd == "find") {
            char index_[65];
            std::cin >> index_;
            BPT.find(index_);
        }
    }
    return 0;
}