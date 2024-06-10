#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struktur data untuk Flight
struct Flight {
    int id;
    char airline[50];
    char destination[50];
    int departure_hour;
    int departure_minute;
};

// Struktur Node untuk AVL Tree
struct AVLNode {
    struct Flight data;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
};

// Struktur Node untuk BST Tree
struct BSTNode {
    struct Flight data;
    struct BSTNode *left;
    struct BSTNode *right;
};

// Fungsi untuk mendapatkan tinggi dari node AVL
int height(struct AVLNode *node) {
    if (node == NULL) return 0;
    return node->height;
}

// Fungsi untuk menghitung nilai maksimum dari dua bilangan
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fungsi untuk menciptakan node baru untuk AVL Tree
struct AVLNode* createNode(struct Flight data) {
    struct AVLNode* newNode = (struct AVLNode*)malloc(sizeof(struct AVLNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

// Fungsi untuk mendapatkan balance factor dari node AVL
int getBalance(struct AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

// Fungsi rotasi kanan pada AVL Tree
struct AVLNode* rightRotate(struct AVLNode *y) {
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

// Fungsi rotasi kiri pada AVL Tree
struct AVLNode* leftRotate(struct AVLNode *x) {
    struct AVLNode *y = x->right;
    struct AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}


void printFlightInfo(struct Flight data) {
    printf("%-5d %-25s %-25s %02d:%02d\n", data.id, data.airline, 
	data.destination, data.departure_hour, data.departure_minute);
}

void printTableHeader() {
    printf("%-5s %-25s %-25s %-8s\n", "ID", "Maskapai Penerbangan",
	 "Destinasi", "Pukul");
}

void inOrderTraversalAVL(struct AVLNode* root, int* isFirstNode) {
    if (root != NULL) {
        inOrderTraversalAVL(root->left, isFirstNode);
        // Print header before printing the first node
        if (*isFirstNode) {
            printTableHeader();
            *isFirstNode = 0;
        }
        printFlightInfo(root->data);
        inOrderTraversalAVL(root->right, isFirstNode);
    }
}
// Fungsi untuk menyisipkan node baru ke dalam AVL Tree
struct AVLNode* insertAVL(struct AVLNode *root, struct Flight data) {
    if (root == NULL) return createNode(data);

    if (data.id < root->data.id)
        root->left = insertAVL(root->left, data);
    else if (data.id > root->data.id)
        root->right = insertAVL(root->right, data);
    else
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && data.id < root->left->data.id)
        return rightRotate(root);

    if (balance < -1 && data.id > root->right->data.id)
        return leftRotate(root);

    if (balance > 1 && data.id > root->left->data.id) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && data.id < root->right->data.id) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Fungsi untuk mencari node dengan kunci tertentu dalam AVL Tree
struct AVLNode* searchNodeAVL(struct AVLNode* root, int key) {
    if (root == NULL || key == root->data.id)
        return root;

    if (key < root->data.id)
        return searchNodeAVL(root->left, key);

    return searchNodeAVL(root->right, key);
}

// Fungsi untuk menemukan node dengan nilai terkecil dalam AVL Tree
struct AVLNode* minValueNodeAVL(struct AVLNode* node) {
    struct AVLNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Fungsi untuk menghapus node tertentu dalam AVL Tree
struct AVLNode* deleteNodeAVL(struct AVLNode* root, int key) {
    if (root == NULL) return root;

    if (key < root->data.id)
        root->left = deleteNodeAVL(root->left, key);
    else if (key > root->data.id)
        root->right = deleteNodeAVL(root->right, key);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            struct AVLNode* temp = root->left ? root->left : root->right;
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else
                *root = *temp;
            free(temp);
        } else {
            struct AVLNode* temp = minValueNodeAVL(root->right);
            root->data = temp->data;
            root->right = deleteNodeAVL(root->right, temp->data.id);
        }
    }

    if (root == NULL) return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}
struct AVLNode* loadFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }

    struct AVLNode* root = NULL;
    struct Flight data;

    while (fscanf(file, "%d %s %s %d:%d", &data.id, data.airline, data.destination,
                  &data.departure_hour, &data.departure_minute) == 5) {
        root = insertAVL(root, data);
    }

    if (fclose(file) != 0) {
        perror("Error closing file");
    }

    return root;
}

void saveNodeToFileAVL(struct AVLNode* node, FILE* file) {
    if (node != NULL) {
        saveNodeToFileAVL(node->left, file);
        fprintf(file, "%d %s %s %02d:%02d\n", node->data.id, node->data.airline, node->data.destination,
                node->data.departure_hour, node->data.departure_minute);
        saveNodeToFileAVL(node->right, file);
    }
}

void saveToFileAVL(struct AVLNode* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    saveNodeToFileAVL(root, file);

    if (fclose(file) != 0) {
        perror("Error closing file");
    }
}

// Fungsi untuk membuat node baru pada BST Tree
struct BSTNode* createNodeBST(struct Flight data) {
    struct BSTNode* newNode = (struct BSTNode*)malloc(sizeof(struct BSTNode));
    if (newNode == NULL) {
        printf("Gagal mengalokasikan memori.\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Fungsi rekursif untuk traversal in-order pada BST Tree
void inOrderTraversalBST(struct BSTNode* root, int* isFirstNode) {
    if (root != NULL) {
        inOrderTraversalBST(root->left, isFirstNode);

        // Print header before printing the first node
        if (*isFirstNode) {
            printTableHeader();
            *isFirstNode = 0;
        }

        printFlightInfo(root->data);
        inOrderTraversalBST(root->right, isFirstNode);
    }
}

// Fungsi untuk memasukkan data baru ke dalam BST Tree
struct BSTNode* insertBST(struct BSTNode* root, struct Flight data) {
    if (root == NULL) return createNodeBST(data);

    if (data.id < root->data.id)
        root->left = insertBST(root->left, data);
    else if (data.id > root->data.id)
        root->right = insertBST(root->right, data);
    else
        return root;

    return root;
}

// Fungsi untuk memuat data dari file ke dalam BST Tree
struct BSTNode* loadFromFileBST(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }

    struct BSTNode* root = NULL;
    struct Flight data;

    while (fscanf(file, "%d %s %s %d:%d", &data.id, data.airline, data.destination,
                  &data.departure_hour, &data.departure_minute) == 5) {
        root = insertBST(root, data);
    }

    if (fclose(file) != 0) {
        perror("Error closing file");
    }

    return root;
}

// Fungsi untuk menyimpan data dari BST Tree ke dalam file
void saveNodeToFileBST(struct BSTNode* node, FILE* file) {
    if (node != NULL) {
        saveNodeToFileBST(node->left, file);
        fprintf(file, "%d %s %s %02d:%02d\n", node->data.id, node->data.airline, node->data.destination,
                node->data.departure_hour, node->data.departure_minute);
        saveNodeToFileBST(node->right, file);
    }
}

// Fungsi untuk menyimpan BST Tree ke dalam file
void saveToFileBST(struct BSTNode* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    saveNodeToFileBST(root, file);

    if (fclose(file) != 0) {
        perror("Error closing file");
    }
}

// Fungsi untuk mencari node dengan kunci tertentu pada BST Tree
struct BSTNode* searchNodeBST(struct BSTNode* root, int key) {
    if (root == NULL || key == root->data.id)
        return root;

    if (key < root->data.id)
        return searchNodeBST(root->left, key);

    return searchNodeBST(root->right, key);
}

// Fungsi untuk mendapatkan node dengan nilai terkecil pada BST Tree
struct BSTNode* minValueNodeBST(struct BSTNode* node) {
    struct BSTNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Fungsi untuk menghapus node tertentu pada BST Tree berdasarkan kuncinya
struct BSTNode* deleteNodeBST(struct BSTNode* root, int key) {
    if (root == NULL) return root;

    if (key < root->data.id)
        root->left = deleteNodeBST(root->left, key);
    else if (key > root->data.id)
        root->right = deleteNodeBST(root->right, key);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            struct BSTNode* temp = root->left ? root->left : root->right;
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else
                *root = *temp;
            free(temp);
        } else {
            struct BSTNode* temp = minValueNodeBST(root->right);
            root->data = temp->data;
            root->right = deleteNodeBST(root->right, temp->data.id);
        }
    }

    return root;
}

// Fungsi rekursif untuk traversal in-order pada AVL Tree
void inOrderTraversal(struct AVLNode* root, int* isFirstNode) {
    if (root != NULL) {
        inOrderTraversal(root->left, isFirstNode);

        // Cetak header sebelum mencetak node pertama
        if (*isFirstNode) {
            printTableHeader();
            *isFirstNode = 0;
        }

        printFlightInfo(root->data);
        inOrderTraversal(root->right, isFirstNode);
    }
}

// Function to allow user to choose between AVL or BST Tree
int chooseTree() {
    int choice;
    printf("\n Pilih Tipe:\n");
    printf("1. AVL Tree\n");
    printf("2. BST Tree\n");
    printf("3. Keluar\n");
    printf("Masukan pilhan anda: ");
    scanf("%d", &choice);
    return choice;
}

int main() {
	int choice;
    int searchKey;
    int treeChoice;
	do {
    	treeChoice = chooseTree();
		struct AVLNode* avl_root = NULL; // AVL Tree root
	    struct BSTNode* bst_root = NULL; // BST Tree root
	
	    if (treeChoice == 1) {
	        // Load jadwal penerbangan 
	        avl_root = loadFromFile("file data.txt");
	    } else if (treeChoice == 2) {
	        bst_root = loadFromFile("file data.txt");
	    } else if (treeChoice == 3) {
	        printf("\nKeluar...");
	        return 1;
	    } else {
	        printf("Pilihan Invalid. \nKeluar...\n");
	        return 1;
	    }
	
	    do {
	        printf("\n1. Lihat Data\n");
	        printf("2. Tambah Data\n");
	        printf("3. Hapus Data\n");
	        printf("4. Cari Data\n");
	        printf("5. Kembali\n");
	        printf("Enter your choice: ");
	        scanf("%d", &choice);
	
	        switch (choice) {
	            case 1: {
	            	if (treeChoice == 1) {
		            printf("\n\t\tJadwal Keberangkatan Penerbangan (AVL)\n");
		            int isFirstNode = 1;
		            inOrderTraversal(avl_root, &isFirstNode);
		            } else {
		            printf("\n\t\tJadwal Keberangkatan Penerbangan (BST)\n");
		            int isFirstNode = 1;
		            inOrderTraversal(bst_root, &isFirstNode);
		            }
					break;
				}
				case 2: {
	                struct Flight newData;
	                printf("\nMasukan Nama Maskapai Penerbangan: ");
	                scanf("%s", newData.airline);
	                printf("Masukan Destinasi: ");
	                scanf("%s", newData.destination);
	                printf("Masukan Waktu Keberangkatan (hh:mm): ");
	                scanf("%d:%d", &newData.departure_hour, &newData.departure_minute);
	
	                // Assign unique ID (you can modify this logic based on your requirements)
	                newData.id = rand() % 100 + 1;
	
	                if (treeChoice == 1) {
	                    avl_root = insertAVL(avl_root, newData);
	                    saveToFileAVL(avl_root, "file data.txt");
	                } else {
	                    bst_root = insertBST(bst_root, newData);
	                    saveToFileBST(bst_root, "file data.txt");
	                }
	
	                printf("\nData Berhasil Ditambahkan.\n");
	                break;
	            }            
				case 3: {
	                int deleteKey;
	                printf("\nMasukan ID Yang Dihapus: ");
	                scanf("%d", &deleteKey);
	
	                if (treeChoice == 1) {
	                    avl_root = deleteNodeAVL(avl_root, deleteKey);
	                    saveToFileAVL(avl_root, "file data.txt");
	                } else {
	                    bst_root = deleteNodeBST(bst_root, deleteKey);
	                    saveToFileBST(bst_root, "file data.txt");
	                }
	
	                printf("\nData Berhasil Dihapus.\n");
	                break;
	            }
	            case 4: {
	                printf("\nMasukan ID yang akan Dicari: ");
	                scanf("%d", &searchKey);
	
	                if (treeChoice == 1) {
	                    struct AVLNode* avl_result = searchNodeAVL(avl_root, searchKey);
	                    if (avl_result != NULL) {
	                        printf("\nData Ditemukan AVL Tree:\n");
	                        printTableHeader();
	                        printFlightInfo(avl_result->data);
	                    } else {
	                        printf("\nData Tidak Ditemukan AVL Tree.\n");
	                    }
	                } else {
	                    struct BSTNode* bst_result = searchNodeBST(bst_root, searchKey);
	                    if (bst_result != NULL) {
	                        printf("\nData Ditemukan BST Tree:\n");
	                        printTableHeader();
	                        printFlightInfo(bst_result->data);
	                    } else {
	                        printf("\nData Tidak Ditemukan BST Tree.\n");
	                    }
	                }
	                break;
	            }
	            case 5:
	            	break;
	            default:
	                printf("\nInvalid. \nMasukan Data Yang Benar.\n");
	        }
	    } while (choice !=5);
	    // Free allocated memory for BST & AVL Tree
	    free(avl_root);
	    free(bst_root);
	    
} while (treeChoice != 3);	

    return 0;
}
