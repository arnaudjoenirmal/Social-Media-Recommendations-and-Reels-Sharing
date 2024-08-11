#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

class User {
public:
    string username;
    string password;

    User() : username(""), password("") {}

    User(string uname, string pwd) : username(uname), password(pwd) {}
};

class Graph {
public:
    unordered_map<string, unordered_map<string, int>> adjList;

    void addEdge(string user1, string user2, int weight = 1) {
        adjList[user1][user2] += weight;
        adjList[user2][user1] += weight;
    }

    vector<pair<string, int>> getFriends(string username) {
        vector<pair<string, int>> friends;
        for (const auto& [friendName, weight] : adjList[username]) {
            friends.push_back({friendName, weight});
        }
        return friends;
    }

    unordered_map<string, vector<pair<string, int>>> getFriendsOfFriends(string username) {
        unordered_map<string, vector<pair<string, int>>> friendsOfFriends;
        for (const auto& [friendName, weight] : adjList[username]) {
            for (const auto& [fof, fofWeight] : adjList[friendName]) {
                if (fof != username && adjList[username].find(fof) == adjList[username].end()) {
                    friendsOfFriends[fof].push_back({friendName, fofWeight});
                }
            }
        }
        return friendsOfFriends;
    }

    bool areFriends(string user1, string user2) {
        return adjList[user1].find(user2) != adjList[user1].end();
    }
};

template<typename T>
struct stree {
    stree *left, *right, *parent;
    T data;

public:
    stree(T d) : data(d) {
        left = right = parent = nullptr;
    }
};

template<typename T>
void displayTree(stree<T> *root, int space = 0) {
    if (root == nullptr)
        return;

    space += 10;

    displayTree(root->right, space);

    cout << endl;
    for (int i = 5; i < space; i++)
        cout << " ";
    cout << root->data.second << " (" << root->data.first << ")\n";

    displayTree(root->left, space);
}

template<typename T>
void right_rotation(stree<T> *&Tree, stree<T> *&x) {
    stree<T> *y = x->left;
    x->left = y->right;
    if (y->right != nullptr)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nullptr)
        Tree = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
}

template<typename T>
void left_rotation(stree<T> *&Tree, stree<T> *&x) {
    stree<T> *y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nullptr)
        Tree = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

template<typename T>
void splaying(stree<T> *&Tree, stree<T> *&node) {
    while (node->parent != nullptr) {
        stree<T> *parent = node->parent;
        stree<T> *grandparent = parent->parent;

        if (grandparent == nullptr) {
            if (node == parent->left) {
                right_rotation(Tree, parent);
                cout << endl << "Zig" << endl;
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;

            } else {
                left_rotation(Tree, parent);
                cout << endl << "Zag" << endl;
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
            }
        } else {
            if (node == parent->left && parent == grandparent->left) {
                cout << endl << "Zig-Zig" << endl;
                right_rotation(Tree, grandparent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
                right_rotation(Tree, parent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
            } else if (node == parent->right && parent == grandparent->right) {
                cout << endl << "Zag-Zag" << endl;
                left_rotation(Tree, grandparent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
                left_rotation(Tree, parent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
            } else if (node == parent->right && parent == grandparent->left) {
                cout << endl << "Zig-Zag" << endl;
                left_rotation(Tree, parent);
                displayTree(Tree);
                cout << "--------------------------------------------------------------------" << endl;
                right_rotation(Tree, grandparent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
            } else {
                cout << endl << "Zag-Zig" << endl;
                right_rotation(Tree, parent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
                left_rotation(Tree, grandparent);
                displayTree(Tree); 
                cout << "--------------------------------------------------------------------" << endl;
            }
        }
    }
}

template<typename T>
void insert_splay(stree<pair<string, T>> *&Tree, pair<string, T> key) {
    stree<pair<string, T>> *node = new stree<pair<string, T>>(key);
    stree<pair<string, T>> *parent = nullptr;
    stree<pair<string, T>> *temp = Tree;

    while (temp != nullptr) {
        parent = temp;
        if (key.second < temp->data.second)
            temp = temp->left;
        else
            temp = temp->right;
    }
    node->parent = parent;
    if (parent == nullptr) {
        Tree = node;
    } else if (key.second < parent->data.second) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    displayTree(Tree);
    cout << "--------------------------------------------------------------------" << endl; // Display the tree after insertion according to BST property
    splaying(Tree, node);
}

template<typename T>
stree<pair<string, T>>* find_max(stree<pair<string, T>>* node) {
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}

template<typename T>
void splay_max_element_to_root(stree<pair<string, T>> *&Tree) {
    stree<pair<string, T>>* max_node = find_max(Tree);
    splaying(Tree, max_node);
}

class SocialMedia {
private:
    unordered_map<string, User> users;
    Graph graph;

    void loadUsers() {
        ifstream file("users1.txt");
        string line, username, password;
        while (getline(file, line)) {
            stringstream ss(line);
            ss >> username >> password;
            users[username] = User(username, password);
        }
    }

    void loadFriendships() {
        ifstream file("friendships1.txt");
        string line, user1, user2;
        int weight;
        while (getline(file, line)) {
            stringstream ss(line);
            ss >> user1 >> user2 >> weight;
            graph.addEdge(user1, user2, weight);
        }
    }

    void saveUsers() {
        ofstream file("users1.txt");
        for (const auto& [username, user] : users) {
            file << username << " " << user.password << endl;
        }
    }

    void saveFriendships() {
        ofstream file("friendships1.txt");
        for (const auto& [user, friends] : graph.adjList) {
            for (const auto& [friendName, weight] : friends) {
                if (user < friendName) { 
                    file << user << " " << friendName << " " << weight << endl;
                }
            }
        }
    }

public:
    SocialMedia() {
        loadUsers();
        loadFriendships();
    }

    bool login(string username, string password) {
        return users.find(username) != users.end() && users[username].password == password;
    }

    void displayFriends(string username) {
        vector<pair<string, int>> friends = graph.getFriends(username);
        cout << "Friends of " << username << ": ";
        for (const auto& [friendName, weight] : friends) {
            cout << friendName << " (" << weight << ") ";
        }
        cout << endl;
    }

    void recommendFriends(string username) {
        unordered_map<string, vector<pair<string, int>>> recommendations = graph.getFriendsOfFriends(username);
        cout << endl << endl;
        cout << "Friend recommendations for " << username << ": \n";
        for (const auto& [friendName, interactions] : recommendations) {
            cout << friendName << " (interacted via: ";
            for (const auto& [mutualFriend, weight] : interactions) {
                //cout << mutualFriend << " (" << weight << " interactions), ";
                cout << mutualFriend << " " ;
            }
            cout << ")\n";
        }
        cout << endl;
    }

    void addFriend(string user1, string user2) {
        if (graph.getFriendsOfFriends(user1).find(user2) != graph.getFriendsOfFriends(user1).end() || users.find(user2) != users.end()) {
            graph.addEdge(user1, user2);
            saveFriendships();
            cout << "Friend added successfully!" << endl;
        } else {
            cout << "Error: Friend not in recommendations list." << endl;
        }
    }

    void addChat(string user1, string user2) {
        if (graph.areFriends(user1, user2)) {
            graph.addEdge(user1, user2, 1);
            saveFriendships(); 
            cout << "Chat added successfully!" << endl;
        } else {
            cout << "Error: Users are not friends." << endl;
        }
    }

    void addReel(string user1, string user2) {
        if (graph.areFriends(user1, user2)) {
            graph.addEdge(user1, user2, 5);
            saveFriendships(); // Save the updated friendship list to file
            cout << "Reel added successfully!" << endl;
        } else {
            cout << "Error: Users are not friends." << endl;
        }
    }

    void splayFriends(string username) {
        vector<pair<string, int>> friends = graph.getFriends(username);
        if (friends.empty()) {
            cout << "No friends to splay." << endl;
            return;
        }

        stree<pair<string, int>> *splay_tree = nullptr;

        for (const auto &friendPair : friends) {
            insert_splay(splay_tree, friendPair);
            cout << "--------------------------------------------------------------------" << endl;
        }

        cout << "Splaying the maximum weight friend to the root:" << endl;
        splay_max_element_to_root(splay_tree);
        displayTree(splay_tree);
    }

    void registerUser(string username, string password) {
        if (users.find(username) != users.end()) {
            cout << "Error: Username already exists." << endl;
            return;
        }
        users[username] = User(username, password);
        saveUsers();
        cout << "User registered successfully!" << endl;


        cout << "Welcome, " << username << "! Here are all existing users you can add as friends: " << endl;
        for (const auto& [uname, user] : users) {
            if (uname != username) {
                cout << uname << endl;
            }
        }
        while (true) {
            string newFriend;
            cout << "Enter the name of a friend to add (or 'no' to skip): ";
            cin >> newFriend;
            if (newFriend != "no") {
                addFriend(username, newFriend);
            } else {
                break;
            }
        }
    }
};

int main() {
    SocialMedia app;
    string username, password;

    while (true) {
        cout << "Welcome to Soxial Media" << endl << endl;
        cout << "Enter 'login' to log in \nEnter 'register' to create a new account \nEnter 'exit' to close the application\n";
        string choice;
        cin >> choice;

        if (choice == "login") {
            cout << endl << endl;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            if (app.login(username, password)) {
                cout << "Login successful!" << endl << endl;
                while (true) {
                    app.displayFriends(username);
                    app.splayFriends(username); // Display splay tree before adding a friend
                    
                    app.recommendFriends(username);

                    string newFriend;
                    cout << endl << endl;
                    cout << "Enter the name of a friend to add (or 'no' to skip): ";
                    cin >> newFriend;
                    if (newFriend != "no") {
                        app.addFriend(username, newFriend);
                    } else {
                        break;
                    }
                }

                while (true) {
                    app.splayFriends(username); // Display splay tree before adding a chat or reel

                    string action, friendName;
                    cout << "Enter 'chat' to send a chat\nEnter 'reel' to send a reel\nEnter 'logout' to logout\nEnter 'exit' to close the application\n";
                    cin >> action;
                    if (action == "exit") return 0;
                    if (action == "logout"){cout << endl << endl;break;}

                    cout << "Enter the friend's name: ";
                    cin >> friendName;

                    if (action == "chat") {
                        app.addChat(username, friendName);
                    } else if (action == "reel") {
                        app.addReel(username, friendName);
                    } else {
                        cout << "Invalid action." << endl;
                    }
                }

            } else {
                cout << "Login failed!" << endl;
            }
        } else if (choice == "register") {
            cout << endl << endl;
            cout << "Enter a new username: ";
            cin >> username;
            cout << "Enter a new password: ";
            cin >> password;
            app.registerUser(username, password);


            while (true) {
                app.displayFriends(username);
                app.recommendFriends(username);
                app.splayFriends(username); 
                app.recommendFriends(username);

                string newFriend;
                cout << endl << endl;
                cout << "Enter the name of a friend to add (or 'no' to skip): ";
                cin >> newFriend;
                if (newFriend != "no") {
                    app.addFriend(username, newFriend);
                } else {
                    break;
                }
            }

            
            while (true) {
                app.splayFriends(username);

                string action, friendName;
                cout << endl << endl;
                cout << "Enter 'chat' to add a chat\nEnter'reel' to add a reel\nEnter'logout' to logout\nEnter 'exit' to close the application\n";
                cin >> action;
                if (action == "exit") return 0;
                if (action == "logout") break;

                cout << "Enter the friend's name: ";
                cin >> friendName;

                if (action == "chat") {
                    app.addChat(username, friendName);
                } else if (action == "reel") {
                    app.addReel(username, friendName);
                } else {
                    cout << "Invalid action." << endl;
                }
            }
        } else if (choice == "exit") {
            break;
        } else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
