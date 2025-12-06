#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

struct dataPoint {
    vector<double> data;
    double pClass;
};

struct classifier {
    void train();
    void test(const int&);
};

void classifier::train() {

}

void classifier::test(const int& k) {

}

int main() {
    cout << "Which dataset would you like to use?\n1. Small\n2. Large\n";
    unsigned opinion, n;
    cin >> opinion;

    string filename;
    if (opinion == 1) {
        filename = "small-test-dataset.txt";
        n = 10;
    }
    else {
        filename = "large-test-dataset.txt";
        n = 40;
    }

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

    //Populate dataset
    vector<dataPoint> data;
    ifstream inData(filename);
    if (!inData.is_open()) throw runtime_error("oops");
    while (inData.is_open() && !inData.eof()) {
        dataPoint newDP;
        inData >> newDP.pClass;
        for (unsigned i = 0; i < n; i++) {
            double featPoint;
            inData >> featPoint;
            newDP.data.push_back(featPoint);
        }
        data.push_back(newDP);
    } data.pop_back();

    
}