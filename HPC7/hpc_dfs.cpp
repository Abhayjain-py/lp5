// parallel_dfs.cpp
#include <iostream>
#include <omp.h>

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

// Depth-first search implementation
class ParallelDFS {
private:
    Node* root;
    
public:
    ParallelDFS() : root(nullptr) {}
    
    Node* insert(Node* root, int data) {
        if (!root) {
            return new Node(data);
        }
        
        if (data < root->data) {
            root->left = insert(root->left, data);
        } else {
            root->right = insert(root->right, data);
        }
        
        return root;
    }
    
    void dfs(Node* root) {
        if (!root) {
            cout << "Tree is empty!" << endl;
            return;
        }
        
        cout << "\n===== Depth First Search Traversal (Parallel) =====\n" << endl;
        
        #pragma omp parallel
        {
            #pragma omp single nowait
            {
                dfsRecursive(root, 0);
            }
        }
        
        cout << "\n===================================================" << endl;
    }
    
private:
    void dfsRecursive(Node* node, int level) {
        if (!node) return;
        
        #pragma omp critical
        {
            cout << "Level " << level << ": Node " << node->data << endl;
        }
        
        #pragma omp task
        {
            if (node->left)
                dfsRecursive(node->left, level + 1);
        }
        
        #pragma omp task
        {
            if (node->right)
                dfsRecursive(node->right, level + 1);
        }
        
        #pragma omp taskwait
    }
    
public:
    Node* getRoot() { return root; }
    void setRoot(Node* newRoot) { root = newRoot; }
};

int main() {
    ParallelDFS dfsTree;
    Node* root = nullptr;
    
    int numThreads = 4;
    omp_set_num_threads(numThreads);
    
    cout << "Parallel DFS Tree Traversal" << endl;
    cout << "Using " << numThreads << " OpenMP threads" << endl;
    
    char choice;
    do {
        int data;
        cout << "\nEnter value to insert: ";
        cin >> data;
        
        root = dfsTree.insert(root, data);
        dfsTree.setRoot(root);
        
        cout << "Do you want to insert another node? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
    
    dfsTree.dfs(root);
    
    return 0;
}
