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

void forwardSelection(const unsigned& rowFeatures, const string& filename) {
    cout << "Beginning search! Each iteration may take up to a minute with larger datasets.\n";
    string set("");
    double accuracy = 0;

    vector<feature> features(rowFeatures);
    validator val;
    
    while (true) {
        if (set.size()) set = "," + set;

        //Test each unselected feature by adding it to current set
        for (unsigned i = 0; i < rowFeatures; i++) {
            if (!features[i].chosen) {
                //Build feature list: current selected features + this candidate
                vector<int> testFeatures;
                for (unsigned j = 0; j < rowFeatures; j++) if (features[j].chosen || j == i) testFeatures.push_back(j);
                
                //Actually test this feature combination
                vector<dataPoint> data = val.validate(testFeatures, filename, rowFeatures);
                features[i].value = runTests(data);
                cout << "\tUsing feature(s) {" << i + 1 << set << "} accuracy is " << features[i].value << "%\n";
            }
            else features[i].value = 0;
        }

        //Find best feature to add
        int maxidx = 0;
        for (unsigned i = 0; i < features.size(); i++) if (features[i].value > features[maxidx].value) maxidx = i;

        //Check if we can still improve
        if (accuracy >= features[maxidx].value) {
            cout << "\nAccuracy cannot increase from these options.\nSearch completed. The best identified feature subset was {";
            for (unsigned i = 1; i < set.size(); i++) cout << set[i];
            cout << "} with an accuracy of " << accuracy << "%\n";
            return;
        }

        //Update set with best feature
        set = to_string(maxidx + 1) + set;
        if (set.back() == ',') set.pop_back();

        accuracy = features[maxidx].value;
        features[maxidx].chosen = true;
        cout << "Feature set {" << set << "} has the highest accuracy (" << features[maxidx].value << "%)\n\n";
    }
}

void backwardElimination(const unsigned& rowFeatures, const string& filename) {
    cout << "Beginning search! Each iteration may take up to a minute with larger datasets.\n";
    string set("");
    double accuracy = 0;
    vector<feature> features(rowFeatures);
    validator val;

    int failures = 0;
    double maxAcc;
    string maxSet;
    
    //Start with all features selected
    for (unsigned i = 0; i < rowFeatures; i++) {
        features[i].chosen = true;
        if (set.size()) set += ",";
        set += to_string(i + 1);
    }
    
    //Get initial accuracy with all features
    vector<int> allFeatures;
    for (unsigned i = 0; i < rowFeatures; i++) allFeatures.push_back(i);
    vector<dataPoint> data = val.validate(allFeatures, filename, rowFeatures);
    accuracy = runTests(data);
    cout << "Using all features {" << set << "} accuracy is " << accuracy << "%\n\n";
    
    while (true) {
        //Test removing each currently selected feature
        for (unsigned i = 0; i < rowFeatures; i++) {
            if (features[i].chosen) {
                //Build feature list: all selected features EXCEPT this one
                vector<int> testFeatures;
                for (unsigned j = 0; j < rowFeatures; j++) if (features[j].chosen && j != i) testFeatures.push_back(j);
                
                //Test this feature combination
                if (testFeatures.empty()) features[i].value = 0; //Can't have empty feature set
                else {
                    vector<dataPoint> data = val.validate(testFeatures, filename, rowFeatures);
                    features[i].value = runTests(data);
                }
                
                //Build display str for this combo
                string tempSet = "";
                for (auto f : testFeatures) {
                    if (tempSet.size()) tempSet += ",";
                    tempSet += to_string(f + 1);
                }
                cout << "\tRemoving feature " << i + 1 << " (using {" << tempSet << "}) accuracy is " << features[i].value << "%\n";
            }
            else {
                features[i].value = 0;
            }
        }
        
        //Find best feature to REMOVE (highest accuracy when removed)
        int maxidx = 0;
        for (unsigned i = 0; i < features.size(); i++) if (features[i].value > features[maxidx].value) maxidx = i;
        
        //Check if we can still improve
        if (accuracy > features[maxidx].value) {
            failures++;
            cout << "\nAccuracy cannot increase from these options.\n";
            if (failures > 1) {
                cout << "Search completed. The best identified feature subset was {" << maxSet << "} with an accuracy of " << maxAcc << "%\n";
                return;
            }
            cout << "Continuing...\n";
        }
        
        //Remove the feature that improved accuracy most
        features[maxidx].chosen = false;
        accuracy = features[maxidx].value;
        
        //Rebuild set string without removed feature
        set = "";
        for (unsigned i = 0; i < rowFeatures; i++) {
            if (features[i].chosen) {
                if (set.size()) set += ",";
                set += to_string(i + 1);
            }
        }
        
        cout << "Feature set {" << set << "} has the highest accuracy (" << accuracy << "%)\n\n";

        if (accuracy > maxAcc) { //Update maxima in case continuing search further decreases accuracy
            maxAcc = accuracy;
            maxSet = set;
        }
        
        //Stop if we've removed all but one feature
        if (set.find(',') == string::npos) {
            cout << "Only one feature remaining. Search complete!\n";
            if (maxAcc > accuracy) cout << "The best identified feature subset was {" << maxSet << "} with an accuracy of " << maxAcc << "%\n";
            return;
        }
    }
}

int main() {
    srand(time(0));
    // srand(0);
    cout << "Welcome to Aidan Ciesla (acies002)'s Feature Selection Algorithm!\nEnter the algorithm to be used:\n1: Forward selection\n2: Backward elimination\n";
    int features, algNum;
    cin >> algNum;
    cout << "Enter the dataset to be used:\n1: Small\n2: Large\n3. Titanic\n";
    unsigned dataset, n, rowFeatures;
    cin >> dataset;

    string filename;
    switch (dataset) {
        case 1:
            filename = "small-4.txt.";
            rowFeatures = 10; break;
        case 2:
            filename = "large-4.txt";
            rowFeatures = 40; break;
        case 3:
            filename = "titanic.txt";
            rowFeatures = 6; break;
    }

    if (algNum == 1) forwardSelection(rowFeatures, filename);
    else if (algNum == 2) backwardElimination(rowFeatures, filename);

    if (dataset == 3) cout << "Titanic features:\n1: Sex\n2: Ticket class\n3: Age\n4. Number of spouses and siblings aboard\n5. Number of parents and children aboard\n6. Ticket fare\n";
}