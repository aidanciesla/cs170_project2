#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

vector<double> convertIEEE(const vector<string>& inNums) {
    vector<double> double_list;
    // Use stod to convert the string (even in scientific notation) to a double
    for (const string& str : inNums) double_list.push_back(stod(str));
    return double_list;
}

int main() {
    cout << "Which dataset would you like to use?\n1. Small\n2. Large\n";
    unsigned opinion;
    cin >> opinion;

    string filename;
    if (opinion == 1) filename = "small-test-dataset.txt";
    else filename = "large-test-dataset.txt";

    cout << "List the features you would like to use (insert 0 to finish): ";
    vector<int> features;
    int feature = -1;
    while (feature != 0) {
        cin >> feature;
        if (feature > 0 && feature < 41) features.push_back(feature);
        else if (feature != 0) cout << "Invalid input!\n";
    }
    sort(features.begin(), features.end());

    while (features.back() > 10 && opinion == 1) features.pop_back(); //only features 1-10 for small dataset


    ifstream inFile(filename);
    ofstream outFile("large-test-dataset-2.txt");
    while (!inFile.eof() && inFile.is_open()) {
        vector<string> inLines;
        string inLine;
        inLines.resize(41);
        for (unsigned i = 0; i < 41; i++) {
            inFile >> inLine;
            inLines[i] = inLine;
        }
        vector<double> output = convertIEEE(inLines);
        for (auto d : output) outFile << d << "  ";
        outFile << endl;
    }
}