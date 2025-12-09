#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

string CSV_FILE_PATH = "E:\\InnovationDataset\\DeepInnovationAI\\DeepPatentAI_1979-1981_keyword_analysis_result.csv";

// 树节点结构

struct TreeNode {
    string keyword;
    int freq;
    double avg_novelty;
    TreeNode *left, *right;

    TreeNode(string k, int f, double a) : keyword(k), freq(f), avg_novelty(a), left(nullptr), right(nullptr) {}
};

TreeNode* root = nullptr;
// 插入操作

TreeNode* insert(TreeNode* node, string keyword, int freq, double avg_novelty) {
    if (node == nullptr) {   //出口，空就插入
        return new TreeNode(keyword, freq, avg_novelty);
    }

    if (keyword < node->keyword) {
        node->left = insert(node->left, keyword, freq, avg_novelty);
    } else if (keyword > node->keyword) {
        node->right = insert(node->right, keyword, freq, avg_novelty);
    } else {
        // 相等则更新
        node->freq = freq;
        node->avg_novelty = avg_novelty;
    }

    return node;
}

// 查找操作

TreeNode* search(TreeNode* node, string keyword) {
    if (node == nullptr || node->keyword == keyword) {
        return node;
    }

    if (keyword < node->keyword) {
        return search(node->left, keyword);
    } else {
        return search(node->right, keyword);
    }
}

// 删除操作

TreeNode* findMin(TreeNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

TreeNode* deleteNode(TreeNode* node, string keyword) {
    if (node == nullptr) {
        return nullptr;
    }

    if (keyword < node->keyword) {
        node->left = deleteNode(node->left, keyword);
    } else if (keyword > node->keyword) {
        node->right = deleteNode(node->right, keyword);
    } else {
        // 找到要删除的节点

        // 情况1：叶子节点
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }
        // 情况2：只有右子树
        else if (node->left == nullptr) {
            TreeNode* temp = node->right;//用右子树代填删除的结点
            delete node;
            return temp;
        }
        // 情况3：只有左子树
        else if (node->right == nullptr) {
            TreeNode* temp = node->left;
            delete node;
            return temp;
        }
        // 情况4：有两个子树（用中序后继替代）
        else {
            TreeNode* temp = findMin(node->right);
            node->keyword = temp->keyword;
            node->freq = temp->freq;
            node->avg_novelty = temp->avg_novelty;
            node->right = deleteNode(node->right, temp->keyword);
        }
    }

    return node;
}

// 先序遍历（Pre-order）
void preorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        cout << "Keyword: " << node->keyword
             << ", Freq: " << node->freq
             << ", Avg_Novelty: " << node->avg_novelty << endl;
        preorderTraversal(node->left);
        preorderTraversal(node->right);
    }
}

// 中序遍历（In-order）
void inorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        inorderTraversal(node->left);
        cout << "Keyword: " << node->keyword
             << ", Freq: " << node->freq
             << ", Avg_Novelty: " << node->avg_novelty << endl;
        inorderTraversal(node->right);
    }
}

// 后序遍历（Post-order）
void postorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        postorderTraversal(node->left);
        postorderTraversal(node->right);
        cout << "Keyword: " << node->keyword
             << ", Freq: " << node->freq
             << ", Avg_Novelty: " << node->avg_novelty << endl;
    }
}

// 层次遍历（Level-order）
void levelorderTraversal(TreeNode* root) {
    if (root == nullptr) return;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        cout << "Keyword: " << current->keyword
             << ", Freq: " << current->freq
             << ", Avg_Novelty: " << current->avg_novelty << endl;

        if (current->left != nullptr) {
            q.push(current->left);
        }
        if (current->right != nullptr) {
            q.push(current->right);
        }
    }
}



// 区间查询：中序遍历输出所有满足 L <= keyword <= R 的节点

void rangeQueryHelper(TreeNode* node, string L, string R) {
    if (node == nullptr) return;

    // 中序遍历：左子树
    rangeQueryHelper(node->left, L, R);

    // 处理当前节点：如果在区间内，输出
    if (node->keyword >= L && node->keyword <= R) {
        cout << "Keyword: " << node->keyword
             << ", Freq: " << node->freq
             << ", Avg_Novelty: " << node->avg_novelty << endl;
    }

    // 中序遍历：右子树
    rangeQueryHelper(node->right, L, R);
}

// 区间查询主函数
bool rangeQuery(TreeNode* root, string L, string R) {
    // 先检查左边界是否存在
    TreeNode* leftNode = search(root, L);
    if (leftNode == nullptr) {
        cout << "错误：左边界 \"" << L << "\" 不存在于BST中！" << endl;
        return false;
    }

    // 再检查右边界是否存在
    TreeNode* rightNode = search(root, R);
    if (rightNode == nullptr) {
        cout << "错误：右边界 \"" << R << "\" 不存在于BST中！" << endl;
        return false;
    }

    // 检查左边界是否小于等于右边界
    if (L > R) {
        cout << "错误：左边界 \"" << L << "\" 大于右边界 \"" << R << "\"！" << endl;
        return false;
    }

    // 执行中序遍历区间查询
    rangeQueryHelper(root, L, R);
    return true;
}

// 按新颖性和频率筛选关键词
void filterByNoveltyAndFreq(TreeNode* node, double minNovelty, int minFreq) {
    if (node == nullptr) return;

    filterByNoveltyAndFreq(node->left, minNovelty, minFreq);

    if (node->avg_novelty >= minNovelty && node->freq >= minFreq) {
        cout << "Keyword: " << node->keyword
             << ", Freq: " << node->freq
             << ", Avg_Novelty: " << node->avg_novelty << endl;
    }

    filterByNoveltyAndFreq(node->right, minNovelty, minFreq);
}


int main() {
    ifstream file(CSV_FILE_PATH);
    if (!file.is_open()) {
        cout << "无法打开文件！" << endl;
        return 1;
    }

    string line;
    bool isHeader = true;

    while (getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }

        stringstream ss(line);
        string keyword, freqStr, noveltyStr;

        getline(ss, keyword, ',');
        getline(ss, freqStr, ',');
        getline(ss, noveltyStr, ',');

        if (!keyword.empty()) {
            root = insert(root, keyword, stoi(freqStr), stod(noveltyStr));
        }
    }
    file.close();
    cout << "BST构建完成！\n" << endl;



    cout << "========== 查找操作 ==========" << endl;
    string searchKey = "voice";  // 这里可以填入你要查找的关键词
    TreeNode* result = search(root, searchKey);
    if (result != nullptr) {
        cout << "找到: " << result->keyword
             << ", 频率: " << result->freq
             << ", 新颖度: " << result->avg_novelty << endl;
    } else {
        cout << "未找到: " << searchKey << endl;
    }
    cout << endl;


    cout << "========== 插入操作 ==========" << endl;
    root = insert(root, "new_keyword", 6, 11.764);
    cout << "插入 new_keyword 成功！" << endl;
    cout << endl;


    cout << "========== 删除操作 ==========" << endl;
    string deleteKey = "voice";  // 修改为你要删除的关键词
    TreeNode* deleteResult = search(root, deleteKey);
    if (deleteResult != nullptr) {
        root = deleteNode(root, deleteKey);
        cout << "删除 " << deleteKey << " 完成！" << endl;
    } else {
        cout << "未找到 " << deleteKey << "，无法删除！" << endl;
    }
    cout << endl;


    cout << "========== 先序遍历（Pre-order）==========" << endl;
    preorderTraversal(root);
    cout << endl;

    cout << "========== 中序遍历（In-order）- 字典序输出 ==========" << endl;
    inorderTraversal(root);
    cout << endl;

    cout << "========== 后序遍历（Post-order）==========" << endl;
    postorderTraversal(root);
    cout << endl;

    cout << "========== 层次遍历（Level-order）==========" << endl;
    levelorderTraversal(root);
    cout << endl;


    cout << "========== 区间查询 ==========" << endl;
    string L = "method";  // 在这里填入区间左边界
    string R = "recognizing";    // 在这里填入区间右边界
    cout << "查询区间 [" << L << ", " << R << "] 内的关键词（中序遍历）：" << endl;
    if (!rangeQuery(root, L, R)) {
        cout << "区间查询失败！" << endl;
    }
    cout << endl;


    cout << "========== 按新颖性和频率筛选 ==========" << endl;
    double minNovelty = 8.2;  // 在这里填入最低新颖度
    int minFreq = 4;         // 在这里填入最低频率
    cout << "筛选条件：avg_novelty >= " << minNovelty
         << " 且 freq >= " << minFreq << endl;
    filterByNoveltyAndFreq(root, minNovelty, minFreq);
    cout << endl;

    return 0;
}
