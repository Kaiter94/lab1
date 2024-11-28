#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class DoublyLinkedList {
private:
    struct Node {
        int value;
        Node* next;
        Node* prev;
        Node(int v) : value(v), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~DoublyLinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void LPUSH(int value) {
        Node* newNode = new Node(value);
        newNode->next = head;
        if (head) head->prev = newNode;
        head = newNode;
        if (!tail) tail = newNode;
    }

    void LAPPEND(int value) {
        Node* newNode = new Node(value);
        if (!tail) {
            head = tail = newNode;
            return;
        }
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    void LPOP() {
        if (!head) {
            cout << "Список пуст, удаление невозможно." << endl;
            return;
        }
        Node* temp = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
        delete temp;
    }

    void LREMOVE_LAST() {
        if (!tail) {
            cout << "Список пуст, удаление невозможно." << endl;
            return;
        }
        Node* temp = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr;
        delete temp;
    }

    void LDEL(int value) {
        Node* temp = head;
        while (temp && temp->value != value) temp = temp->next;
        if (!temp) return;
        if (temp->prev) temp->prev->next = temp->next;
        if (temp->next) temp->next->prev = temp->prev;
        if (temp == head) head = temp->next;
        if (temp == tail) tail = temp->prev;
        delete temp;
    }

    bool LSEARCH(int value) {
        Node* temp = head;
        while (temp) {
            if (temp->value == value) return true;
            temp = temp->next;
        }
        return false;
    }

    void PRINT() {
        cout << "Список: ";
        Node* temp = head;
        while (temp) {
            cout << temp->value << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json j;
        Node* temp = head;
        while (temp) {
            j["list"].push_back(temp->value);
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
            if (j.contains("list")) {
                for (const auto& val : j["list"]) {
                    LAPPEND(val);
                }
            }
        }
    }
};

void processCommand(DoublyLinkedList& list, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "LPUSH") {
        int value;
        ss >> value;
        list.LPUSH(value);
    } else if (operation == "LAPPEND") {
        int value;
        ss >> value;
        list.LAPPEND(value);
    } else if (operation == "LPOP") {
        list.LPOP();
    } else if (operation == "LREMOVE_LAST") {
        list.LREMOVE_LAST();
    } else if (operation == "LDEL") {
        int value;
        ss >> value;
        list.LDEL(value);
    } else if (operation == "LSEARCH") {
        int value;
        ss >> value;
        cout << (list.LSEARCH(value) ? "Найдено" : "Не найдено") << endl;
    } else if (operation == "PRINT") {
        list.PRINT();
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./list --file <имя_файла> --query '<команда>'\n";
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

    DoublyLinkedList list;
    list.loadFromJson(filename);
    processCommand(list, filename, command);
    list.saveToJson(filename);

    return 0;
}
