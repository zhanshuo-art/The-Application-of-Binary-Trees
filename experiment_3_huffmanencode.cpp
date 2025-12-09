#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

// 哈夫曼树节点结构
struct HuffmanNode {
    string keyword;      // 关键词（只有叶子节点有）
    int freq;           // 频率（权值）
    HuffmanNode* left;  // 左子节点
    HuffmanNode* right; // 右子节点

    HuffmanNode(string kw, int f) : keyword(kw), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f) : keyword(""), freq(f), left(nullptr), right(nullptr) {}
};

// 优先队列的比较器（小顶堆）
struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq; // 频率小的优先级高
    }
};

// 读取CSV文件
bool readCSV(const string& filename, vector<string>& keywords, vector<int>& freqs, vector<double>& novelties) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误: 无法打开文件 " << filename << endl;
        return false;
    }

    string line;
    // 跳过表头
    getline(file, line);

    // 读取数据
    while (getline(file, line)) {
        stringstream ss(line);
        string keyword, freqStr, noveltyStr;

        // 读取三列数据
        getline(ss, keyword, ',');
        getline(ss, freqStr, ',');
        getline(ss, noveltyStr, ',');

        if (!keyword.empty() && !freqStr.empty()) {
            keywords.push_back(keyword);
            freqs.push_back(stoi(freqStr));
            if (!noveltyStr.empty()) {
                novelties.push_back(stod(noveltyStr));
            }
        }
    }

    file.close();
    return true;
}

// 构建哈夫曼树
HuffmanNode* buildHuffmanTree(const vector<string>& keywords, const vector<int>& freqs) {
    // 创建优先队列（小顶堆）
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;

    // 为每个关键词创建叶子节点并加入优先队列
    for (size_t i = 0; i < keywords.size(); i++) {
        HuffmanNode* node = new HuffmanNode(keywords[i], freqs[i]);
        pq.push(node);
    }

    // 构建哈夫曼树
    while (pq.size() > 1) {
        // 取出两个最小权值的节点
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        // 合并为新节点
        HuffmanNode* parent = new HuffmanNode(left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        // 将新节点插回优先队列
        pq.push(parent);
    }

    // 返回根节点
    return pq.empty() ? nullptr : pq.top();
}

// 递归生成哈夫曼编码
void generateCodesHelper(HuffmanNode* root, const string& code, map<string, string>& codes) {
    if (root == nullptr) return;

    // 如果是叶子节点，保存编码
    if (root->left == nullptr && root->right == nullptr) {
        codes[root->keyword] = code.empty() ? "0" : code; // 特殊情况：只有一个节点时编码为"0"
        return;
    }

    // 左分支记为0
    if (root->left) {
        generateCodesHelper(root->left, code + "0", codes);
    }

    // 右分支记为1
    if (root->right) {
        generateCodesHelper(root->right, code + "1", codes);
    }
}

// 生成哈夫曼编码
map<string, string> generateCodes(HuffmanNode* root) {
    map<string, string> codes;//map是用来储存编码的
    if (root == nullptr) return codes;

    // 特殊情况：只有一个节点
    if (root->left == nullptr && root->right == nullptr) {
        codes[root->keyword] = "0";
        return codes;
    }

    generateCodesHelper(root, "", codes);
    return codes;
}

// 计算编码效果并分析压缩率
void analyzeCompression(const vector<string>& keywords, const vector<int>& freqs,
                        const map<string, string>& huffmanCodes) {
    int N = keywords.size(); // 关键词数量

    // 1. 定长编码
    int fixedBitsPerKeyword = ceil(log2(N));
    if (fixedBitsPerKeyword == 0) fixedBitsPerKeyword = 1; // 至少1位

    long long totalFreq = 0; // 总频率（所有关键词出现次数之和）
    for (int f : freqs) {
        totalFreq += f;
    }

    long long fixedTotalBits = totalFreq * fixedBitsPerKeyword;

    // 2. 哈夫曼编码
    long long huffmanTotalBits = 0;
    for (size_t i = 0; i < keywords.size(); i++) {
        const string& keyword = keywords[i];
        int freq = freqs[i];
        int codeLength = huffmanCodes.at(keyword).length();
        huffmanTotalBits += (long long)freq * codeLength;
    }

    // 3. 计算压缩率
    double compressionRatio = (double)huffmanTotalBits / fixedTotalBits;

    // 输出结果
    cout << "\n========== 两种编码对比 ==========" << endl;
    cout << "关键词总数 (N): " << N << endl;
    cout << "关键词总出现次数: " << totalFreq << endl;
    cout << "\n【定长编码】" << endl;
    cout << "  总比特数: " << fixedTotalBits << " bits" << endl;
    cout << "  每个关键词位数: " << fixedBitsPerKeyword << " bits" << endl;
    cout << "\n【哈夫曼编码】" << endl;
    cout << "  总比特数: " << huffmanTotalBits << " bits" << endl;
    double avgCodeLength = (double)huffmanTotalBits / totalFreq;// 计算平均编码长度
    cout << "  平均编码长度: " << avgCodeLength << " bits" << endl;
    cout << "  压缩率 (Compression Ratio): " << compressionRatio << endl;
}

// 显示哈夫曼编码
void displayHuffmanCodes(const map<string, string>& codes, int maxDisplay = 20) {
    cout << "\n========== 哈夫曼编码 ==========" << endl;
    cout << "显示 " << min((int)codes.size(), maxDisplay) << " 个关键词的编码:" << endl;

    int count = 0;
    for (const auto& pair : codes) {
        if (count >= maxDisplay) break;
        cout << "  " << pair.first << " -> " << pair.second << endl;
        count++;
    }

    if (codes.size() > maxDisplay) {
        cout << "  ... (还有 " << (codes.size() - maxDisplay) << " 个关键词)" << endl;
    }
}

// 释放哈夫曼树内存
void deleteHuffmanTree(HuffmanNode* root) {
    if (root == nullptr) return;
    deleteHuffmanTree(root->left);
    deleteHuffmanTree(root->right);
    delete root;
}

int main() {
    // ========== 在此处填写CSV文件路径 ==========
    string csvFilePath = "E:\\InnovationDataset\\DeepInnovationAI\\DeepPatentAI_1979-1981_keyword_analysis_result.csv";  // 请修改为你的CSV文件路径
    // ==========================================

    // 读取CSV文件
    vector<string> keywords;
    vector<int> freqs;
    vector<double> novelties;

    if (!readCSV(csvFilePath, keywords, freqs, novelties)) {
        cerr << "程序终止。" << endl;
        return 1;
    }

    cout << "读取 " << keywords.size() << " 个关键词" << endl;

    if (keywords.empty()) {
        cerr << "错误: 没有读取到任何数据" << endl;
        return 1;
    }

    // 构建哈夫曼树
    HuffmanNode* root = buildHuffmanTree(keywords, freqs);

    if (root == nullptr) {
        cerr << "错误: 哈夫曼树构建失败" << endl;
        return 1;
    }
    // 生成哈夫曼编码
    map<string, string> huffmanCodes = generateCodes(root);

    // 显示哈夫曼编码
    displayHuffmanCodes(huffmanCodes, 100);//100，最多显示100个，但实际上只有13个

    // 分析压缩效果
    analyzeCompression(keywords, freqs, huffmanCodes);

    // 释放内存
    deleteHuffmanTree(root);

    return 0;
}
