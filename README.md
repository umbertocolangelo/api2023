## Progetto API (Algoritmi e Principi dell'Informatica) 2023

This project implements a word comparison game similar to **Wordle**. The program efficiently manages and compares words using a red-black tree data structure. It takes user input for a reference word and a set of guessed words, processes them according to the game rules, and outputs the results. Developed as part of the course Algoritmi e Principi dell'Informatica at Politecnico di Milano.

Professor: Alessandro Barenghi

Grade: 30/30

## Key Components of the Project

### 1. Data Structures
- **Red-Black Tree**: The primary data structure used to store words. It maintains a balanced tree, allowing for efficient insertion, deletion, and searching of words.
  
### 2. Node Management
- **Node Creation**: The `new_node` function allocates memory for a new tree node and initializes it with a word.
- **Insertion**: The `insert` function adds a new word to the tree while maintaining the red-black properties through the `insert_fix` function, ensuring the tree remains balanced.

### 3. Comparison Logic
- The main comparison functionality is encapsulated in the `compare` function, which checks the guessed word against the reference word.
- It populates matrices (`mtx1` and `mtx2`) that track occurrences and positions of letters.
- The filtering functions (`filter_1`, `filter_2`, `filter_3`, `filter_4`, and `filter_4bis`) determine which words should be retained based on various conditions, such as the presence or absence of specific characters.

### 4. Traversal and Output
- **In-Order Traversal**: The `inorder` function prints the words in the tree in lexicographic order.
- The program continuously reads user input, processes word comparisons, and outputs results based on the rules defined by the Wordle-like mechanics.

### 5. Memory Management
- The program dynamically allocates memory for nodes and ensures proper deallocation in the `free_space_game` function, which recursively frees all nodes in the tree.

## Code Summary

Here’s a summarized explanation of some key functions in the code:

- **`struct tree* new_node(char* word, bool mem)`**: Creates a new node for the tree, allocating memory for the word.
- **`struct tree* insert(struct tree* root, struct tree* new)`**: Inserts a new node into the red-black tree.
- **`struct tree* compare(...)`**: Compares the guessed word against the reference word and updates the matrices for filtering.
- **`void filters(...)`**: Applies filtering logic to determine which words remain as valid candidates.
- **`struct tree* delete_nodes(struct tree *root)`**: Deletes nodes from the tree based on filtering criteria, adjusting the game size accordingly.
- **`void free_space_game(struct tree* root)`**: Frees all allocated memory for the tree nodes, preventing memory leaks.

## Performance Considerations

- The use of a red-black tree ensures that insertion, deletion, and search operations remain efficient with O(log n) time complexity.
- The filtering functions are designed to minimize the number of words retained in the game, enhancing performance by reducing the search space.
