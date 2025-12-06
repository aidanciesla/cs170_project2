#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

struct dataPoint {
    vector<double> dpts;
    double pClass;
    double euclid;
    int baseIndex;
};

struct classifier {
    void train(vector<dataPoint>&, const vector<int>&, int);
    int test(const int&, vector<dataPoint>&);
};

void runTests(vector<dataPoint>& data) {
    classifier cfr;
    double successes = 0;
    for (unsigned i = 0; i < data.size(); i++) {
        cfr.train(data, {0}, i);
        successes += cfr.test(5, data);
        sort(data.begin(), data.end(), [](const dataPoint& a, const dataPoint& b) {return a.baseIndex < b.baseIndex;}); //return to normal order
    }
    cout << "Result: " << successes / data.size() * 100 << "% accuracy!\n";
}

void classifier::train(vector<dataPoint>& data, const vector<int>& features, int exclude) {
    for (unsigned i = 0; i < data.size(); i++) {
        //Get the Euclidean distance for all points from the excluded (testing) point
        if (i == exclude) continue;
        
        double runEuclid = 0;
        for (unsigned j = 0; j < data[i].dpts.size(); j++) runEuclid += pow(data[i].dpts[j] - data[exclude].dpts[j], 2);
        data[i].euclid = sqrt(runEuclid);
    }
    data[exclude].euclid = 0;
    sort(data.begin(), data.end(), [](const dataPoint &a, const dataPoint &b) {return a.euclid < b.euclid;});
}

int classifier::test(const int& k, vector<dataPoint>& data) {
    double tugOfWar = 0;
    for (unsigned i = 1; i <= k; i++) { //Closer data points have a stronger sway
        if (data[i].pClass == 1) tugOfWar += 1 / data[i].euclid;
        else tugOfWar -= 1 / data[i].euclid;
    }
    if (tugOfWar >= 0) {
        // cout << "1 predicted! Actual - " << data[0].pClass << endl;
        if (data[0].pClass == 1) return 1;
        else return 0;
    }
    // cout << "2 predicted! Actual - " << data[0].pClass << endl;
    if (data[0].pClass == 2) return 1;
    else return 0;
}

int main() {
    cout << "Which dataset would you like to use?\n1. Small\n2. Large\n";
    unsigned opinion, n;
    cin >> opinion;

    string filename;
    if (opinion == 1) {
        filename = "small-test-dataset-3.txt";
        n = 10;
    }
    else {
        filename = "large-test-dataset-3.txt";
        n = 40;
    }

    cout << "List the features you would like to use (insert 0 to finish; insert -1 to use all): ";
    vector<int> features;
    int feature = -1;
    while (feature != 0) {
        cin >> feature;
        if (feature > 0 && feature < 41 || feature == -1) features.push_back(feature);
        else if (feature != 0) cout << "Invalid input!\n";
        if (feature == -1) break;
    }
    sort(features.begin(), features.end());

    while (features.back() > 10 && opinion == 1) features.pop_back(); //only features 1-10 for small dataset

    //Populate dataset
    vector<dataPoint> data;
    int index = 0;
    ifstream inData(filename);
    if (!inData.is_open()) throw runtime_error("oops");
    while (inData.is_open() && !inData.eof()) {
        dataPoint newDP;
        inData >> newDP.pClass;
        for (unsigned i = 0; i < n; i++) {
            double featPoint;
            inData >> featPoint;
            newDP.dpts.push_back(featPoint);
        }
        newDP.baseIndex = index;
        index++;
        data.push_back(newDP);
    } data.pop_back();

    runTests(data);
}