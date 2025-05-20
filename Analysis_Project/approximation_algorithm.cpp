#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

int compute_finish_time(const vector<int>& durations, int k) {
    int sum = 0;
    for (int i = 0; i < k && i < durations.size(); ++i) {
        sum += durations[i];
    }
    return sum;
}

int get_max_first_finish_time(const vector<vector<int>>& durations, int T) {
    vector<int> finish_times;
    for (const auto& lab : durations) {
        if (!lab.empty()) {
            finish_times.push_back(lab[0]);
        }
    }
    sort(finish_times.begin(), finish_times.end(), greater<int>());
    for (int t : finish_times) {
        if (t <= T) return t;
    }
    return -1; // fallback in case all are greater than T
}

vector<int> schedule_inspections(const vector<vector<int>>& durations, int L, int C, int T) {
    vector<int> inspection_times;

    if (C == 1) {
        int t = get_max_first_finish_time(durations, T);
        if (t != -1) inspection_times.push_back(t);
        return inspection_times;
    }

    double standard_interval = static_cast<double>(T) / C;

    for (int i = 1; i <= C; ++i) {
        double target_time = i * standard_interval;
        vector<int> candidate_times;

        for (int l = 0; l < L; ++l) {
            if (durations[l].size() >= i) {
                int finish_time = compute_finish_time(durations[l], i);
                if (finish_time <= T) {
                    candidate_times.push_back(finish_time);
                }
            }
        }

        if (candidate_times.empty()) {
            inspection_times.push_back(static_cast<int>(target_time));
            continue;
        }

        int closest_time = candidate_times[0];
        int min_diff = abs(candidate_times[0] - target_time);

        for (int t : candidate_times) {
            int diff = abs(t - target_time);
            if (diff < min_diff) {
                min_diff = diff;
                closest_time = t;
            }
        }
        inspection_times.push_back(closest_time);
    }
    return inspection_times;
}

int calculate_total_occupied_time(const vector<vector<int>>& durations, const vector<int>& inspection_times, int L) {
    int total_occupied_time = 0;
    for (int l = 0; l < L; ++l) {
        int lab_time = 0;
        int inspection_idx = 0;
        for (int j = 0; j < durations[l].size() && inspection_idx < inspection_times.size(); ++j) {
            int start_time = max(lab_time, inspection_times[inspection_idx]);
            lab_time = start_time + durations[l][j];
            total_occupied_time += durations[l][j];
            ++inspection_idx;
        }
    }
    return total_occupied_time;
}

int main() {
    int L, C, D;
    cin >> L;
    vector<int> students_per_lab(L);
    for (int i = 0; i < L; ++i) {
        cin >> students_per_lab[i];
    }

    cin >> D;
    int T = D * 24;
    cin >> C;

    vector<vector<int>> durations(L);
    for (int i = 0; i < L; ++i) {
        durations[i].resize(students_per_lab[i]);
        for (int j = 0; j < students_per_lab[i]; ++j) {
            cin >> durations[i][j];
        }
    }

    vector<int> inspection_times = schedule_inspections(durations, L, C, T);
    int total_occupied_time = calculate_total_occupied_time(durations, inspection_times, L);

    cout << "Schedule system (inspection times):\n";
    for (int t : inspection_times) {
        cout << t << " ";
    }
    cout << "\n";

    cout << "Total occupied time: " << total_occupied_time << "\n";

    return 0;
}
