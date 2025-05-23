#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <numeric>
#include <cstdlib>   
#include <ctime> 

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

int calculate_total_unoccupied_time(const vector<vector<int>>& durations, const vector<int>& inspection_times) {
    int total_unoccupied = 0;
    int L = durations.size();
    int C = inspection_times.size();
    for (int i = 0; i < C; ++i) {
        int inspect_time = inspection_times[i];
        for (int l = 0; l < L; ++l) {
            if ((int)durations[l].size() > i) {
                int finish_time = compute_finish_time(durations[l], i + 1);
                // clamp to zero
                total_unoccupied += max(0, inspect_time - finish_time);
            }
        }
    }
    return total_unoccupied;
}

//----------------------optimal solution----------------------------------
int calculate_total_unoccupied_time_brute(const vector<vector<int>>& durations, const vector<int>& inspection_times) {
    int L = durations.size();
    int total_unoccupied = 0;
    for (int i = 0; i < inspection_times.size(); ++i) {
        int inspect_time = inspection_times[i];
        for (int l = 0; l < L; ++l) {
            if ((int)durations[l].size() > i) {
                int finish_time = compute_finish_time(durations[l], i + 1);
                total_unoccupied += max(0, inspect_time - finish_time);
            }
        }
    }
    return total_unoccupied;
}

vector<int> generate_next_combination(vector<int> current, int T) {
    int n = current.size();
    for (int i = n - 1; i >= 0; --i) {
        if (current[i] < T - (n - 1 - i)) {
            ++current[i];
            for (int j = i + 1; j < n; ++j) {
                current[j] = current[j - 1] + 1;
            }
            return current;
        }
    }
    return {};
}

vector<int> optimal_brute_force(const vector<vector<int>>& durations, int L, int C, int T) {
    vector<int> best_schedule;
    int min_unoccupied = numeric_limits<int>::max();
    vector<int> current(C);
    for (int i = 0; i < C; ++i) current[i] = i + 1;  // Start from 1 instead of 0 to avoid meaningless early inspections

    while (!current.empty() && current.back() < T) {
        bool valid = true;
        for (int i = 0; i < C; ++i) {
            int must_time = 0;
            for (auto &lab : durations) {
                if ((int)lab.size() > i) {
                    must_time = max(
                      must_time,
                      compute_finish_time(lab, i+1)
                    );
                }
            }
            if (current[i] < must_time) {
                valid = false;
                break;
            }
        }
        if (valid){
        int unoccupied = calculate_total_unoccupied_time_brute(durations, current);
        if (unoccupied < min_unoccupied) {
            min_unoccupied = unoccupied;
            best_schedule = current;
        }
        }
        current = generate_next_combination(current, T);
    }

   cout << "\nOptimal system (inspection times): ";
    for (int t : best_schedule) cout << t << " ";
    cout << "\nTotal optimal unoccupied time: " << min_unoccupied << "\n";

    return best_schedule;
}

//--------------------------generate instances-------------------------
vector<vector<int>> generate_random_instance(int L, int max_students, int max_duration) {
    vector<vector<int>> durations(L);
    for (int i = 0; i < L; ++i) {
        int num_students = rand() % max_students + 1;
        for (int j = 0; j < num_students; ++j) {
            durations[i].push_back(rand() % max_duration + 1);
        }
    }
    return durations;
}

int main() {
   int L = 3, C = 2, D = 1;
    int T = D * 24;

    // seed RNG
    srand(static_cast<unsigned>(time(nullptr)));

    // randomize bounds
    int max_students = rand() % 10 + 1;  // 1..10
    int max_duration = rand() % 24 + 1;  // 1..24
    cout << "Random instance: max_students=" << max_students
         << ", max_duration=" << max_duration << "\n";

    // generate and display random durations
    auto durations = generate_random_instance(L, max_students, max_duration);
    cout << "Durations per lab:\n";
    for (int i = 0; i < L; ++i) {
        cout << " Lab " << i + 1 << ": ";
        for (int x : durations[i]) cout << x << " ";
        cout << "\n";
    }

    // heuristic solution
    auto times = schedule_inspections(durations, L, C, T);
    int idle_approx = calculate_total_unoccupied_time(durations, times);
    cout << "\nSchedule system (inspection times): ";
    for (int t : times) cout << t << " ";
    cout << "\nTotal unoccupied time: " << idle_approx << "\n";

    // brute-force optimal solution
    optimal_brute_force(durations, L, C, T);

    return 0;
}

