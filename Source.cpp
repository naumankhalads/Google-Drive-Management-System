#include<iostream>
#include<string>
#include<ctime>
#include<windows.h>
#include<iomanip>
using namespace std;
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
class File;
class LinkedList;
class LinkedList {
private:
    struct Version {
        string content;
        string timestamp;
        Version* prev;
        Version* next;
    };

    Version* head;
    Version* current;

public:
    LinkedList(string initialContent) {
        Version* newVersion = new Version;
        newVersion->content = initialContent;
        newVersion->timestamp = getCurrentDateTime();
        newVersion->prev = nullptr;
        newVersion->next = nullptr;

        head = newVersion;
        current = newVersion;
    }

    void addVersion(string content) {
        Version* newVersion = new Version;
        newVersion->content = content;
        newVersion->timestamp = getCurrentDateTime();
        newVersion->prev = current;
        newVersion->next = nullptr;

        current->next = newVersion;
        current = newVersion;

        cout << "New version added at " << newVersion->timestamp << "." << endl;
    }

    void rollback() {
        if (current->prev == nullptr) {
            cout << "Already at the oldest version. Cannot rollback further." << endl;
            return;
        }

        current = current->prev;
        current->next = nullptr;
        cout << "Rolled back to version created at " << current->timestamp << "." << endl;
    }

    void showHistory() {
        Version* temp = head;
        cout << "Version History:" << endl;
        while (temp != nullptr) {
            cout << " - " << temp->timestamp << ": " << temp->content << endl;
            temp = temp->next;
        }
    }

private:
    string getCurrentDateTime() {
        time_t now = time(0);
        char dt[26];
        ctime_s(dt, sizeof(dt), &now);
        string result = "";
        for (int i = 0; dt[i] != '\0'; i++) {
            if (dt[i] != '\n') result += dt[i];
        }
        return result;
    }
};
class File {
public:
    string name, type, owner, content;
    int size;
    string creationDate;
    LinkedList* versions;
    File* nextFile;

    File(string fileName, string fileType, string fileOwner, string fileContent) {
        name = fileName;
        type = fileType;
        owner = fileOwner;
        content = fileContent;
        size = fileContent.length();
        creationDate = getCurrentDateTime();
        versions = new LinkedList(fileContent);
        nextFile = nullptr;
    }

    void updateContent(string newContent) {
        content = newContent;
        size = newContent.length();
        versions->addVersion(newContent);
        cout << "File \"" << name << "\" updated successfully." << endl;
    }

    void displayContent() {
        cout << "File Name: " << name << endl;
        cout << "Type: " << type << endl;
        cout << "Owner: " << owner << endl;
        cout << "Size: " << size << " characters" << endl;
        cout << "Created on: " << creationDate << endl;
        cout << "Content: " << content << endl;
    }

private:
    string getCurrentDateTime() {
        time_t now = time(0);
        char dt[26];
        ctime_s(dt, sizeof(dt), &now);
        string result = "";
        for (int i = 0; dt[i] != '\0'; i++) {
            if (dt[i] != '\n') result += dt[i];
        }
        return result;
    }
};
class Folder {
public:
    string name;
    Folder* parent;
    Folder* firstChild;
    Folder* nextSibling;
    File* firstFile;

    Folder(string folderName, Folder* parentFolder = nullptr) {
        name = folderName;
        parent = parentFolder;
        firstChild = nullptr;
        nextSibling = nullptr;
        firstFile = nullptr;
    }

    void createSubFolder(string subName) {
        Folder* newFolder = new Folder(subName, this);
        newFolder->nextSibling = firstChild;
        firstChild = newFolder;
        cout << "Folder \"" << subName << "\" created inside \"" << name << "\"." << endl;
    }

    void deleteSubFolder(string subName) {
        Folder* current = firstChild;
        Folder* prev = nullptr;

        while (current != nullptr) {
            if (current->name == subName) {
                if (prev == nullptr) {
                    firstChild = current->nextSibling;
                }
                else {
                    prev->nextSibling = current->nextSibling;
                }
                delete current;
                cout << "Folder \"" << subName << "\" deleted from \"" << name << "\"." << endl;
                return;
            }
            prev = current;
            current = current->nextSibling;
        }

        cout << "Subfolder \"" << subName << "\" not found in \"" << name << "\"." << endl;
    }

    Folder* navigateTo(string subName) {
        Folder* current = firstChild;
        while (current != nullptr) {
            if (current->name == subName) {
                return current;
            }
            current = current->nextSibling;
        }
        cout << "Folder \"" << subName << "\" not found in \"" << name << "\"." << endl;
        return nullptr;
    }

    void listContents() {
        cout << "Contents of \"" << name << "\":" << endl;
        Folder* current = firstChild;
        while (current != nullptr) {
            cout << "  [Folder] " << current->name << endl;
            current = current->nextSibling;
        }

        File* f = firstFile;
        while (f != nullptr) {
            cout << "  [File] " << f->name << endl;
            f = f->nextFile;
        }
    }
};



class RecycleBinStack {
private:
    struct Node {
        File* file;
        Node* next;
    };
    Node* top;

public:
    RecycleBinStack() {
        top = nullptr;
    }

    void push(File* file) {
        Node* newNode = new Node;
        newNode->file = file;
        newNode->next = top;
        top = newNode;
        cout << "File \"" << file->name << "\" moved to Recycle Bin." << endl;
    }

    File* pop() {
        if (isEmpty()) {
            cout << "Recycle Bin is empty." << endl;
            return nullptr;
        }
        Node* temp = top;
        File* restoredFile = temp->file;
        top = top->next;
        delete temp;
        cout << "File \"" << restoredFile->name << "\" restored from Recycle Bin." << endl;
        return restoredFile;
    }

    bool isEmpty() {
        return top == nullptr;
    }

    void displayDeletedFiles() {
        if (isEmpty()) {
            cout << "Recycle Bin is empty." << endl;
            return;
        }

        cout << "Deleted Files in Recycle Bin:" << endl;
        Node* current = top;
        while (current != nullptr) {
            cout << "  [File] " << current->file->name << endl;
            current = current->next;
        }
    }
};
class RecentFilesQueue {
private:
    struct Node {
        File* file;
        Node* next;
    };
    Node* front;
    Node* rear;
    int capacity;
    int size;

public:
    RecentFilesQueue(int cap) {
        front = rear = nullptr;
        capacity = cap;
        size = 0;
    }

    void enqueue(File* file) {
        if (size == capacity) {
            dequeue();
        }

        Node* newNode = new Node;
        newNode->file = file;
        newNode->next = nullptr;

        if (rear == nullptr) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }

        size++;
        cout << "File \"" << file->name << "\" added to recent files." << endl;
    }

    File* dequeue() {
        if (front == nullptr) {
            return nullptr;
        }

        Node* temp = front;
        File* file = temp->file;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        delete temp;
        size--;
        return file;
    }

    void displayRecentFiles() {
        if (front == nullptr) {
            cout << "No recent files found." << endl;
            return;
        }

        cout << "Recent Files (from oldest to newest):" << endl;
        Node* current = front;
        while (current != nullptr) {
            cout << "  [File] " << current->file->name << endl;
            current = current->next;
        }
    }
};

class User {
public:
    string username;
    string password;
    string role;
    string securityQuestion;
    string lastLogin;
    string lastLogout;
    File* ownedFiles;
    User* next;

    User(string uname, string pwd, string r, string secQ) {
        username = uname;
        password = pwd;
        role = r;
        securityQuestion = secQ;
        lastLogin = getCurrentDateTime();
        lastLogout = "";
        ownedFiles = nullptr;
        next = nullptr;
    }

private:
    string getCurrentDateTime() {
        time_t now = time(0);
        char dt[26];
        ctime_s(dt, sizeof(dt), &now);
        string result = "";
        for (int i = 0; dt[i] != '\0'; i++) {
            if (dt[i] != '\n') result += dt[i];
        }
        return result;
    }
};

class UserGraph {
private:
    struct Connection {
        User* user;
        Connection* next;
    };

    struct UserNode {
        User* user;
        Connection* connections;
        UserNode* next;
    };

    UserNode* head;

public:
    UserGraph() {
        head = nullptr;
    }

    void addUser(string uname, string pwd, string role, string secQ) {
        if (findUserNode(uname) != nullptr) {
            cout << "User \"" << uname << "\" already exists." << endl;
            return;
        }

        User* newUser = new User(uname, pwd, role, secQ);
        UserNode* newNode = new UserNode;
        newNode->user = newUser;
        newNode->connections = nullptr;
        newNode->next = head;
        head = newNode;

        cout << "User \"" << uname << "\" added successfully." << endl;
    }

    User* authenticate(string uname, string pwd) {
        UserNode* temp = head;
        while (temp != nullptr) {
            if (temp->user->username == uname && temp->user->password == pwd) {
                temp->user->lastLogin = getCurrentDateTime();
                cout << "Login successful for \"" << uname << "\"." << endl;
                return temp->user;
            }
            temp = temp->next;
        }
        cout << "Authentication failed for \"" << uname << "\"." << endl;
        return nullptr;
    }

    void shareFile(User* fromUser, User* toUser, File* file) {
        UserNode* fromNode = findUserNode(fromUser->username);
        if (fromNode == nullptr) return;

        Connection* newConn = new Connection;
        newConn->user = toUser;
        newConn->next = fromNode->connections;
        fromNode->connections = newConn;

        cout << "File \"" << file->name << "\" shared from \"" << fromUser->username << "\" to \"" << toUser->username << "\"." << endl;
    }

    void showConnections(User* user) {
        UserNode* node = findUserNode(user->username);
        if (node == nullptr || node->connections == nullptr) {
            cout << "No connections found for \"" << user->username << "\"." << endl;
            return;
        }

        cout << "Connections for user \"" << user->username << "\":" << endl;
        Connection* conn = node->connections;
        while (conn != nullptr) {
            cout << "  → " << conn->user->username << endl;
            conn = conn->next;
        }
    }

private:
    UserNode* findUserNode(string uname) {
        UserNode* temp = head;
        while (temp != nullptr) {
            if (temp->user->username == uname) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    string getCurrentDateTime() {
        time_t now = time(0);
        char dt[26];
        ctime_s(dt, sizeof(dt), &now);
        string result = "";
        for (int i = 0; dt[i] != '\0'; i++) {
            if (dt[i] != '\n') result += dt[i];
        }
        return result;
    }
};
class AVLNode {
public:
    string key;
    File* file;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(string k, File* f) {
        key = k;
        file = f;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

class AVLTree {
public:
    AVLNode* root;

    AVLTree() {
        root = nullptr;
    }

    int getHeight(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return node->height;
    }

    int getBalance(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;


        x->right = y;
        y->left = T2;


        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));


        return x;
    }

    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;


        y->left = x;
        x->right = T2;


        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));


        return y;
    }

    AVLNode* insert(AVLNode* node, string key, File* file) {
        if (node == nullptr)
            return new AVLNode(key, file);

        if (key < node->key)
            node->left = insert(node->left, key, file);
        else if (key > node->key)
            node->right = insert(node->right, key, file);
        else
            return node;


        node->height = 1 + max(getHeight(node->left), getHeight(node->right));


        int balance = getBalance(node);


        if (balance > 1 && key < node->left->key)
            return rotateRight(node);


        if (balance < -1 && key > node->right->key)
            return rotateLeft(node);


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

    File* search(AVLNode* node, string key) {
        if (node == nullptr)
            return nullptr;
        if (key == node->key)
            return node->file;
        else if (key < node->key)
            return search(node->left, key);
        else
            return search(node->right, key);
    }

    void inOrder(AVLNode* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << "[File Key] " << node->key << endl;
            inOrder(node->right);
        }
    }
};
class FileHeap {
private:
    File** heap;
    int capacity;
    int size;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index]->name > heap[parent]->name) {
                File* temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            }
            else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        while (index < size) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && heap[left]->name > heap[largest]->name)
                largest = left;
            if (right < size && heap[right]->name > heap[largest]->name)
                largest = right;

            if (largest != index) {
                File* temp = heap[index];
                heap[index] = heap[largest];
                heap[largest] = temp;
                index = largest;
            }
            else {
                break;
            }
        }
    }

public:
    FileHeap(int cap) {
        capacity = cap;
        size = 0;
        heap = new File * [capacity];
    }

    void insert(File* file) {
        if (size == capacity) {
            cout << "Heap is full. Cannot insert \"" << file->name << "\"." << endl;
            return;
        }

        heap[size] = file;
        heapifyUp(size);
        size++;

        cout << "File \"" << file->name << "\" inserted into heap." << endl;
    }

    File* extractMax() {
        if (size == 0) {
            cout << "Heap is empty." << endl;
            return nullptr;
        }

        File* maxFile = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);

        cout << "File \"" << maxFile->name << "\" extracted from heap (highest priority)." << endl;
        return maxFile;
    }

    void displayHeap() {
        if (size == 0) {
            cout << "Heap is empty." << endl;
            return;
        }

        cout << "Files in Heap (unordered view):" << endl;
        for (int i = 0; i < size; i++) {
            cout << "  " << heap[i]->name << endl;
        }
    }
};
class AccessControl {
private:
    struct Permission {
        string filename;
        string accessType;
        Permission* next;
    };

    struct UserPermission {
        string username;
        Permission* permissions;
        UserPermission* next;
    };

    UserPermission* head;

public:
    AccessControl() {
        head = nullptr;
    }

    void grantPermission(string username, string filename, string access) {
        UserPermission* user = findUser(username);

        if (user == nullptr) {
            user = new UserPermission;
            user->username = username;
            user->permissions = nullptr;
            user->next = head;
            head = user;
        }


        Permission* perm = user->permissions;
        while (perm != nullptr) {
            if (perm->filename == filename) {
                perm->accessType = access;
                cout << "Updated permission for \"" << username << "\" on \"" << filename << "\" to \"" << access << "\"." << endl;
                return;
            }
            perm = perm->next;
        }


        Permission* newPerm = new Permission;
        newPerm->filename = filename;
        newPerm->accessType = access;
        newPerm->next = user->permissions;
        user->permissions = newPerm;

        cout << "Granted \"" << access << "\" access to \"" << username << "\" for file \"" << filename << "\"." << endl;
    }

    bool checkPermission(string username, string filename, string access) {
        UserPermission* user = findUser(username);
        if (user == nullptr) {
            return false;
        }

        Permission* perm = user->permissions;
        while (perm != nullptr) {
            if (perm->filename == filename && perm->accessType == access) {
                return true;
            }
            perm = perm->next;
        }

        return false;
    }

private:
    UserPermission* findUser(string username) {
        UserPermission* temp = head;
        while (temp != nullptr) {
            if (temp->username == username) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
};
class Utils {
public:
    static string getCurrentDate() {
        time_t now = time(0);
        tm localTimeStruct;
        tm* localTime = &localTimeStruct;
        localtime_s(localTime, &now);

        int year = 1900 + localTime->tm_year;
        int month = 1 + localTime->tm_mon;
        int day = localTime->tm_mday;

        string date = to_string(year) + "-";
        if (month < 10) date += "0";
        date += to_string(month) + "-";
        if (day < 10) date += "0";
        date += to_string(day);

        return date;
    }

    static string getCurrentTime() {
        time_t now = time(0);
        tm localTimeStruct;
        tm* localTime = &localTimeStruct;
        localtime_s(localTime, &now);

        int hour = localTime->tm_hour;
        int min = localTime->tm_min;
        int sec = localTime->tm_sec;

        string timeStr = "";
        if (hour < 10) timeStr += "0";
        timeStr += to_string(hour) + ":";
        if (min < 10) timeStr += "0";
        timeStr += to_string(min) + ":";
        if (sec < 10) timeStr += "0";
        timeStr += to_string(sec);

        return timeStr;
    }
};
int main() {
    Folder* root = new Folder("Root");
    Folder* current = root;
    RecycleBinStack recycleBin;
    RecentFilesQueue recentFiles(5);
    AVLTree indexTree;
    AccessControl accessControl;
    UserGraph userGraph;
    User* loggedInUser = nullptr;

    while (!loggedInUser) {
        cout << "================== LOGIN / REGISTER ==================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        int loginChoice;
        cin >> loginChoice;
        cin.ignore();

        string username, password, role, secQ;
        if (loginChoice == 1) {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            loggedInUser = userGraph.authenticate(username, password);
            if (!loggedInUser) {
                cout << "Login failed. Try again." << endl;
            }
        }
        else if (loginChoice == 2) {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            while (true) {
                cout << "Enter role (admin/editor/viewer): ";
                cin >> role;
                if (role == "admin" || role == "editor" || role == "viewer") {
                    break;
                }
                cout << "Invalid role. Try again." << endl;
            }

            cout << "Enter security question: ";
            cin.ignore();
            getline(cin, secQ);

            userGraph.addUser(username, password, role, secQ);
            loggedInUser = userGraph.authenticate(username, password);


        }
        else if (loginChoice == 3) {
            cout << " GoodBye (- -)" << endl;
            system("pause");
            return 0;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
        system("cls");
    }

    int choice;
    while (true) {

        setColor(11);
        cout << "1. Create Subfolder" << endl;
        cout << "2. Navigate to Subfolder" << endl;
        cout << "3. Go to Parent Folder" << endl;
        cout << "4. List Folder Contents" << endl;
        cout << "5. Create File" << endl;
        cout << "6. Display File" << endl;
        cout << "7. Update File Content" << endl;
        cout << "8. Delete File (to Recycle Bin)" << endl;
        cout << "9. Restore File from Recycle Bin" << endl;
        cout << "10. View Recent Files" << endl;
        cout << "11. Show File Version History" << endl;
        cout << "12. Search File by Name (AVL)" << endl;
        cout << "13. Share File" << endl;
        cout << "14. Grant Access" << endl;
        cout << "15. Check Access" << endl;
        cout << "16. Logout" << endl;
        cout << "==========================================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.get();
        system("cls");

        string name, content, username, password, role, secQ, access;

        switch (choice) {


        case 1: {
            cout << "Enter subfolder name: ";

            bool valid = false;
            while (!valid) {
                cin >> name;
                valid = true;


                for (char ch : name) {
                    if (!(isalnum(ch) || ch == '_')) {
                        valid = false;
                        break;
                    }
                }


                if (!valid) {
                    cout << "Invalid name! Use only letters, digits, or underscores. Try again: ";
                }
                else if (name.empty()) {
                    cout << "Name cannot be empty. Try again: ";
                    valid = false;
                }
            }

            current->createSubFolder(name);
            Folder* temp = current->firstChild;
            bool folderExists = false;
            while (temp != nullptr) {
                if (temp->name == name) {
                    folderExists = true;
                    break;
                }
                temp = temp->nextSibling;
            }
            if (folderExists) {
                cout << "A subfolder named \"" << name << "\" already exists in this folder. Try a different name." << endl;
                break;
            }

            cout << "Subfolder '" << name << "' has been successfully created.\n";
            cin.ignore();
            cin.get();
            system("cls");
            break;
        }



        case 2: {
            cout << "Enter folder name to navigate: ";
            cin >> name;

            Folder* next = current->navigateTo(name);
            if (next) {
                current = next;
                cout << "Successfully navigated to folder '" << name << "'.\n";
            }
            else {
                cout << "Folder '" << name << "' does not exist. Navigation failed.\n";
            }

            cin.ignore();
            cin.get();
            system("cls");
            break;
        }


        case 3: {
            if (current->parent != nullptr) {
                current = current->parent;
                cout << "Successfully moved to the parent folder: " << current->name << endl;
            }
            else {
                cout << "Already at the root folder. Cannot go further up." << endl;
            }

            cin.ignore();
            cin.get();
            system("cls");
            break;
        }


        case 4:
            current->listContents();
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 5:
            if (!loggedInUser) {
                cout << "You must login first." << endl;
                break;
            }
            cout << "Enter file name: ";
            cin >> name;
            cout << "Enter file type: ";
            cin >> role;
            cout << "Enter file content: ";
            cin.ignore();
            getline(cin, content);
            {
                File* temp = current->firstFile;
                bool fileExists = false;
                while (temp != nullptr) {
                    if (temp->name == name) {
                        fileExists = true;
                        break;
                    }
                    temp = temp->nextFile;
                }
                if (fileExists) {
                    cout << "File with the name \"" << name << "\" already exists in this folder. Try a different name." << endl;
                    break;
                }

                File* newFile = new File(name, role, loggedInUser->username, content);
                newFile->nextFile = current->firstFile;
                current->firstFile = newFile;

                indexTree.root = indexTree.insert(indexTree.root, name, newFile);
                loggedInUser->ownedFiles = newFile;
                cout << "File created and added to AVL index." << endl;

            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 6:
            cout << "Enter file name to display: ";
            cin >> name;
            {
                File* f = indexTree.search(indexTree.root, name);
                if (f) {
                    f->displayContent();
                    recentFiles.enqueue(f);
                }
                else {
                    cout << "File not found." << endl;
                }
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 7:
            cout << "Enter file name to update: ";
            cin >> name;
            {
                File* f = indexTree.search(indexTree.root, name);
                if (f) {
                    cout << "Enter new content: ";
                    cin.ignore();
                    getline(cin, content);
                    f->updateContent(content);
                }
                else {
                    cout << "File not found." << endl;
                }
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 8:
            cout << "Enter file name to delete: ";
            cin >> name;
            {
                File** prev = &current->firstFile;
                while (*prev != nullptr) {
                    if ((*prev)->name == name) {
                        File* toDelete = *prev;
                        *prev = (*prev)->nextFile;
                        recycleBin.push(toDelete);
                        break;
                    }
                    prev = &((*prev)->nextFile);
                }
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 9:
        {
            File* restored = recycleBin.pop();
            if (restored != nullptr) {
                restored->nextFile = current->firstFile;
                current->firstFile = restored;
            }
        }
        cin.ignore();
        cin.get();
        system("cls");
        break;

        case 10:
            recentFiles.displayRecentFiles();
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 11:
            cout << "Enter file name to view versions: ";
            cin >> name;
            {
                File* f = indexTree.search(indexTree.root, name);
                if (f) f->versions->showHistory();
                else cout << "File not found." << endl;
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 12:
            cout << "Enter file name to search: ";
            cin >> name;
            {
                File* f = indexTree.search(indexTree.root, name);
                if (f) f->displayContent();
                else cout << "File not found." << endl;
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 13: {
            if (!loggedInUser) {
                cout << "Please log in first." << endl;
                break;
            }

            cout << "Enter username to share with: ";
            cin >> username;


            if (username.empty()) {
                cout << "Username cannot be empty. Please try again." << endl;
                break;
            }

            cout << "Enter file name to share: ";
            cin >> name;


            if (name.empty()) {
                cout << "File name cannot be empty. Please try again." << endl;
                break;
            }


            File* f = indexTree.search(indexTree.root, name);
            if (!f) {
                cout << "File not found in the index tree: " << name << endl;
            }
            else {
                cout << "File found: " << name << endl;
            }


            User* other = userGraph.authenticate(username, "");
            if (!other) {
                cout << "User not found: " << username << endl;
            }
            else {
                cout << "User authenticated: " << username << endl;
            }


            if (f && other) {

                userGraph.shareFile(loggedInUser, other, f);
                cout << "File '" << name << "' shared successfully with user '" << username << "'." << endl;
            }
            else {
                if (!f) {
                    cout << "File not found: " << name << endl;
                }
                if (!other) {
                    cout << "User not found: " << username << endl;
                }
            }

            cin.ignore();
            cin.get();
            system("cls");
            break;
        }



        case 14:
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter file name: ";
            cin >> name;
            cout << "Enter access type (read/write/execute): ";
            cin >> access;
            accessControl.grantPermission(username, name, access);
            cin.ignore();
            cin.get();
            system("cls");
            break;

        case 15:
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter file name: ";
            cin >> name;
            cout << "Enter access type to check: ";
            cin >> access;
            if (accessControl.checkPermission(username, name, access)) {
                cout << "ACCESS GRANTED." << endl;
            }
            else {
                cout << "ACCESS DENIED." << endl;
            }
            cin.ignore();
            cin.get();
            system("cls");
            break;
        case 16:
            if (loggedInUser) {
                loggedInUser->lastLogout = Utils::getCurrentTime();
                cout << "User \"" << loggedInUser->username << "\" logged out at " << loggedInUser->lastLogout << "." << endl;
                loggedInUser = nullptr;


                while (!loggedInUser) {
                    cout << "================== LOGIN / REGISTER ==================" << endl;
                    cout << "1. Login" << endl;
                    cout << "2. Register" << endl;
                    cout << "Enter choice: ";
                    int loginChoice;
                    cin >> loginChoice;
                    cin.ignore();

                    string username, password, role, secQ;
                    if (loginChoice == 1) {
                        cout << "Enter username: ";
                        cin >> username;
                        cout << "Enter password: ";
                        cin >> password;
                        loggedInUser = userGraph.authenticate(username, password);
                        if (!loggedInUser) {
                            cout << "Login failed. Try again.\n";
                        }
                    }
                    else if (loginChoice == 2) {
                        cout << "Enter username: ";
                        cin >> username;
                        cout << "Enter password: ";
                        cin >> password;

                        while (true) {
                            cout << "Enter role (admin/editor/viewer): ";
                            cin >> role;
                            if (role == "admin" || role == "editor" || role == "viewer") {
                                break;
                            }
                            cout << "Invalid role. Try again.\n";
                        }

                        cout << "Enter security question: ";
                        cin.ignore();
                        getline(cin, secQ);

                        userGraph.addUser(username, password, role, secQ);
                        loggedInUser = userGraph.authenticate(username, password);
                    }
                    else {
                        cout << "Invalid choice. Try again.\n";
                    }
                    system("cls");
                }
            }
            else {
                cout << "No user is currently logged in." << endl;
            }
            cin.get();
            system("cls");
            break;

        default:
            cout << "Invalid option. Please try again." << endl;
        }
    }
    system("pause");
    return 0;
}
