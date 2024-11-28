#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class AVLTree {
private:
    struct Node {
        int key;
        Node* left;
        Node* right;
        int height;
        Node(int k) : key(k), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int getHeight(Node* node) {
        return node ? node->height : 0;
    }

    int getBalance(Node* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    Node* insert(Node* node, int key) {
        if (!node) return new Node(key);
        if (key < node->key) node->left = insert(node->left, key);
        else if (key > node->key) node->right = insert(node->right, key);
        else return node;
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);
        if (balance > 1 && key < node->left->key) return rotateRight(node);
        if (balance < -1 && key > node->right->key) return rotateLeft(node);
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left) current = current->left;
        return current;
    }

    Node* deleteNode(Node* root, int key) {
        if (!root) return root;
        if (key < root->key) root->left = deleteNode(root->left, key);
        else if (key > root->key) root->right = deleteNode(root->right, key);
        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else *root = *temp;
                delete temp;
            } else {
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                root->right = deleteNode(root->right, temp->key);
            }
        }
        if (!root) return root;
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalance(root);
        if (balance > 1 && getBalance(root->left) >= 0) return rotateRight(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0) return rotateLeft(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        return root;
    }

    bool search(Node* node, int key) {
        if (!node) return false;
        if (key == node->key) return true;
        if (key < node->key) return search(node->left, key);
        else return search(node->right, key);
    }

    void toJson(Node* node, json& jNode) {
        if (!node) return;
        jNode["key"] = node->key;
        if (node->left) {
            jNode["left"] = json::object();
            toJson(node->left, jNode["left"]);
        }
        if (node->right) {
            jNode["right"] = json::object();
            toJson(node->right, jNode["right"]);
        }
    }

    Node* buildTreeFromJson(const json& jNode) {
        if (jNode.is_null()) return nullptr;
        Node* node = new Node(jNode["key"]);
        if (jNode.contains("left")) node->left = buildTreeFromJson(jNode["left"]);
        if (jNode.contains("right")) node->right = buildTreeFromJson(jNode["right"]);
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        return node;
    }

public:
    AVLTree() : root(nullptr) {}

    void TINSERT(int key) {
        root = insert(root, key);
    }

    void TDEL(int key) {
        root = deleteNode(root, key);
    }

    bool TGET(int key) {
        return search(root, key);
    }

    void saveToJson(const string& filename) {
        json treeJson;
        toJson(root, treeJson);
        ofstream file(filename);
        if (file.is_open()) {
            file << treeJson.dump(4);
            file.close();
        }
    }

    void loadFromJson(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            json treeJson;
            file >> treeJson;
            file.close();
            root = buildTreeFromJson(treeJson);
        }
    }
};

void processCommand(AVLTree& tree, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "TINSERT") {
        int key;
        ss >> key;
        tree.TINSERT(key);
    } else if (operation == "TDEL") {
        int key;
        ss >> key;
        tree.TDEL(key);
    } else if (operation == "TGET") {
        int key;
        ss >> key;
        cout << (tree.TGET(key) ? "Найдено" : "Не найдено") << endl;
    } else if (operation == "PRINT") {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            cout << "Содержимое дерева: " << endl;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cout << "Ошибка: невозможно открыть файл." << endl;
        }
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./tree --file <имя_файла> --query '<команда>'\n";
        return 1;
    }

    string fileArg = argv[1];
    string filename = argv[2];
    string queryArg = argv[3];
    string command = argv[4];

    if (fileArg != "--file" || queryArg != "--query") {
        cerr << "Неверные аргументы. Используйте --file и --query.\n";
        return 1;
    }

    AVLTree tree;
    tree.loadFromJson(filename);
    processCommand(tree, filename, command);
    tree.saveToJson(filename);

    return 0;
}

