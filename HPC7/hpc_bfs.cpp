// parallel_bfs.cpp
#include <iostream>
#include <queue>
#include <omp.h>
#include <iomanip>
#include <vector>

using namespace std;

// Node structure for tree
class Node {
public:
    int data;
    Node *left, *right;
    
    Node(int value) {
        data = value;
        left = right = nullptr;
    }
};

// BFS Tree traversal class
class ParallelBFS {
private:
    Node* root;
    
public:
    ParallelBFS() : root(nullptr) {}
    
    Node* insert(Node* root, int data) {
        if (!root) {
            return new Node(data);
        }
        
        queue<Node*> q;
        q.push(root);
        
        while (!q.empty()) {
            Node* temp = q.front();
            q.pop();
            
            if (!temp->left) {
                temp->left = new Node(data);
                return root;
            } else {
                q.push(temp->left);
            }
            
            if (!temp->right) {
                temp->right = new Node(data);
                return root;
            } else {
                q.push(temp->right);
            }
        }
        return root;
    }
    
    void bfs(Node* root) {
        if (!root) {
            cout << "Tree is empty!" << endl;
            return;
        }
        
        queue<Node*> q;
        q.push(root);
        
        int level = 0;
        
        cout << "\n===== Breadth First Search Traversal (Parallel) =====\n" << endl;
        
        while (!q.empty()) {
            int levelSize = q.size();
            vector<Node*> currentLevel;
            
            for (int i = 0; i < levelSize; i++) {
                currentLevel.push_back(q.front());
                q.pop();
            }
            
            cout << "Level " << level << ": ";
            
            #pragma omp parallel for
            for (int i = 0; i < levelSize; i++) {
                Node* current = currentLevel[i];
                
                #pragma omp critical
                {
                    cout << setw(4) << current->data << " ";
                }
                
                #pragma omp critical
                {
                    if (current->left)
                        q.push(current->left);
                    if (current->right)
                        q.push(current->right);
                }
            }
            
            cout << endl;
            level++;
        }
        
        cout << "\n====================================================" << endl;
    }
    
    Node* getRoot() { return root; }
    void setRoot(Node* newRoot) { root = newRoot; }
};

int main() {
    ParallelBFS bfsTree;
    Node* root = nullptr;
    
    int numThreads = 4;
    omp_set_num_threads(numThreads);
    
    cout << "Parallel BFS Tree Traversal" << endl;
    cout << "Using " << numThreads << " OpenMP threads" << endl;
    
    char choice;
    do {
        int data;
        cout << "\nEnter value to insert: ";
        cin >> data;
        
        root = bfsTree.insert(root, data);
        bfsTree.setRoot(root);
        
        cout << "Do you want to insert another node? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
    
    bfsTree.bfs(root);
    
    return 0;
}
