#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;

struct feature {
    bool chosen = false;
    double value;
};

void forwardSelection(const unsigned& size) {
    cout << "Beginning search!";
    string set("");
    double accuracy = 0;

    vector<feature> features;
    features.resize(size);
    while (true) {
        if (set.size()) set = "," + set;

        //rando values
        for (unsigned i = 0; i < size; i++) {
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

}

int main() {
    srand(time(0));
    // srand(0);
    cout << "Welcome to Aidan Ciesla (acies002)'s Feature Selection Algorithm!\nPlease enter total number of features: ";
    int features, algNum;
    cin >> features;
    cout << "Enter the algorithm to be used:\n1: Forward selection\n2: Backward elimination\n";
    cin >> algNum;

    if (algNum == 1) forwardSelection(features);
    else if (algNum == 2) backwardElimination(features);
}