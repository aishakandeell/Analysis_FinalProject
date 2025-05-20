#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <numeric>

using namespace std;

int compute_finish_time(const vector<int>& durations, int k) {
    return accumulate(durations.begin(), durations.begin() + min(k, (int)durations.size()), 0);
}

vector<int> schedule_inspections(const vector<vector<int>>& durations, int L, int C, int T) {
    vector<int> inspection_times;
    set<int> used_times;

    if (C == 1) {
        int max_finish = 0;
        for (int l = 0; l < L; ++l) {
            if (!durations[l].empty()) {
                max_finish = max(max_finish, durations[l][0]);
            }
        }
        if (max_finish <= T) inspection_times.push_back(max_finish);
        return inspection_times;
    }

    int max_students = 0;
    for (const auto& lab : durations) {
        max_students = max(max_students, (int)lab.size());
    }

    double standard_interval = static_cast<double>(T) / (C + 1);

    for (int i = 1; i <= C; ++i) {
        double target_time = i * standard_interval;
        vector<int> candidate_times;

        for (int l = 0; l < L; ++l) {
            if ((int)durations[l].size() >= i) {
                int finish_time = compute_finish_time(durations[l], i);
                candidate_times.push_back(finish_time);
            }
        }

        // Remove duplicates
        candidate_times.erase(remove_if(candidate_times.begin(), candidate_times.end(),
            [&](int t) { return used_times.count(t); }), candidate_times.end());

        // Select the closest candidate to target_time
        int chosen_time;
        if (!candidate_times.empty()) {
            chosen_time = candidate_times[0];
            int min_diff = abs(candidate_times[0] - target_time);
            for (int t : candidate_times) {
                int diff = abs(t - target_time);
                if (diff < min_diff || (diff == min_diff && t < chosen_time)) {
                    min_diff = diff;
                    chosen_time = t;
                }
            }
        }
        else {
            chosen_time = static_cast<int>(round(target_time));
        }

        inspection_times.push_back(chosen_time);
        used_times.insert(chosen_time);
    }
    return inspection_times;
}

int calculate_total_unoccupied_time(int T, int L, const vector<vector<int>>& durations, const vector<int>& inspection_times) {
    int total_unoccupied = 0;
    for (int i = 0; i < inspection_times.size(); ++i) {
        int inspect_time = inspection_times[i];
        cout << "inspection " << i<<", ";
        for (int l = 0; l < L; ++l) {
            if ((int)durations[l].size() > i) {
                cout << "lab " << l << " :";
                int finish_time = compute_finish_time(durations[l], i+1);
                cout << "finish time: " << finish_time<<endl;
                total_unoccupied += inspect_time - finish_time;
            }
        }
    }
    return total_unoccupied;
}

int main() {
    int L = 3, C = 2, D = 1;
    int T = D * 24;

    vector<vector<int>> durations = {
        {4, 5},
        {3, 2, 5},
        {6, 4}
    };

    vector<int> inspection_times = schedule_inspections(durations, L, C, T);
    int total_unoccupied_time = calculate_total_unoccupied_time(T, L, durations, inspection_times);

    cout << "\nSchedule system (inspection times): ";
    for (int t : inspection_times) {
        cout << t << " ";
    }
    cout << "\n";

    cout << "Total unoccupied time: " << total_unoccupied_time << "\n";

    return 0;
}
