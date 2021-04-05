#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

// TODO add LEFT_CHILD and RIGHT_CHILD defines?

#define RB_TREE_DEBUG_LOG 1

enum rb_color { RED, BLACK };
enum rb_dir { LEFT=0, RIGHT=1 };

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


void debug_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (RB_TREE_DEBUG_LOG) {
        vfprintf(stderr, fmt, args);
    }
    va_end(args);
}

void debug_traverse(struct RBNode *node, int left_count, int right_count, int height) {
    debug_log("(%d, %s)- ", node->value, (node->color == RED) ? "RED" : "BLACK");
    if (height == 0) debug_log("ROOT");
    for (int i = 1; i <= height; i++) {
        if (left_count & (int) pow(2, i)) {
            debug_log("LEFT ");
        } else if(right_count & (int) pow(2, i)) {
            debug_log("RIGHT ");
        }
    }
    debug_log("\n");
    if (node->children[0] != NULL) debug_traverse(node->children[0], left_count | (int) pow(2, height + 1), right_count, height + 1);
    if (node->children[1] != NULL) debug_traverse(node->children[1], left_count, right_count | (int) pow(2, height + 1), height + 1);
}


void rb_tree_swap_colors(struct RBNode *n1, struct RBNode *n2) {
    enum rb_color tmp_color;
    tmp_color = n1->color;
    n1->color = n2->color;
    n2->color = tmp_color;
}


// TODO REPLACE AS MACRO?
enum rb_dir rb_tree_dir(struct RBNode *node) {
    // ASSUMES NODE IS NOT ROOT
    return (node->value < node->parent->value) ? LEFT : RIGHT;
}


// TODO REPLACE AS MACRO?
struct RBNode *rb_tree_sibling(struct RBNode *node) {
    // ASSUMES NODE IS NOT ROOT
    return (rb_tree_dir(node) == LEFT) ? node->parent->children[1] : node->parent->children[0];
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
        if (rb_tree_dir(node) == LEFT) {
            child->parent->children[0] = child;
        } else {
            child->parent->children[1] = child;
        }
    }
    node->parent = child;
    node->children[1] = leftover;
    if (leftover != NULL) {
        leftover->parent = node;
    }
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
        if (rb_tree_dir(node) == LEFT) {
            child->parent->children[0] = child;
        } else {
            child->parent->children[1] = child;
        }
    }
    node->parent = child;
    node->children[0] = leftover;
    if (leftover != NULL) {
        leftover->parent = node;
    }
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
            if (rb_tree_dir(node) == RIGHT && rb_tree_dir(parent) == LEFT) {
                rb_tree_left_rotate(tree, parent);
                node = node->children[0];
            } else if (rb_tree_dir(node) == LEFT && rb_tree_dir(parent) == RIGHT) {
                rb_tree_right_rotate(tree, node->parent);
                node = node->children[1];
            } else if (rb_tree_dir(node) == LEFT && rb_tree_dir(parent) == LEFT) {
                rb_tree_right_rotate(tree, grandparent);
                rb_tree_swap_colors(node->parent, node->parent->children[1]);
            } else if (rb_tree_dir(node) == RIGHT && rb_tree_dir(parent) == RIGHT) {
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

struct RBNode * rb_tree_rotate(struct RBTree *tree, struct RBNode *p, enum rb_dir dir) {
    struct RBNode *g, *s, *c;
    printf("ROTATE START - %s\n", (dir == LEFT) ? "LEFT" : "RIGHT");
    printf("%d\n", p->value);
    g = p->parent;
    s = p->children[1-dir];
    assert(s != NULL);
    c = s->children[dir];
    p->children[1-dir] = c;
    if (c != NULL) c->parent = p;
    s->children[dir] = p;
    if (g != NULL) {
        g->children[(p == g->children[RIGHT]) ? RIGHT : LEFT] = s;
    } else {
        tree->root = s;
    }
    printf("ROTATE FINISH\n");
    return s;
}

void rb_tree_insert_node(struct RBTree *tree, struct RBNode *p, struct RBNode* n, enum rb_dir dir) {
    struct RBNode *g, *u;
    // add the node into the tree
    n->color = RED;
    n->children[LEFT] = NULL;
    n->children[RIGHT] = NULL;
    n->parent = p;
    if (p == NULL) {
        n->color = BLACK;
        tree->root = n;
        return;
    }
    p->children[dir] = n;
    do {
        // nothing more to resolve
        if (p->color == BLACK) return;
        // root is red - change to black
        if ((g = p->parent) == NULL) {
            p->color = BLACK;
            return;
        }
        dir = rb_tree_dir(p);
        u = g->children[1-dir];
        if (u == NULL || u->color == BLACK) {
            if (n == p->children[1-dir]) {
                rb_tree_rotate(tree, p, dir);
                n = p;
                p = g->children[dir];
            }
            rb_tree_rotate(tree, g, 1-dir);
            p->color = BLACK;
            g->color = RED;
            return;
        }
        p->color = BLACK;
        u->color = BLACK;
        g->color = RED;
        n = g;
    } while((p = n->parent) != NULL);
}

void rb_tree_insert(struct RBTree *tree, int val) {
    struct RBNode *n, *p;
    enum rb_dir dir;
    printf("INSERT - %d\n", val);
    n = calloc(1, sizeof(struct RBNode));
    n->value = val;
    p = tree->root;
    // tree is empty
    if (p == NULL) {
        rb_tree_insert_node(tree, p, n, 0);  // dir argument is irrelevant here
        return;
    }
    while (1) {
        if (val == p->value) return;
        if (val < p->value) {
            if (p->children[LEFT] == NULL) {
                rb_tree_insert_node(tree, p, n, LEFT);
                return;
            }
            p = p->children[LEFT];
        } else {
            if (p->children[RIGHT] == NULL) {
                rb_tree_insert_node(tree, p, n, RIGHT);
                return;
            }
            p = p->children[RIGHT];
        }
    }
    return;
    // find where to insert node in the tree
    dir = (val < p->value) ? LEFT : RIGHT;
    while (p->children[dir] != NULL) {
        if (val == p->value) return;  // do nothing if already in tree
        p = p->children[dir];
        dir = (val < p->value) ? LEFT : RIGHT;
    }
    rb_tree_insert_node(tree, p, n, dir);
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


// TODO MAKE MACRO?
bool rb_tree_is_black(struct RBNode *node) {
    return (node == NULL || node->color == BLACK);
}

bool rb_tree_is_red(struct RBNode *node) {
    return (node != NULL && node->color == RED);
}

bool rb_tree_has_red_child(struct RBNode *node) {
    for (int i = 0; i < 2; i++) {  // TODO maybe more efficient if not loop?
        if (rb_tree_is_red(node->children[i])) return true;
    }
    return false;
}

void rb_tree_fix_double_black(struct RBTree *tree, struct RBNode *node,  struct RBNode *parent) {
    // double black node is passed in as parent and orientation instead of the node itself
    // as double black node can be null, resulting in not being able to access it's parent
    // with a ->parent reference
    if (parent == NULL) {
        node->color = BLACK;
        return;
    }
    struct RBNode *sibling = (parent->children[LEFT] == node) ? parent->children[RIGHT] : parent->children[LEFT];
    if (sibling->color == BLACK && rb_tree_is_black(sibling->children[LEFT]) && rb_tree_is_black(sibling->children[RIGHT])) {  // sibling is black and so are both it's children
        rb_tree_swap_colors(sibling, parent);
        sibling->color = RED;
        if (sibling->color == BLACK) {  // parent was black and needs correcting
            rb_tree_fix_double_black(tree, parent, parent->parent);
        }
    } else if (sibling->color == BLACK && rb_tree_has_red_child(sibling)) {
        if (rb_tree_dir(sibling) == RIGHT && rb_tree_is_red(sibling->children[LEFT])) {
            sibling->children[LEFT]->color = BLACK;
            rb_tree_right_rotate(tree, sibling);
            rb_tree_left_rotate(tree, parent);
        } else if (rb_tree_dir(sibling) == RIGHT && rb_tree_is_red(sibling->children[RIGHT])) {
            parent->color = RED;
            rb_tree_left_rotate(tree, parent);
        } else if (rb_tree_dir(sibling) == LEFT && rb_tree_is_red(sibling->children[RIGHT])) {
            sibling->children[RIGHT]->color = BLACK;
            rb_tree_left_rotate(tree, sibling);
            rb_tree_right_rotate(tree, parent);
        } else if (rb_tree_dir(sibling) == LEFT && rb_tree_is_red(sibling->children[LEFT])) {
            parent->color = RED;
            rb_tree_right_rotate(tree, parent);
            
        }
   } else if (sibling->color == RED) {
        printf("Here Z\n");
        if (rb_tree_dir(sibling) == RIGHT) {
            rb_tree_left_rotate(tree, parent);
            rb_tree_swap_colors(parent, parent->parent);
            rb_tree_fix_double_black(tree, node, parent);
        } else if (rb_tree_dir(sibling) == LEFT) {
            rb_tree_left_rotate(tree, parent);
            rb_tree_swap_colors(parent, parent->parent);
            rb_tree_fix_double_black(tree, node, parent);
        }
        printf("Complete Z\n");
    }
}

// TODO this should be shorter and cleaner
// TODO make this not recursive
// TODO this is so messy
void rb_tree_remove(struct RBTree *tree, int n) {
    struct RBNode *node = rb_tree_find_node(tree, n);
    if (node == NULL) return;  // do nothing if value isn't in tree
    int child_count = 0;
    if (node->children[LEFT] != NULL) child_count++;
    if (node->children[RIGHT] != NULL) child_count++;
    if (child_count == 0) {
        if (node->parent == NULL) {
            tree->root = NULL;
        } else {
            node->parent->children[rb_tree_dir(node)] = NULL;
            if (node->color == BLACK) {
                rb_tree_fix_double_black(tree, NULL, node->parent);
            }
        }
        free(node);
    }
    if (child_count == 1) {
        struct RBNode *child = (node->children[LEFT] == NULL) ? node->children[RIGHT] : node->children[LEFT];
        node->value = child->value;
        node->children[rb_tree_dir(child)] = NULL; // TODO can probably be done more efficiently
        if (node->color == RED || child->color == RED) {
            node->color = BLACK;
        } else {
            if (node->parent != NULL) rb_tree_fix_double_black(tree, node, node->parent);
        }
        free(child);
    } else if (child_count == 2) {
        struct RBNode *inorder_predecessor = node->children[LEFT];
        while (inorder_predecessor->children[RIGHT] != NULL) inorder_predecessor = inorder_predecessor->children[RIGHT];
        int inorder_predecessor_value = inorder_predecessor->value;
        rb_tree_remove(tree, inorder_predecessor_value);
        node->value = inorder_predecessor_value;
    }
}

int main() {
    int nums[] = {762, 736, 705, 130, 541, 783, 246, 18, 520, 607, 714, 834, 233, 304, 447, 318, 981, 51, 288, 18, 630, 655, 733, 204, 414, 667, 81, 804, 274, 544, 260, 856, 41, 488, 215, 769, 688, 666, 528, 849, 207, 820, 451, 9, 88, 669, 127, 289, 726, 203, 162, 16, 943, 954, 42, 102, 207, 926, 824, 435, 295, 898, 998, 635, 940, 218, 134, 406, 706, 260, 615, 622, 178, 617, 61, 248, 578, 652, 262, 486, 148, 519, 299, 415, 565, 764, 746, 409, 252, 670, 470, 603, 951, 590, 807, 951, 826, 416, 998, 836};
    int shuffled_nums[] = {274, 470, 218, 630, 233, 622, 951, 726, 783, 520, 836, 652, 414, 16, 667, 769, 148, 807, 603, 746, 565, 998, 762, 134, 246, 666, 289, 299, 416, 669, 544, 304, 688, 204, 41, 488, 9, 764, 733, 127, 617, 820, 951, 655, 706, 615, 42, 898, 215, 940, 856, 736, 824, 447, 207, 834, 18, 954, 318, 61, 406, 849, 130, 295, 260, 826, 943, 541, 409, 451, 528, 926, 162, 102, 18, 203, 260, 590, 252, 262, 51, 714, 670, 998, 981, 248, 804, 207, 178, 607, 578, 435, 288, 88, 635, 705, 519, 486, 415, 81};
    struct RBTree tree = rb_tree_init();
    for (int i = 0; i < 100; i++) {
        rb_tree_insert(&tree, nums[i]);
        debug_traverse(tree.root, 0, 0, 0);
    }
    exit(1);
    for (int i = 0; i < 100; i++) {
        if (i == 33) {
            printf("----------------------------------\n");
        }
        rb_tree_remove(&tree, shuffled_nums[i]);
    }
    return 0;
}
