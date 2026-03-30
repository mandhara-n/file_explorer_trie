#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 50
#define MAX_NAME 50
#define MAX_CONTENT 500

typedef struct Node {
    char name[MAX_NAME];
    int isFile;
    char content[MAX_CONTENT];
    struct Node* children[MAX_CHILDREN];
    int childCount;
} Node;

// Create node
Node* createNode(const char* name, int isFile) {
    Node* node = (Node*)malloc(sizeof(Node));
    strcpy(node->name, name);
    node->isFile = isFile;
    node->childCount = 0;
    node->content[0] = '\0';
    return node;
}

// Normalize path
void normalizePath(char* path) {
    if (path[0] != '/') {
        char temp[200];
        strcpy(temp, path);
        sprintf(path, "/%s", temp);
    }
}

// Find child
Node* findChild(Node* parent, const char* name) {
    for (int i = 0; i < parent->childCount; i++) {
        if (strcmp(parent->children[i]->name, name) == 0)
            return parent->children[i];
    }
    return NULL;
}

// Insert path
void insertPath(Node* root, char* path, int isFile) {
    normalizePath(path);

    char part[MAX_NAME];
    int idx = 0;
    Node* current = root;

    for (int i = 1;; i++) {
        if (path[i] == '/' || path[i] == '\0') {

            if (idx > 0) {
                part[idx] = '\0';

                if (current->isFile) {
                    printf("ERROR: Cannot create inside a file.\n");
                    return;
                }

                Node* child = findChild(current, part);

                if (!child) {
                    child = createNode(part, 0);
                    current->children[current->childCount++] = child;
                }

                current = child;
                idx = 0;
            }

            if (path[i] == '\0') break;
        }
        else {
            part[idx++] = path[i];
        }
    }

    current->isFile = isFile;
    printf("%s created successfully.\n", current->name);
}

// Display tree
void display(Node* root, int level) {
    for (int i = 0; i < level; i++) printf("  ");

    printf("%c %s%s\n",
           root->isFile ? '-' : '+',
           root->name,
           root->isFile ? "" : "/");

    for (int i = 0; i < root->childCount; i++) {
        display(root->children[i], level + 1);
    }
}

// Delete path
void deletePath(Node* root, char* path) {
    normalizePath(path);

    char part[MAX_NAME];
    int idx = 0;

    Node* current = root;
    Node* parent = NULL;
    int parentIndex = -1;

    for (int i = 1;; i++) {
        if (path[i] == '/' || path[i] == '\0') {

            if (idx > 0) {
                part[idx] = '\0';

                int found = 0;
                for (int j = 0; j < current->childCount; j++) {
                    if (strcmp(current->children[j]->name, part) == 0) {
                        parent = current;
                        parentIndex = j;
                        current = current->children[j];
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    printf("ERROR: Path not found.\n");
                    return;
                }

                idx = 0;
            }

            if (path[i] == '\0') break;
        }
        else part[idx++] = path[i];
    }

    if (!current->isFile && current->childCount > 0) {
        printf("ERROR: Folder not empty.\n");
        return;
    }

    for (int i = parentIndex; i < parent->childCount - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }

    parent->childCount--;
    free(current);

    printf("Deleted successfully.\n");
}

// Rename
void renamePath(Node* root, char* path, char* newName) {
    normalizePath(path);

    char part[MAX_NAME];
    int idx = 0;

    Node* current = root;
    Node* parent = NULL;
    int parentIndex = -1;

    if (strcmp(path, "/root") == 0) {
        printf("ERROR: Cannot rename root.\n");
        return;
    }

    for (int i = 1;; i++) {
        if (path[i] == '/' || path[i] == '\0') {

            if (idx > 0) {
                part[idx] = '\0';

                int found = 0;
                for (int j = 0; j < current->childCount; j++) {
                    if (strcmp(current->children[j]->name, part) == 0) {
                        parent = current;
                        parentIndex = j;
                        current = current->children[j];
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    printf("ERROR: Path not found.\n");
                    return;
                }

                idx = 0;
            }

            if (path[i] == '\0') break;
        }
        else part[idx++] = path[i];
    }

    strcpy(current->name, newName);
    printf("Renamed successfully.\n");
}

// Write file
void writeFile(Node* root, char* path) {
    normalizePath(path);

    char part[MAX_NAME];
    int idx = 0;
    Node* current = root;

    for (int i = 1;; i++) {
        if (path[i] == '/' || path[i] == '\0') {

            if (idx > 0) {
                part[idx] = '\0';

                Node* child = findChild(current, part);
                if (!child) {
                    printf("ERROR: Path not found.\n");
                    return;
                }

                current = child;
                idx = 0;
            }

            if (path[i] == '\0') break;
        }
        else part[idx++] = path[i];
    }

    if (!current->isFile) {
        printf("ERROR: Not a file.\n");
        return;
    }

    printf("Enter content: ");
    getchar();
    fgets(current->content, MAX_CONTENT, stdin);

    printf("Saved.\n");
}

// Read file
void readFile(Node* root, char* path) {
    normalizePath(path);

    char part[MAX_NAME];
    int idx = 0;
    Node* current = root;

    for (int i = 1;; i++) {
        if (path[i] == '/' || path[i] == '\0') {

            if (idx > 0) {
                part[idx] = '\0';

                Node* child = findChild(current, part);
                if (!child) {
                    printf("ERROR: Path not found.\n");
                    return;
                }

                current = child;
                idx = 0;
            }

            if (path[i] == '\0') break;
        }
        else part[idx++] = path[i];
    }

    if (!current->isFile) {
        printf("ERROR: Not a file.\n");
        return;
    }

    printf("\nContent:\n%s\n", current->content);
}

// Main
int main() {
    Node* root = createNode("root", 0);

    int choice;
    char path[200], newName[MAX_NAME];

    while (1) {
        printf("\n--- FILE EXPLORER ---\n");
        printf("1. Create Folder\n");
        printf("2. Create File\n");
        printf("3. Display\n");
        printf("4. Delete\n");
        printf("5. Rename\n");
        printf("6. Write File\n");
        printf("7. Read File\n");
        printf("8. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                scanf("%s", path);
                insertPath(root, path, 0);
                break;
            case 2:
                scanf("%s", path);
                insertPath(root, path, 1);
                break;
            case 3:
                display(root, 0);
                break;
            case 4:
                scanf("%s", path);
                deletePath(root, path);
                break;
            case 5:
                scanf("%s %s", path, newName);
                renamePath(root, path, newName);
                break;
            case 6:
                scanf("%s", path);
                writeFile(root, path);
                break;
            case 7:
                scanf("%s", path);
                readFile(root, path);
                break;
            case 8:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}