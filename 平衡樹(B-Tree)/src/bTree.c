#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define M 3 // 2-3 樹，M=3

// 節點結構
struct node
{
    int keys[M - 1];          // 最多 2 個鍵
    struct node *children[M]; // 最多 3 個子節點
    int num_keys;             // 當前鍵數量
    bool is_leaf;             // 是否為葉節點
};

// 創建新節點
struct node *create_node(bool is_leaf)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (!new_node)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_node->num_keys = 0;
    new_node->is_leaf = is_leaf;
    for (int i = 0; i < M - 1; i++)
    {
        new_node->keys[i] = 0;
    }
    for (int i = 0; i < M; i++)
    {
        new_node->children[i] = NULL;
    }
    return new_node;
}

// 輔助結構：分裂時返回中間鍵和新節點
struct split_result
{
    int key;            // 中間鍵
    struct node *left;  // 左節點
    struct node *right; // 右節點
};

// 分裂滿節點
struct split_result split_node(struct node *node, int new_key, struct node *new_child)
{
    struct split_result result;
    int temp_keys[M];                           // 臨時儲存 3 個鍵
    struct node *temp_children[M + 1] = {NULL}; // 臨時儲存 4 個子節點

    // 複製現有鍵和新鍵
    for (int i = 0; i < M - 1; i++)
    {
        temp_keys[i] = node->keys[i];
    }
    int i = M - 2;
    while (i >= 0 && temp_keys[i] > new_key)
    {
        temp_keys[i + 1] = temp_keys[i];
        i--;
    }
    temp_keys[i + 1] = new_key;

    // 複製子節點
    for (int j = 0; j < M; j++)
    {
        temp_children[j] = node->children[j];
    }
    if (!node->is_leaf)
    {
        int j = M - 1;
        while (j > i + 1)
        {
            temp_children[j] = temp_children[j - 1];
            j--;
        }
        temp_children[j] = new_child;
    }

    // 創建新節點
    struct node *left = create_node(node->is_leaf);
    struct node *right = create_node(node->is_leaf);

    // 中間鍵提升
    result.key = temp_keys[1];
    result.left = left;
    result.right = right;

    // 左節點：1 個鍵
    left->num_keys = 1;
    left->keys[0] = temp_keys[0];
    left->children[0] = temp_children[0];
    left->children[1] = temp_children[1];

    // 右節點：1 個鍵
    right->num_keys = 1;
    right->keys[0] = temp_keys[2];
    right->children[0] = temp_children[2];
    right->children[1] = temp_children[3];

    return result;
}

// 插入鍵（遞迴）
struct node *insert_recursive(struct node *root, int key, struct split_result *split)
{
    if (!root)
    {
        root = create_node(true);
        root->keys[0] = key;
        root->num_keys = 1;
        split->key = 0;
        split->left = NULL;
        split->right = NULL;
        return root;
    }

    // 葉節點插入
    if (root->is_leaf)
    {
        if (root->num_keys < M - 1)
        {
            // 直接插入
            int i = root->num_keys - 1;
            while (i >= 0 && root->keys[i] > key)
            {
                root->keys[i + 1] = root->keys[i];
                i--;
            }
            root->keys[i + 1] = key;
            root->num_keys++;
            split->key = 0;
            split->left = NULL;
            split->right = NULL;
        }
        else
        {
            // 分裂節點
            *split = split_node(root, key, NULL);
            free(root); // 釋放原節點
        }
        return NULL;
    }

    // 內部節點：找到插入子樹
    int i = root->num_keys - 1;
    while (i >= 0 && key < root->keys[i])
    {
        i--;
    }
    i++;

    struct split_result child_split = {0, NULL, NULL};
    insert_recursive(root->children[i], key, &child_split);

    // 處理子節點分裂
    if (child_split.left)
    {
        if (root->num_keys < M - 1)
        {
            // 插入中間鍵到當前節點
            int j = root->num_keys - 1;
            while (j >= i)
            {
                root->keys[j + 1] = root->keys[j];
                root->children[j + 2] = root->children[j + 1];
                j--;
            }
            root->keys[i] = child_split.key;
            root->children[i] = child_split.left;
            root->children[i + 1] = child_split.right;
            root->num_keys++;
            split->key = 0;
            split->left = NULL;
            split->right = NULL;
        }
        else
        {
            // 分裂當前節點
            struct node *new_child = child_split.right;
            int insert_key = child_split.key;
            if (i < root->num_keys)
            {
                new_child = root->children[i];
                root->children[i] = child_split.left;
            }
            *split = split_node(root, insert_key, new_child);
            free(root);
            return NULL;
        }
    }
    return root;
}

// 插入鍵（主函數）
struct node *insert(struct node *root, int key)
{
    struct split_result split = {0, NULL, NULL};
    root = insert_recursive(root, key, &split);

    // 根節點分裂
    if (split.left)
    {
        struct node *new_root = create_node(false);
        new_root->num_keys = 1;
        new_root->keys[0] = split.key;
        new_root->children[0] = split.left;
        new_root->children[1] = split.right;
        return new_root;
    }
    return root;
}

// 查找左子樹最大鍵
int find_max(struct node *node)
{
    while (!node->is_leaf)
    {
        node = node->children[node->num_keys];
    }
    return node->keys[node->num_keys - 1];
}

// 合併節點
void merge_nodes(struct node *parent, int index, struct node *child, struct node *sibling)
{
    // 假設 child 是欠流節點，sibling 是兄弟節點
    child->keys[child->num_keys] = parent->keys[index];
    child->num_keys++;
    child->keys[child->num_keys] = sibling->keys[0];
    child->num_keys++;

    // 移動子節點
    child->children[child->num_keys] = sibling->children[1];
    child->children[child->num_keys - 1] = sibling->children[0];

    // 更新父節點
    for (int i = index; i < parent->num_keys - 1; i++)
    {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    parent->children[parent->num_keys + 1] = NULL;

    free(sibling);
}

// 借用鍵
void borrow_from_sibling(struct node *parent, int index, struct node *child, struct node *sibling, bool from_right)
{
    if (from_right)
    {
        // 從右兄弟借
        child->keys[child->num_keys] = parent->keys[index];
        child->num_keys++;
        child->children[child->num_keys] = sibling->children[0];
        parent->keys[index] = sibling->keys[0];
        for (int i = 0; i < sibling->num_keys - 1; i++)
        {
            sibling->keys[i] = sibling->keys[i + 1];
            sibling->children[i] = sibling->children[i + 1];
        }
        sibling->children[sibling->num_keys - 1] = sibling->children[sibling->num_keys];
        sibling->num_keys--;
    }
    else
    {
        // 從左兄弟借
        for (int i = child->num_keys; i > 0; i--)
        {
            child->keys[i] = child->keys[i - 1];
            child->children[i + 1] = child->children[i];
        }
        child->children[1] = child->children[0];
        child->keys[0] = parent->keys[index];
        child->num_keys++;
        child->children[0] = sibling->children[sibling->num_keys];
        parent->keys[index] = sibling->keys[sibling->num_keys - 1];
        sibling->num_keys--;
    }
}

// 刪除鍵（遞迴）
struct node *delete_recursive(struct node *root, int key)
{
    if (!root)
        return root;

    // 查找鍵
    int i = 0;
    while (i < root->num_keys && key > root->keys[i])
    {
        i++;
    }

    if (i < root->num_keys && key == root->keys[i])
    {
        // 找到鍵
        if (root->is_leaf)
        {
            // 葉節點直接刪除
            for (int j = i; j < root->num_keys - 1; j++)
            {
                root->keys[j] = root->keys[j + 1];
            }
            root->num_keys--;
        }
        else
        {
            // 內部節點：用左子樹最大鍵替換
            int pred = find_max(root->children[i]);
            root->keys[i] = pred;
            root->children[i] = delete_recursive(root->children[i], pred);
        }
    }
    else
    {
        // 鍵不在當前節點，進入子樹
        if (root->is_leaf)
        {
            printf("Key %d not found\n", key);
            return root;
        }
        root->children[i] = delete_recursive(root->children[i], key);
    }

    // 修復欠流
    if (!root->is_leaf && root->children[i] && root->children[i]->num_keys < 1)
    {
        struct node *child = root->children[i];
        struct node *left_sibling = (i > 0) ? root->children[i - 1] : NULL;
        struct node *right_sibling = (i < root->num_keys) ? root->children[i + 1] : NULL;

        if (left_sibling && left_sibling->num_keys > 1)
        {
            borrow_from_sibling(root, i - 1, child, left_sibling, false);
        }
        else if (right_sibling && right_sibling->num_keys > 1)
        {
            borrow_from_sibling(root, i, child, right_sibling, true);
        }
        else
        {
            if (left_sibling)
            {
                merge_nodes(root, i - 1, left_sibling, child);
            }
            else
            {
                merge_nodes(root, i, child, right_sibling);
            }
        }
    }

    // 根節點特殊處理
    if (root->num_keys == 0 && !root->is_leaf)
    {
        struct node *new_root = root->children[0];
        free(root);
        return new_root;
    }

    return root;
}

// 顯示樹結構（層次遍歷）
void print_tree(struct node *root)
{
    if (!root)
    {
        printf("Empty tree\n");
        return;
    }

    struct node *queue[100];
    int front = 0, rear = 0;
    queue[rear++] = root;
    queue[rear++] = NULL; // 層分隔符

    int level = 0;
    printf("Tree structure:\n");
    while (front < rear)
    {
        struct node *current = queue[front++];
        if (current == NULL)
        {
            printf("\n");
            level++;
            if (front < rear)
            {
                queue[rear++] = NULL;
            }
            continue;
        }

        // 打印縮進
        for (int i = 0; i < level; i++)
        {
            printf("  ");
        }
        // 打印節點鍵
        printf("[");
        for (int i = 0; i < current->num_keys; i++)
        {
            printf("%d", current->keys[i]);
            if (i < current->num_keys - 1)
                printf(",");
        }
        printf("]");

        // 將子節點加入佇列
        if (!current->is_leaf)
        {
            for (int i = 0; i <= current->num_keys; i++)
            {
                if (current->children[i])
                {
                    queue[rear++] = current->children[i];
                }
            }
        }
    }
    printf("\n");
}

// 釋放樹記憶體
void free_tree(struct node *root)
{
    if (!root)
        return;
    for (int i = 0; i <= root->num_keys; i++)
    {
        free_tree(root->children[i]);
    }
    free(root);
}

// 主程式：測試插入和刪除
int main()
{
    struct node *root = NULL;
    int keys[] = {8, 15, 2, 4, 1, 3, 5, 6, 9, 16, 18, 20, 30, 40, 17}; // 測試鍵
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    // 測試插入
    printf("Inserting keys:\n");
    for (int i = 0; i < num_keys; i++)
    {
        printf("Insert %d:\n", keys[i]);
        root = insert(root, keys[i]);
        print_tree(root);
    }

    // 測試刪除
    int delete_keys[] = {17, 18, 40};
    int num_delete = sizeof(delete_keys) / sizeof(delete_keys[0]);
    printf("\nDeleting keys:\n");
    for (int i = 0; i < num_delete; i++)
    {
        printf("Delete %d:\n", delete_keys[i]);
        root = delete_recursive(root, delete_keys[i]);
        print_tree(root);
    }

    // 清理
    free_tree(root);
    return 0;
}