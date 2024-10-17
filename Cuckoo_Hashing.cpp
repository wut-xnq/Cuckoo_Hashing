#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream> // for std::istringstream
#include <cstring> // for std::strlen
using namespace std;

class CuckooHash {
private:
    int size;                     // 哈希表的大小
    int count;                    // 当前存储的元素数量
    vector<int> table1;           // 第一个哈希表
    vector<int> table2;           // 第二个哈希表
    int hash1(int key) { return key % size; } // 第一个哈希函数
    int hash2(int key) { return (key / size) % size; } // 第二个哈希函数

public:
    CuckooHash(int sz) : size(sz), count(0) {
        table1.resize(size, -1);
        table2.resize(size, -1);
    }

    void insert(int key) {
        if (count >= size) { // 检查当前元素数量是否达到容量
            throw runtime_error("Insertion failed. Hash table is full.");
        }

        int pos1 = hash1(key);
        if (table1[pos1] == -1) {
            table1[pos1] = key;
            count++; // 增加元素计数
            return;
        }

        int displaced = table1[pos1];
        table1[pos1] = key;

        for (int i = 0; i < size; ++i) {
            int pos2 = hash2(displaced);
            if (table2[pos2] == -1) {
                table2[pos2] = displaced;
                count++; // 增加元素计数
                return;
            }

            swap(displaced, table2[pos2]);
            pos1 = hash1(displaced);
            if (table1[pos1] == -1) {
                table1[pos1] = displaced;
                count++; // 增加元素计数
                return;
            }

            swap(displaced, table1[pos1]);
        }

        throw runtime_error("Cycle detected. Rehash required.");
    }

    bool find(int key) {
        return table1[hash1(key)] == key || table2[hash2(key)] == key;
    }

    void remove(int key) {
        int pos1 = hash1(key);
        if (table1[pos1] == key) {
            table1[pos1] = -1;
            count--; // 减少元素计数
            return;
        }

        int pos2 = hash2(key);
        if (table2[pos2] == key) {
            table2[pos2] = -1;
            count--; // 减少元素计数
        }
    }

    int getCount() const { // 获取当前元素数量
        return count;
    }
};

// 函数：清理输入字符串
void trim(string& str) {
    str.erase(0, str.find_first_not_of(" \n\r\t"));
    str.erase(str.find_last_not_of(" \n\r\t") + 1);
}

int main() {
    int size;
    cout << "Enter the size of the hash table: ";
    cin >> size;
    cin.ignore(); // 清除输入缓冲区中的换行符

    CuckooHash hashTable(size);
    string input;

    cout << "Please input a command: insert <key> <key> ..., find <key>, remove <key>, or exit" << endl;

    while (true) {
        cout << ">> ";
        getline(cin, input); // 获取整行输入
        trim(input); // 清理输入字符串

        if (input.empty()) {
            cout << "Error: Empty input. Please enter a valid command." << endl;
            continue;
        }

        stringstream ss(input); // 使用 stringstream 解析输入
        string command;
        ss >> command; // 获取命令

        if (command == "insert") {
            int key;
            while (ss >> key) { // 读取所有的键
                try {
                    hashTable.insert(key);
                    cout << "Success: " << key << " has been successfully inserted into the hash table." << endl;
                    cout << "Current number of elements: " << hashTable.getCount() << endl; // 显示当前元素数量
                    if (hashTable.getCount() < 0) {
                        cout << "Error: Element count is negative. Exiting program." << endl;
                        return 1; // 退出程序
                    }
                }
                catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << " for key " << key << ". Please try again." << endl;
                }
            }
        }
        else if (command == "find") {
            int key;
            ss >> key; // 读取查找的键
            if (hashTable.find(key)) {
                cout << "Success: " << key << " is present in the hash table." << endl;
            }
            else {
                cout << "Error: " << key << " is not found in the hash table." << endl;
            }
        }
        else if (command == "remove") {
            int key;
            ss >> key; // 读取删除的键
            if (hashTable.find(key)) {
                hashTable.remove(key);
                cout << "Removed " << key << endl;
                cout << "Current number of elements: " << hashTable.getCount() << endl; // 显示当前元素数量
                if (hashTable.getCount() < 0) {
                    cout << "Error: Element count is negative. Exiting program." << endl;
                    return 1; // 退出程序
                }
            }
            else {
                cout << "Error: " << key << " not found, cannot remove." << endl;
            }
        }
        else if (command == "exit") {
            break; // 退出循环
        }
        else {
            cout << "Unknown command. Available commands: insert <key> <key>..., find <key>, remove <key>, exit." << endl;
        }
    }
    return 0;
}
