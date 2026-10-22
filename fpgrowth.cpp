#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

struct FPNode {
    string item;
    int count;
    FPNode* parent;
    map<string, shared_ptr<FPNode>> children;

    FPNode(string name, int cnt, FPNode* par) {
        item = name;
        count = cnt;
        parent = par;
    }
};

vector<vector<string>> loadFPGrowthTransactions(const string& filename) {
    vector<vector<string>> transactions;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        vector<string> trans;
        while (ss >> item) {
            trans.push_back(item);
        }
        if (!trans.empty()) {
            transactions.push_back(trans);
        }
    }
    return transactions;
}

void runFPGrowth(const string& filename, double minSupport) {
    cout << "--- Running FP-Growth Algorithm ---" << endl;
    vector<vector<string>> transactions = loadFPGrowthTransactions(filename);
    int totalTrans = transactions.size();
    if (totalTrans == 0) {
        cout << "No transactions found or file is empty.\n";
        return;
    }
    double minSupCount = minSupport * totalTrans;

    map<string, int> freqMap;
    for (const auto& trans : transactions) {
        for (const auto& item : trans) {
            freqMap[item]++;
        }
    }

    map<string, int> globalFreqItems;
    for (const auto& pair : freqMap) {
        if (pair.second >= minSupCount) {
            globalFreqItems[pair.first] = pair.second;
        }
    }

    shared_ptr<FPNode> root = make_shared<FPNode>("Null", 0, nullptr);

    for (const auto& trans : transactions) {
        vector<string> filteredTrans;
        for (const auto& item : trans) {
            if (globalFreqItems.find(item) != globalFreqItems.end()) {
                filteredTrans.push_back(item);
            }
        }
        sort(filteredTrans.begin(), filteredTrans.end(), [&](const string& a, const string& b) {
            if (globalFreqItems[a] != globalFreqItems[b])
                return globalFreqItems[a] > globalFreqItems[b];
            return a > b;
        });

        shared_ptr<FPNode> curr = root;
        for (const auto& item : filteredTrans) {
            if (curr->children.find(item) == curr->children.end()) {
                curr->children[item] = make_shared<FPNode>(item, 0, curr.get());
            }
            curr->children[item]->count++;
            curr = curr->children[item];
        }
    }

    cout << "FP-Tree constructed successfully. Total frequent items mined: " << globalFreqItems.size() << endl;
}