#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    int value;
    // int color;
    enum {red, black} color;
    struct node *left, *right, *parent;
} t_red_black_node_t;

typedef struct tree
{
    t_red_black_node_t *root;
    t_red_black_node_t *NIL; // 哨兵节点
} t_red_black_tree_t;

// 创建新节点
t_red_black_node_t *create_node(int value, t_red_black_node_t *NIL)
{
    t_red_black_node_t *node = (t_red_black_node_t *)malloc(sizeof(t_red_black_node_t));
    if (node == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(1);
    }
    node->value = value;
    node->color = red;
    node->left = NIL;
    node->right = NIL;
    node->parent = NIL;
    return node;
}

// 初始化红黑树
t_red_black_tree_t *init_red_black_tree()
{
    t_red_black_tree_t *tree = (t_red_black_tree_t *)malloc(sizeof(t_red_black_tree_t));
    if (tree == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(1);
    }
    
    // 创建NIL（哨兵）节点
    tree->NIL = (t_red_black_node_t *)malloc(sizeof(t_red_black_node_t));
    if (tree->NIL == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(1);
    }
    
    tree->NIL->color = black;
    tree->NIL->left = NULL;
    tree->NIL->right = NULL;
    tree->NIL->parent = NULL;
    tree->NIL->value = 0;
    
    tree->root = tree->NIL;
    
    return tree;
}

// 左旋
void left_rotate(t_red_black_tree_t *tree, t_red_black_node_t *x)
{
    t_red_black_node_t *y = x->right;
    x->right = y->left;
    
    if (y->left != tree->NIL)
        y->left->parent = x;
    
    y->parent = x->parent;
    
    if (x->parent == tree->NIL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    
    y->left = x;
    x->parent = y;
}

// 右旋
void right_rotate(t_red_black_tree_t *tree, t_red_black_node_t *y)
{
    t_red_black_node_t *x = y->left;
    y->left = x->right;
    
    if (x->right != tree->NIL)
        x->right->parent = y;
    
    x->parent = y->parent;
    
    if (y->parent == tree->NIL)
        tree->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    
    x->right = y;
    y->parent = x;
}

// 插入修复
void insert_fixup(t_red_black_tree_t *tree, t_red_black_node_t *z)
{
    while (z->parent->color == red) {
        if (z->parent == z->parent->parent->left) {
            t_red_black_node_t *y = z->parent->parent->right;
            
            if (y->color == red) {
                z->parent->color = black;
                y->color = black;
                z->parent->parent->color = red;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                
                z->parent->color = black;
                z->parent->parent->color = red;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            t_red_black_node_t *y = z->parent->parent->left;
            
            if (y->color == red) {
                z->parent->color = black;
                y->color = black;
                z->parent->parent->color = red;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                
                z->parent->color = black;
                z->parent->parent->color = red;
                left_rotate(tree, z->parent->parent);
            }
        }
        
        if (z == tree->root)
            break;
    }
    
    tree->root->color = black;
}

// 插入操作
void insert(t_red_black_tree_t *tree, int value)
{
    t_red_black_node_t *z = create_node(value, tree->NIL);
    t_red_black_node_t *y = tree->NIL;
    t_red_black_node_t *x = tree->root;
    
    while (x != tree->NIL) {
        y = x;
        if (z->value < x->value)
            x = x->left;
        else
            x = x->right;
    }
    
    z->parent = y;
    
    if (y == tree->NIL)
        tree->root = z;
    else if (z->value < y->value)
        y->left = z;
    else
        y->right = z;
    
    insert_fixup(tree, z);
}

// 查找节点
t_red_black_node_t *search(t_red_black_tree_t *tree, int value)
{
    t_red_black_node_t *current = tree->root;
    
    while (current != tree->NIL) {
        if (value == current->value)
            return current;
        
        if (value < current->value)
            current = current->left;
        else
            current = current->right;
    }
    
    return NULL;
}

// 找到最小值节点
t_red_black_node_t *minimum(t_red_black_tree_t *tree, t_red_black_node_t *node)
{
    while (node->left != tree->NIL)
        node = node->left;
    
    return node;
}

// 替换节点
void transplant(t_red_black_tree_t *tree, t_red_black_node_t *u, t_red_black_node_t *v)
{
    if (u->parent == tree->NIL)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    
    v->parent = u->parent;
}

// 删除修复
void delete_fixup(t_red_black_tree_t *tree, t_red_black_node_t *x)
{
    while (x != tree->root && x->color == black) {
        if (x == x->parent->left) {
            t_red_black_node_t *w = x->parent->right;
            
            if (w->color == red) {
                w->color = black;
                x->parent->color = red;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            
            if (w->left->color == black && w->right->color == black) {
                w->color = red;
                x = x->parent;
            } else {
                if (w->right->color == black) {
                    w->left->color = black;
                    w->color = red;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                
                w->color = x->parent->color;
                x->parent->color = black;
                w->right->color = black;
                left_rotate(tree, x->parent);
                x = tree->root;
            }
        } else {
            t_red_black_node_t *w = x->parent->left;
            
            if (w->color == red) {
                w->color = black;
                x->parent->color = red;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            
            if (w->right->color == black && w->left->color == black) {
                w->color = red;
                x = x->parent;
            } else {
                if (w->left->color == black) {
                    w->right->color = black;
                    w->color = red;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                
                w->color = x->parent->color;
                x->parent->color = black;
                w->left->color = black;
                right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    
    x->color = black;
}

// 删除节点
void delete_node(t_red_black_tree_t *tree, t_red_black_node_t *z)
{
    t_red_black_node_t *y = z;
    t_red_black_node_t *x;
    int y_original_color = y->color;
    
    if (z->left == tree->NIL) {
        x = z->right;
        transplant(tree, z, z->right);
    } else if (z->right == tree->NIL) {
        x = z->left;
        transplant(tree, z, z->left);
    } else {
        y = minimum(tree, z->right);
        y_original_color = y->color;
        x = y->right;
        
        if (y->parent == z)
            x->parent = y;
        else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    if (y_original_color == black)
        delete_fixup(tree, x);
    
    free(z);
}

// 删除值
void delete_value(t_red_black_tree_t *tree, int value)
{
    t_red_black_node_t *node = search(tree, value);
    
    if (node != NULL)
        delete_node(tree, node);
    else
        printf("值 %d 不存在于树中\n", value);
}

// 销毁树（递归）
void destroy_tree_recursive(t_red_black_tree_t *tree, t_red_black_node_t *node)
{
    if (node == tree->NIL)
        return;
    
    destroy_tree_recursive(tree, node->left);
    destroy_tree_recursive(tree, node->right);
    free(node);
}

// 销毁整个树
void destroy_tree(t_red_black_tree_t *tree)
{
    destroy_tree_recursive(tree, tree->root);
    free(tree->NIL);
    free(tree);
}

// 打印节点及其连接（递归辅助函数）
void print_tree_recursive(t_red_black_tree_t *tree, t_red_black_node_t *node, int level, char **matrix, int col, int width)
{
    if (node == tree->NIL)
        return;
    
    // 节点位置
    int row = level * 2;
    char node_color = (node->color == red) ? 'r' : 'b';
    
    // 添加节点值和颜色
    char node_str[10];
    sprintf(node_str, "%d%c", node->value, node_color);
    int len = strlen(node_str);
    
    for (int i = 0; i < len; i++) {
        if (col + i < width)
            matrix[row][col + i] = node_str[i];
    }
    
    // 处理左子树
    if (node->left != tree->NIL) {
        int left_col = col - (1 << (3 - level));
        
        // 添加左连接
        matrix[row + 1][col - 1] = '/';
        
        print_tree_recursive(tree, node->left, level + 1, matrix, left_col, width);
    }
    
    // 处理右子树
    if (node->right != tree->NIL) {
        int right_col = col + (1 << (3 - level));
        
        // 添加右连接
        matrix[row + 1][col + 1] = '\\';
        
        print_tree_recursive(tree, node->right, level + 1, matrix, right_col, width);
    }
}

// 可视化打印红黑树
void print_tree(t_red_black_tree_t *tree)
{
    if (tree->root == tree->NIL) {
        printf("空树\n");
        return;
    }
    
    int height = 10; // 假设树的高度不超过5（2*5行）
    int width = 80;
    
    // 创建并初始化矩阵
    char **matrix = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        matrix[i] = (char *)malloc(width * sizeof(char));
        for (int j = 0; j < width; j++) {
            matrix[i][j] = ' ';
        }
        matrix[i][width - 1] = '\0';
    }
    
    // 递归填充矩阵
    print_tree_recursive(tree, tree->root, 0, matrix, width / 2, width);
    
    // 打印矩阵
    for (int i = 0; i < height; i++) {
        printf("%s\n", matrix[i]);
    }
    
    // 释放矩阵内存
    for (int i = 0; i < height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// 使用DOT语言生成可视化文件
void generate_dot_file(t_red_black_tree_t *tree, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "无法创建文件\n");
        return;
    }
    
    fprintf(fp, "digraph RedBlackTree {\n");
    fprintf(fp, "    node [style=filled, fontcolor=white];\n");
    
    // 创建队列用于层次遍历
    t_red_black_node_t **queue = (t_red_black_node_t **)malloc(1000 * sizeof(t_red_black_node_t *));
    int front = 0, rear = 0;
    
    if (tree->root != tree->NIL) {
        queue[rear++] = tree->root;
        
        while (front < rear) {
            t_red_black_node_t *node = queue[front++];
            
            // 节点颜色和标签
            fprintf(fp, "    n%d [label=\"%d\", fillcolor=%s];\n", 
                   node->value, node->value, 
                   (node->color == red) ? "red" : "black");
            
            // 处理左子节点
            if (node->left != tree->NIL) {
                fprintf(fp, "    n%d -> n%d [label=\"L\"];\n", 
                       node->value, node->left->value);
                queue[rear++] = node->left;
            } else {
                fprintf(fp, "    nil%dL [label=\"NIL\", fillcolor=black, shape=box];\n", node->value);
                fprintf(fp, "    n%d -> nil%dL [label=\"L\"];\n", node->value, node->value);
            }
            
            // 处理右子节点
            if (node->right != tree->NIL) {
                fprintf(fp, "    n%d -> n%d [label=\"R\"];\n", 
                       node->value, node->right->value);
                queue[rear++] = node->right;
            } else {
                fprintf(fp, "    nil%dR [label=\"NIL\", fillcolor=black, shape=box];\n", node->value);
                fprintf(fp, "    n%d -> nil%dR [label=\"R\"];\n", node->value, node->value);
            }
        }
    }
    
    fprintf(fp, "}\n");
    fclose(fp);
    free(queue);
    
    printf("Graphviz DOT 文件已生成: %s\n", filename);
    printf("可使用命令生成图片: dot -Tpng %s -o tree.png\n", filename);
}

// 测试主函数
int main()
{
    t_red_black_tree_t *tree = init_red_black_tree();
    
    // 插入一些值
    insert(tree, 10);
    insert(tree, 20);
    insert(tree, 5);
    insert(tree, 30);
    insert(tree, 15);
    insert(tree, 25);
    insert(tree, 2);
    
    printf("红黑树简单可视化:\n");
    print_tree(tree);
    
    // 生成DOT文件以便更好地可视化
    generate_dot_file(tree, "rbtree.dot");
    
    // 删除一个节点
    printf("\n删除值 15 后:\n");
    delete_value(tree, 15);
    print_tree(tree);
    
    // 清理
    destroy_tree(tree);
    
    return 0;
}