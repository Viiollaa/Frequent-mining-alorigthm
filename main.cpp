#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

void runApriori(const string& filename, double minSupport);
void runFPGrowth(const string& filename, double minSupport);

int main() {
    string filename = "retail.txt";
    double minSupport = 0.05; // 5% minimum support

    cout << "=== FREQUENT ITEMSET MINING BENCHMARK ===" << endl << endl;

    auto start1 = high_resolution_clock::now();
    runApriori(filename, minSupport);
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    cout << "Apriori Execution Time: " << duration1.count() << " microseconds\n\n";

    auto start2 = high_resolution_clock::now();
    runFPGrowth(filename, minSupport);
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout << "FP-Growth Execution Time: " << duration2.count() << " microseconds\n";

    return 0;
}