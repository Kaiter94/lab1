#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class Stack {
private:
    struct Node {
        int value;
        Node* next;
        Node(int v) : value(v), next(nullptr) {}
    };

    Node* top;

public:
    Stack() : top(nullptr) {}

    ~Stack() {
        while (top) {
            Node* temp = top;
            top = top->next;
            delete temp;
        }
    }

    void SPUSH(int value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    void SPOP() {
        if (!top) return;
        Node* temp = top;
        top = top->next;
        delete temp;
    }

    void PRINT() {
        cout << "Стек: ";
        Node* temp = top;
        while (temp) {
            cout << temp->value << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json j;
        Node* temp = top;
        while (temp) {
            j["stack"].push_back(temp->value);
            temp = temp->next;
        }
        ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
        }
    }

    void loadFromJson(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            json j;
            file >> j;
            file.close();
            if (j.contains("stack")) {
                for (auto it = j["stack"].rbegin(); it != j["stack"].rend(); ++it) {
                    SPUSH(*it);
                }
            }
        }
    }
};

void processCommand(Stack& stack, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "SPUSH") {
        int value;
        ss >> value;
        stack.SPUSH(value);
    } else if (operation == "SPOP") {
        stack.SPOP();
    } else if (operation == "PRINT") {
        stack.PRINT();
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./stack --file <имя_файла> --query '<команда>'\n";
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

    Stack stack;
    stack.loadFromJson(filename);
    processCommand(stack, filename, command);
    stack.saveToJson(filename);

    return 0;
}
