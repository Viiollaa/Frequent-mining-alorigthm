#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

struct FPNode : public enable_shared_from_this<FPNode> {
    string item;
    int count;
    shared_ptr<FPNode> parent;
    map<string, shared_ptr<FPNode>> children;

    FPNode(string name, int cnt, shared_ptr<FPNode> par) {
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

void mineTree(map<string, vector<pair<shared_ptr<FPNode>, int>>>& headerTable, 
              vector<string>& alpha, int minSupCount, int& totalItemsets) {
    
    vector<string> sortedItems;
    for (auto const& pair : headerTable) {
        sortedItems.push_back(pair.first);
    }

    for (const auto& item : sortedItems) {
        vector<string> beta = alpha;
        beta.push_back(item);
        
        int supportCount = 0;
        for (const auto& p : headerTable[item]) {
            supportCount += p.second;
        }

        if (supportCount >= minSupCount) {
            totalItemsets++;
            map<string, int> condFreqMap;
            vector<pair<vector<string>, int>> condPatterns;

            for (const auto& p : headerTable[item]) {
                shared_ptr<FPNode> curr = p.first;
                int pathCount = p.second;
                vector<string> path;
                
                while (curr->parent && curr->parent->item != "Null") {
                    curr = curr->parent;
                    path.push_back(curr->item);
                }
                
                if (!path.empty()) {
                    reverse(path.begin(), path.end());
                    condPatterns.push_back({path, pathCount});
                    for (const auto& patItem : path) {
                        condFreqMap[patItem] += pathCount;
                    }
                }
            }

            map<string, vector<pair<shared_ptr<FPNode>, int>>> condHeaderTable;
            shared_ptr<FPNode> condRoot = make_shared<FPNode>("Null", 0, nullptr);

            for (const auto& pat : condPatterns) {
                vector<string> filteredPath;
                for (const auto& patItem : pat.first) {
                    if (condFreqMap[patItem] >= minSupCount) {
                        filteredPath.push_back(patItem);
                    }
                }

                sort(filteredPath.begin(), filteredPath.end(), [&](const string& a, const string& b) {
                    if (condFreqMap[a] != condFreqMap[b])
                        return condFreqMap[a] > condFreqMap[b];
                    return a > b;
                });

                shared_ptr<FPNode> curr = condRoot;
                for (const auto& patItem : filteredPath) {
                    if (curr->children.find(patItem) == curr->children.end()) {
                        curr->children[patItem] = make_shared<FPNode>(patItem, 0, curr);
                        condHeaderTable[patItem].push_back({curr->children[patItem], 0});
                    }
                    curr->children[patItem]->count += pat.second;
                    curr = curr->children[patItem];
                }
            }

            for (auto& pair : condHeaderTable) {
                for (auto& nodePair : pair.second) {
                    nodePair.second = nodePair.first->count;
                }
            }

            if (!condHeaderTable.empty()) {
                mineTree(condHeaderTable, beta, minSupCount, totalItemsets);
            }
        }
    }
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
    map<string, vector<pair<shared_ptr<FPNode>, int>>> headerTable;

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
                curr->children[item] = make_shared<FPNode>(item, 0, curr);
                headerTable[item].push_back({curr->children[item], 0});
            }
            curr->children[item]->count++;
            curr = curr->children[item];
        }
    }

    for (auto& pair : headerTable) {
        for (auto& nodePair : pair.second) {
            nodePair.second = nodePair.first->count;
        }
    }

    int totalFrequentItemsets = 0;
    vector<string> alpha;
    
    mineTree(headerTable, alpha, minSupCount, totalFrequentItemsets);

    cout << "FP-Tree constructed successfully. Total Frequent Itemsets found: " << totalFrequentItemsets << endl;
}
