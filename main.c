#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum rb_color { RED, BLACK };

struct RBNode {
    int value;
    enum rb_color color;
    struct RBNode *parent;  // NULL if root
    struct RBNode *children[2];  //[0] left child, [1] right child
};

struct RBTree {
    struct RBNode *root;
};


struct RBTree rb_tree_init() {
    struct RBTree tree = { NULL };
    return tree;
}

// TODO should be optimised
void rb_tree_add(struct RBTree *tree, int n) {
    struct RBNode *new_node = calloc(1, sizeof(struct RBNode));
    new_node->value = n;
    if (tree->root == NULL) {  // Tree is empty
        new_node->color = BLACK;
        tree->root = new_node;
    } else {  // Tree isn't empty
        struct RBNode *current = tree->root;
        while (true) {
            if (n == current->value) { // value already exists in tree. do nothing...
                return;
            }
            if (n < current->value) {
                if (current->children[0] == NULL) {
                    current->children[0] = new_node;
                    new_node->color = RED;
                    new_node->parent = current;
                    return;
                } else {
                    current = current->children[0];
                    continue;
                }
            } else if (n > current->value) {
                if (current->children[1] == NULL) {
                    current->children[1] = new_node;
                    new_node->color = RED;
                    new_node->parent = current;
                    return;
                } else {
                    current = current->children[1];
                    continue;
                }
            }
        }
    }
}


struct RBNode *rb_tree_find_node(struct RBTree *tree, int n) {
    struct RBNode **walk = &tree->root;
    while (*walk != NULL) {
        if ((*walk)->value == n) {
            return *walk;
        }
        if (n < (*walk)->value) {
            walk = &(*walk)->children[0];
        } else {
            walk = &(*walk)->children[1];
        }
    }
    return NULL;
}

bool rb_tree_contains(struct RBTree *tree, int n) {
    struct RBNode *node = rb_tree_find_node(tree, n);
    return (node == NULL) ? false : true;
}

// TODO this should be shorter and cleaner
void rb_tree_remove(struct RBTree *tree, int n) {
    struct RBNode *node = rb_tree_find_node(tree, n);
    if (node == NULL) return;  // do nothing if value isn't in tree
    int child_count = 0;
    if (node->children[0] != NULL) child_count++;
    if (node->children[1] != NULL) child_count++;
    if (child_count == 0) {
        if (node->value < node->parent->value) {
            node->parent->children[0] = NULL;
        } else {
            node->parent->children[1] = NULL;
        }
        free(node);
    } else if (child_count == 1) {
        struct RBNode *child = (node->children[0] == NULL) ? node->children[1] : node->children[0];
        child->parent = node->parent;
        if (node->value < node->parent->value) {
            node->parent->children[0] = child;
        } else {
            node->parent->children[1] = child;
        }
        free(node);
    } else if (child_count == 2) {
        struct RBNode *inorder_successor = node->children[1];
        while (inorder_successor->children[0] != NULL) inorder_successor = inorder_successor->children[0];
        int inorder_successor_value = inorder_successor->value;
        rb_tree_remove(tree, inorder_successor_value);
        node->value = inorder_successor_value;
    }
}

int main() {
    struct RBTree tree = rb_tree_init();
    rb_tree_add(&tree, 20);
    rb_tree_add(&tree, 10);
    rb_tree_add(&tree, 35);
    rb_tree_add(&tree, 15);
    rb_tree_add(&tree, 25);
    rb_tree_add(&tree, 30);
    rb_tree_add(&tree, 9);
    rb_tree_add(&tree, 14);
    printf("%s\n", (rb_tree_contains(&tree, 20)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 10)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 35)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 15)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 25)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 30)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 9)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 14)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 87)) ? "True" : "False");
    printf("--------------\n");
    rb_tree_remove(&tree, 10);
    printf("%s\n", (rb_tree_contains(&tree, 20)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 10)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 35)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 15)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 25)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 30)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 9)) ? "True" : "False");
    printf("%s\n", (rb_tree_contains(&tree, 14)) ? "True" : "False");
    return 0;
}
