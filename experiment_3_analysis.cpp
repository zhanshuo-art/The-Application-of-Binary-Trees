#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// 去除字符串首尾空格
string trim(const string& str) {
    size_t start = 0;
    size_t end = str.length();

    // 找到第一个非空格字符
    while (start < end && isspace(str[start])) {
        start++;
    }

    // 找到最后一个非空格字符
    while (end > start && isspace(str[end - 1])) {
        end--;
    }

    return str.substr(start, end - start);
}

// 检查关键词是否有效（不包含特殊字符）
bool isValidKeyword(const string& keyword) {
    if (keyword.empty()) {
        return false;
    }

    // 检查是否包含中括号、引号、逗号等特殊字符
    for (char c : keyword) {
        if (c == '[' || c == ']' || c == '"' || c == '\'' || c == '\\' || c == ',') {
            return false;
        }
    }

    // 检查是否只是空格
    bool hasNonSpace = false;
    for (char c : keyword) {
        if (!isspace(c)) {
            hasNonSpace = true;
            break;
        }
    }

    return hasNonSpace;
}

// 解析Keywords字段，提取所有关键词
vector<string> parseKeywords(const string& keywordField) {
    vector<string> keywords;
    string current = "";
    bool inQuote = false;

    for (size_t i = 0; i < keywordField.length(); i++) {
        char c = keywordField[i];

        if (c == '"') {
            inQuote = !inQuote;
            // 当引号关闭时，保存当前关键词（去除空格）
            if (!inQuote && !current.empty()) {
                string trimmed = trim(current);
                if (!trimmed.empty()) {
                    keywords.push_back(trimmed);
                }
                current = "";
            }
        } else if (inQuote) {
            current += c;
        }
    }

    return keywords;
}

// 解析CSV行，处理可能包含逗号的字段
vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string current = "";
    bool inQuote = false;
    bool inBracket = false;

    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];

        if (c == '"') {
            inQuote = !inQuote;
            current += c;
        } else if (c == '[') {
            inBracket = true;
            current += c;
        } else if (c == ']') {
            inBracket = false;
            current += c;
        } else if (c == ',' && !inQuote && !inBracket) {
            fields.push_back(current);
            current = "";
        } else {
            current += c;
        }
    }

    // 添加最后一个字段
    if (!current.empty() || line.back() == ',') {
        fields.push_back(current);
    }

    return fields;
}

int main() {
    // ========== 在这里填写输入文件路径 ==========
    string inputPath = "E:\\InnovationDataset\\DeepInnovationAI\\DeepPatentAI_1979-1981.csv";
    // ===========================================

    // 自动生成输出文件路径（与输入文件同目录）
    string outputPath = "E:\\InnovationDataset\\DeepInnovationAI\\DeepPatentAI_1979-1981_keyword_analysis_result.csv";

    ifstream inputFile(inputPath);
    if (!inputFile.is_open()) {
        cerr << "无法打开输入文件: " << inputPath << endl;
        return 1;
    }

    // 存储每个关键词的频次和Novelty总和
    map<string, int> keywordFreq;
    map<string, double> keywordNoveltySum;

    string line;
    bool isHeader = true;
    int lineCount = 0;

    while (getline(inputFile, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }

        lineCount++;
        vector<string> fields = parseCSVLine(line);

        // 检查是否有足够的列（至少需要9列）
        if (fields.size() < 9) {
            continue;
        }

        // 第八列是Keywords（索引7），第九列是Novelty（索引8）
        string keywordsField = fields[7];
        string noveltyStr = fields[8];

        // 解析Novelty值
        double novelty = 0.0;
        try {
            novelty = stod(noveltyStr);
        } catch (...) {
            // 如果无法解析Novelty，跳过这一行
            continue;
        }

        // 解析Keywords
        vector<string> keywords = parseKeywords(keywordsField);

        // 统计每个关键词（只统计有效的关键词）
        for (const string& keyword : keywords) {
            if (isValidKeyword(keyword)) {
                keywordFreq[keyword]++;
                keywordNoveltySum[keyword] += novelty;
            }
        }
    }

    inputFile.close();

    // 输出结果
    ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        cerr << "无法创建输出文件: " << outputPath << endl;
        return 1;
    }

    // 写入表头
    outputFile << "keyword,freq,avg_novelty" << endl;

    int outputCount = 0;
    for (const auto& entry : keywordFreq) {
        string keyword = entry.first;
        int freq = entry.second;

        // 只保留freq >= 4的关键词
        if (freq >= 4) {
            double avgNovelty = keywordNoveltySum[keyword] / freq;
            outputFile << keyword << "," << freq << "," << avgNovelty << endl;
            outputCount++;
        }
    }

    outputFile.close();

    cout << "处理了 " << lineCount << " 行数据。" << endl;
    cout << "共找到 " << keywordFreq.size() << " 个不同的关键词。" << endl;
    cout << "输出了 " << outputCount << " 个频次>=4的关键词。" << endl;
    cout << "输出文件: " << outputPath << endl;

    return 0;
}
