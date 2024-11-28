#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class Queue {
private:
    struct Node {
        int value;
        Node* next;
        Node(int v) : value(v), next(nullptr) {}
    };

    Node* front;
    Node* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}

    ~Queue() {
        while (front) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
    }

    void QPUSH(int value) {
        Node* newNode = new Node(value);
        if (!rear) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    void QPOP() {
        if (!front) return;
        Node* temp = front;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
    }

    void PRINT() {
        cout << "Очередь: ";
        Node* temp = front;
        while (temp) {
            cout << temp->value << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json j;
        Node* temp = front;
        while (temp) {
            j["queue"].push_back(temp->value);
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
            if (j.contains("queue")) {
                for (const auto& val : j["queue"]) {
                    QPUSH(val);
                }
            }
        }
    }
};

void processCommand(Queue& queue, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "QPUSH") {
        int value;
        ss >> value;
        queue.QPUSH(value);
    } else if (operation == "QPOP") {
        queue.QPOP();
    } else if (operation == "PRINT") {
        queue.PRINT();
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./queue --file <имя_файла> --query '<команда>'\n";
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

    Queue queue;
    queue.loadFromJson(filename);
    processCommand(queue, filename, command);
    queue.saveToJson(filename);

    return 0;
}
