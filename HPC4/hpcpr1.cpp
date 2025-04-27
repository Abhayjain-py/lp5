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
    
    // Constructor
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
    
    // Insert a node in level order
    Node* insert(Node* root, int data) {
        // If tree is empty, create a new node as root
        if (!root) {
            return new Node(data);
        }
        
        queue<Node*> q;
        q.push(root);
        
        // Level order traversal to find the first vacant position
        while (!q.empty()) {
            Node* temp = q.front();
            q.pop();
            
            // If left child is empty, insert here
            if (!temp->left) {
                temp->left = new Node(data);
                return root;
            } else {
                q.push(temp->left);
            }
            
            // If right child is empty, insert here
            if (!temp->right) {
                temp->right = new Node(data);
                return root;
            } else {
                q.push(temp->right);
            }
        }
        return root; // Should never reach here in a proper binary tree
    }
    
    // Breadth-first traversal with parallel processing of each level
    void bfs(Node* root) {
        if (!root) {
            cout << "Tree is empty!" << endl;
            return;
        }
        
        queue<Node*> q;
        q.push(root);
        
        int level = 0;
        
        cout << "\n===== Breadth First Search Traversal (Parallel) =====\n" << endl;
        
        // Process level by level
        while (!q.empty()) {
            int levelSize = q.size();
            vector<Node*> currentLevel;
            
            // Extract all nodes at current level
            for (int i = 0; i < levelSize; i++) {
                currentLevel.push_back(q.front());
                q.pop();
            }
            
            cout << "Level " << level << ": ";
            
            // Process nodes at current level in parallel
            #pragma omp parallel for
            for (int i = 0; i < levelSize; i++) {
                Node* current = currentLevel[i];
                
                // Critical section for output to prevent garbled text
                #pragma omp critical
                {
                    cout << setw(4) << current->data << " ";
                }
                
                // Add children to queue (needs to be in critical section to avoid race conditions)
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
    
    // Getter and setter for root
    Node* getRoot() { return root; }
    void setRoot(Node* newRoot) { root = newRoot; }
};

// Depth-first search implementation (added to complement BFS)
class ParallelDFS {
public:
    // Function to perform parallel DFS traversal
    void dfs(Node* root) {
        if (!root) {
            cout << "Tree is empty!" << endl;
            return;
        }
        
        cout << "\n===== Depth First Search Traversal (Parallel) =====\n" << endl;
        
        // Using OpenMP tasks for parallelism
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
    // Recursive DFS with level tracking
    void dfsRecursive(Node* node, int level) {
        if (!node) return;
        
        // Print current node
        #pragma omp critical
        {
            cout << "Level " << level << ": Node " << node->data << endl;
        }
        
        // Process children in parallel using OpenMP tasks
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
};

int main() {
    ParallelBFS bfsTree;
    Node* root = nullptr;
    
    // Set number of threads for OpenMP
    int numThreads = 4; // You can adjust this based on your system
    omp_set_num_threads(numThreads);
    
    cout << "Parallel BFS and DFS Tree Traversal" << endl;
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
    
    // Perform BFS traversal
    bfsTree.bfs(root);
    
    // Perform DFS traversal
    ParallelDFS dfsTree;
    dfsTree.dfs(root);
    
    return 0;
}

// PS D:\DEGREE\8th Sem\practicals\HPC> g++ -fopenmp pr1.cpp -o bfs
// PS D:\DEGREE\8th Sem\practicals\HPC> ./bfs
// Parallel BFS and DFS Tree Traversal
// Using 4 OpenMP threads

// Enter value to insert: 7
// Do you want to insert another node? (y/n): y

// Enter value to insert: 3
// Do you want to insert another node? (y/n): y

// Enter value to insert: 2
// Do you want to insert another node? (y/n): y

// Enter value to insert: 6
// Do you want to insert another node? (y/n): y

// Enter value to insert: 8
// Do you want to insert another node? (y/n): y

// Enter value to insert: 5
// Do you want to insert another node? (y/n): 1

// ===== Breadth First Search Traversal (Parallel) =====

// Level 0:    7
// Level 1:    2    3
// Level 2:    8    6    5

// ====================================================

// ===== Depth First Search Traversal (Parallel) =====

// Level 0: Node 7
// Level 1: Node 2
// Level 1: Node 3
// Level 2: Node 5
// Level 2: Node 8
// Level 2: Node 6

// ===================================================
// PS D:\DEGREE\8th Sem\practicals\HPC>