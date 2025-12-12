#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

struct feature {
    bool chosen = false;
    double value;
};

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

struct validator {
    vector<dataPoint> validate(const vector<int>&, const string&, unsigned);
};

double runTests(vector<dataPoint>& data) {
    classifier cfr;
    double successes = 0;
    for (unsigned i = 0; i < data.size(); i++) {
        cfr.train(data, {0}, i);
        successes += cfr.test(11, data);
        sort(data.begin(), data.end(), [](const dataPoint& a, const dataPoint& b) {return a.baseIndex < b.baseIndex;}); //return to normal order
    }
    return successes / data.size() * 100;
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
        if (data[i].pClass == 1) tugOfWar += data[i].euclid;
        else tugOfWar -= data[i].euclid;
    }
    if (tugOfWar >= 0) {
        if (data[0].pClass == 1) return 1;
        else return 0;
    }
    if (data[0].pClass == 2) return 1;
    else return 0;
}

vector<dataPoint> validator::validate(const vector<int>& features, const string& filename, unsigned n) {

    bool negOne = false;
    for (auto f : features) if (f == -1) negOne = true;

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

            bool found = false;
            for (auto f : features) if (f == i) found = true;
            if (negOne) newDP.dpts.push_back(featPoint);
            else if (found) newDP.dpts.push_back(featPoint);
        }
        newDP.baseIndex = index;
        index++;
        data.push_back(newDP);
    } data.pop_back();
    return data;
}

void forwardSelection(const unsigned& rowFeatures) {
    cout << "Beginning search!";
    string set("");
    double accuracy = 0;

    vector<feature> features;
    features.resize(0);
    vector<dataPoint> data;

    while (true) {
        if (set.size()) set = "," + set;

        //rando values
        for (unsigned i = 0; i < rowFeatures; i++) {
            if (!features[i].chosen) features[i].value = (rand() % 1000) / 10.0;
            else features[i].value = 0;
        }

        int maxidx = 0;
        for (unsigned i = 0; i < features.size(); i++) {
            if (!features[i].chosen) cout << "\n\tUsing feature(s) {" << i + 1 << set << "} accuracy is " << features[i].value << '%';
            if (features[i].value > features[maxidx].value) maxidx = i;
        }

        //ensure most accurate combination is an improvement
        if (accuracy >= features[maxidx].value) {
            cout << "\n\nAccuracy cannot increase from these options.\nSearch completed. The best identified feature subset was {";
            for (unsigned i = 1; i < set.size(); i++) cout << set[i];
            cout << "} with an accuracy of " << accuracy << endl;
            return;
        }

        //update set
        set = to_string(maxidx + 1) + set;
        if (set.back() == ',') set.pop_back();

        accuracy = features[maxidx].value;
        features[maxidx].chosen = true;
        cout << "\nFeature set {" << set << "} has the highest accuracy (" << features[maxidx].value << "%)\n";
    }
}

void backwardElimination(const unsigned& size) {
    cout << "Beginning search!";

    string set;
    double accuracy = (rand() % 100) / 10.0;
    for (unsigned i = 0; i < size; i++) set += (to_string(i + 1) + ",");
    set.pop_back();

    vector<feature> features;
    features.resize(size);

    while (true) {
        cout << "\nCurrent set: {" << set << "}\n";
        //rando values, this time for the set without that feature
        for (unsigned i = 0; i < size; i++) {
            if (!features[i].chosen) features[i].value = (rand() % 1000) / 10.0;
            else features[i].value = 0;
        }

        int maxidx = 0;
        for (unsigned i = 0; i < features.size(); i++) {
            if (!features[i].chosen) cout << "\n\tRemoving feature {" << i + 1 << "} accuracy would be " << features[i].value << '%';
            if (features[i].value > features[maxidx].value) maxidx = i;
        }

        //ensure most accurate combination is an improvement
        if (accuracy >= features[maxidx].value) {
            cout << "\n\nAccuracy cannot increase from these options.\nSearch completed. The best identified feature subset was {";
            for (unsigned i = 0; i < set.size(); i++) cout << set[i];
            cout << "} with an accuracy of " << accuracy << endl;
            return;
        }

        //update set
        string tempSet;
        for (unsigned i = 0; i < size; i++) if (i != maxidx && !features[i].chosen) tempSet += (to_string(i + 1) + ",");
        set = tempSet;
        set.pop_back();

        accuracy = features[maxidx].value;
        features[maxidx].chosen = true;
        cout << "\nFeature set {" << set << "} has the highest accuracy (" << features[maxidx].value << "%)\n";
    }
}



int main() {
    srand(time(0));
    // srand(0);
    cout << "Welcome to Aidan Ciesla (acies002)'s Feature Selection Algorithm!\nEnter the algorithm to be used:\n1: Forward selection\n2: Backward elimination\n";
    int features, algNum;
    cin >> algNum;
    cout << "Enter the dataset to be used:\n1: Small\n2: Large\n";
    unsigned opinion, n, rowFeatures;
    cin >> opinion;

    string filename;
    if (opinion == 1) {
        filename = "small-4.txt";
        rowFeatures = 10;
        n = 100;
    }
    else {
        filename = "large-4.txt";
        rowFeatures = 40;
        n = 1000;
    }

    if (algNum == 1) forwardSelection(features);
    else if (algNum == 2) backwardElimination(features);
}