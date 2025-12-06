#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int main() {
    cout << "Which dataset would you like to use?\n1. Small\n2. Large";
    unsigned char opinion;
    cin >> opinion;

    cout << "List the features you would like to use (insert -1 to finish): ";
    vector<int> features;
    int feature = -2;
    while (feature != -1) {
        cin >> feature;
        if (feature > 0 && feature < 41) features.push_back(feature);
    }
    sort(features.begin(), features.end());

    while (features.back() > 10 && opinion == 1) features.pop_back(); //only features 1-10 for small dataset

    string filename;
    if (opinion == 1) filename = "small-test-dataset.txt";
    else filename = "large-test-dataset.txt";

    
}