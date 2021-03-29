#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum rb_color { RED, BLACK };
enum rb_orientation { LEFT, RIGHT };

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

void debug_traverse(struct RBNode *node, int left_count, int right_count) {
    printf("%d - (%d, %d)\n", node->value, left_count, right_count);
    if (node->children[0] != NULL) debug_traverse(node->children[0], left_count + 1, right_count);
    if (node->children[1] != NULL) debug_traverse(node->children[1], left_count, right_count + 1);
}


void rb_tree_swap_colors(struct RBNode *n1, struct RBNode *n2) {
    enum rb_color tmp_color;
    tmp_color = n1->color;
    n1->color = n2->color;
    n2->color = tmp_color;
}


// TODO REPLACE AS MACRO?
enum rb_orientation rb_tree_orientation(struct RBNode *node) {
    // ASSUMES NODE IS NOT ROOT
    return (node->value < node->parent->value) ? LEFT : RIGHT;
}


// TODO REPLACE AS MACRO?
struct RBNode *rb_tree_sibling(struct RBNode *node) {
    // ASSUMES NODE IS NOT ROOT
    return (rb_tree_orientation(node) == LEFT) ? node->parent->children[1] : node->parent->children[0];
}

// TODO REPLACE AS MACRO?
struct RBNode *rb_tree_grandparent(struct RBNode *node) {
    // ASSUMES NODE IS NOT ROOT
    return node->parent->parent;
}

// TODO REPLACE AS MACRO?
struct RBNode *rb_tree_uncle(struct RBNode *node) {
    return rb_tree_sibling(node->parent);
}

void rb_tree_left_rotate(struct RBTree *tree, struct RBNode *node) {
    struct RBNode *child, *leftover;
    child = node->children[1];
    leftover = child->children[0];
    child->parent = node->parent;
    if (child->parent == NULL) {
        tree->root = child;
    } else {
        if (rb_tree_orientation(node) == LEFT) {
            child->parent->children[0] = child;
        } else {
            child->parent->children[1] = child;
        }
    }
    node->parent = child;
    node->children[1] = leftover;
    child->children[0] = node;
    if (child->parent == NULL) tree->root = child;
}

void rb_tree_right_rotate(struct RBTree *tree, struct RBNode *node) {
    struct RBNode *child, *leftover;
    child = node->children[0];
    leftover = child->children[1];
    child->parent = node->parent;
    if (child->parent == NULL) {
        tree->root = child;
    } else {
        if (rb_tree_orientation(node) == LEFT) {
            child->parent->children[0] = child;
        } else {
            child->parent->children[1] = child;
        }
    }
    node->parent = child;
    node->children[0] = leftover;
    child->children[1] = node;
    if (child->parent == NULL) tree->root = child;
}

void rb_tree_fixup(struct RBTree *tree, struct RBNode *node) {
    // do nothing if node is root
    if (node->parent == NULL) {
        return;
    }
    while (node->parent != NULL && node->parent->color == RED) {
        struct RBNode *uncle, *parent, *grandparent;
        uncle = rb_tree_uncle(node);
        parent = node->parent;
        grandparent = rb_tree_grandparent(node);
        // uncle is explicitly black or implicitly black due to being a NIL node
        if (uncle == NULL || uncle->color == BLACK) {
            if (rb_tree_orientation(node) == RIGHT && rb_tree_orientation(parent) == LEFT) {
                rb_tree_left_rotate(tree, parent);
                node = node->children[0];
            } else if (rb_tree_orientation(node) == LEFT && rb_tree_orientation(parent) == RIGHT) {
                rb_tree_right_rotate(tree, node->parent);
                node = node->children[1];
            } else if (rb_tree_orientation(node) == LEFT && rb_tree_orientation(parent) == LEFT) {
                rb_tree_right_rotate(tree, grandparent);
                rb_tree_swap_colors(node->parent, node->parent->children[1]);
            } else if (rb_tree_orientation(node) == RIGHT && rb_tree_orientation(parent) == RIGHT) {
                rb_tree_left_rotate(tree, grandparent);
                rb_tree_swap_colors(node->parent, node->parent->children[0]);
            }
        }
        // uncle is red
        else {
            // swap grandparent and grandparents childrens colors
            parent->color = BLACK;
            uncle->color = BLACK;
            grandparent->color = RED;
            grandparent->color = (grandparent->parent == NULL) ? BLACK : RED;
            // grandparent may need fixing up as color has changed so set node to node's grandparent
            node = rb_tree_grandparent(node);
        }
    }
}


void rb_tree_add(struct RBTree *tree, int n) {
    struct RBNode *prev_parent, *new_node = calloc(1, sizeof(struct RBNode));
    new_node->value = n;
    if (tree->root == NULL) {  // Tree is empty
        new_node->color = BLACK;
        tree->root = new_node;
    } else {  // Tree isn't empty
        struct RBNode **current = &tree->root;
        while (*current != NULL) {
            if ((*current)->value == n) {  // value already exists in tree. do nothing...
                return;
            }
            prev_parent = *current;
            current = (n < (*current)->value) ? &(*current)->children[0] : &(*current)->children[1];
        }
        *current = new_node;
        new_node->parent = prev_parent;
        new_node->color = RED;
        rb_tree_fixup(tree, new_node);
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
    rb_tree_add(&tree, 4);
    rb_tree_add(&tree, 3);
    rb_tree_add(&tree, 5);
    rb_tree_add(&tree, 2);
    rb_tree_add(&tree, 6);
    rb_tree_add(&tree, 1);
    rb_tree_add(&tree, 7);
    debug_traverse(tree.root, 0, 0);
    // rb_tree_remove(&tree, 10);
    // printf("%s\n", (rb_tree_contains(&tree, 20)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 10)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 35)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 15)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 25)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 30)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 9)) ? "True" : "False");
    // printf("%s\n", (rb_tree_contains(&tree, 14)) ? "True" : "False");
    return 0;
}
