#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

typedef set<string> Itemset;
typedef vector<Itemset> Transactions;

Transactions loadTransactions(const string& filename) {
    Transactions transactions;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        Itemset trans;
        while (ss >> item) {
            trans.insert(item);
        }
        if (!trans.empty()) {
            transactions.push_back(trans);
        }
    }
    return transactions;
}

map<Itemset, int> getFrequent1Itemsets(const Transactions& transactions, double minSupCount) {
    map<string, int> itemCounts;
    for (const auto& trans : transactions) {
        for (const auto& item : trans) {
            itemCounts[item]++;
        }
    }

    map<Itemset, int> freq1;
    for (const auto& pair : itemCounts) {
        if (pair.second >= minSupCount) {
            freq1[{pair.first}] = pair.second;
        }
    }
    return freq1;
}

set<Itemset> candidateGen(const set<Itemset>& L_prev, int k) {
    set<Itemset> C_k;
    for (auto it1 = L_prev.begin(); it1 != L_prev.end(); ++it1) {
        for (auto it2 = next(it1); it2 != L_prev.end(); ++it2) {
            Itemset unionSet = *it1;
            unionSet.insert(it2->begin(), it2->end());
            if (unionSet.size() == k) {
                C_k.insert(unionSet);
            }
        }
    }
    return C_k;
}

void runApriori(const string& filename, double minSupport) {
    cout << "--- Running Apriori Algorithm ---" << endl;
    Transactions transactions = loadTransactions(filename);
    int totalTrans = transactions.size();
    if (totalTrans == 0) {
        cout << "No transactions found or file is empty.\n";
        return;
    }
    double minSupCount = minSupport * totalTrans;

    map<Itemset, int> L_k = getFrequent1Itemsets(transactions, minSupCount);
    int totalFrequentItemsets = L_k.size();
    
    int k = 2;
    while (!L_k.empty()) {
        set<Itemset> L_prev_keys;
        for (const auto& pair : L_k) {
            L_prev_keys.insert(pair.first);
        }

        set<Itemset> C_k = candidateGen(L_prev_keys, k);
        map<Itemset, int> candidateCounts;

        for (const auto& trans : transactions) {
            for (const auto& cand : C_k) {
                bool isSubset = true;
                for (const auto& item : cand) {
                    if (trans.find(item) == trans.end()) {
                        isSubset = false;
                        break;
                    }
                }
                if (isSubset) {
                    candidateCounts[cand]++;
                }
            }
        }

        map<Itemset, int> next_L_k;
        for (const auto& pair : candidateCounts) {
            if (pair.second >= minSupCount) {
                next_L_k[pair.first] = pair.second;
                totalFrequentItemsets++;
            }
        }

        L_k = next_L_k;
        k++;
    }
    cout << "Total Frequent Itemsets found: " << totalFrequentItemsets << endl;
}