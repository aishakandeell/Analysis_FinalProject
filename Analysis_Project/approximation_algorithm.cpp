#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Forward declarations
int compute_finish_time(const vector<int>& durations, int k);
vector<int> schedule_inspections(const vector<vector<int>>& durations, int L, int C, int T);
int calculate_total_unoccupied_time(const vector<vector<int>>& durations, const vector<int>& inspection_times);
vector<int> generate_next_combination(vector<int> current, int T);
vector<int> optimal_brute_force(const vector<vector<int>>& durations, int L, int C, int T);
vector<vector<int>> generate_random_instance(int L, int max_students, int max_duration);

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

vector<int> optimal_brute_force(
    const vector<vector<int>>& durations,
    int L,               // number of labs
    int C,               // number of inspections
    int T                // time horizon
) {
    // 1) collect every finish‐time event
    vector<int> events;
    for (int l = 0; l < L; ++l) {
        for (int k = 1; k <= (int)durations[l].size(); ++k) {
            int ft = compute_finish_time(durations[l], k);
            if (ft <= T) events.push_back(ft);
        }
    }
    sort(events.begin(), events.end());
    events.erase(unique(events.begin(), events.end()), events.end());
    int N = events.size();
    if (N < C) return {};  // not enough distinct events to schedule C inspections

    // 2) set up initial combination of indices [0,1,2,...,C-1]
    vector<int> idx(C);
    iota(idx.begin(), idx.end(), 0);

    vector<int> best_schedule;
    int best_idle = numeric_limits<int>::max();

    // 3) iterate over all C-combinations of the N events
    while (true) {
        // build the candidate inspection times
        vector<int> cand(C);
        for (int i = 0; i < C; ++i) {
            cand[i] = events[idx[i]];
        }
        // evaluate
        int idle = calculate_total_unoccupied_time(durations, cand);
        if (idle < best_idle) {
            best_idle = idle;
            best_schedule = cand;
        }

        // advance to next combination
        int i = C - 1;
        while (i >= 0 && idx[i] == N - C + i) --i;
        if (i < 0) break;                // we have exhausted all combinations
        ++idx[i];
        for (int j = i + 1; j < C; ++j) {
            idx[j] = idx[j-1] + 1;
        }
    }

    return best_schedule;
}

vector<vector<int>> generate_random_instance(int L, int max_students, int max_duration) {
    vector<vector<int>> durations(L);
    for (int i = 0; i < L; ++i) {
        int num_students = rand() % max_students + 1;
        for (int j = 0; j < num_students; ++j) {
            durations[i].push_back(rand() % max_duration + 1);
        }
        sort(durations[i].begin(), durations[i].end());
    }
    return durations;
}

int main() {
    const int L = 3;
    const int C = 2;
    const int D = 1;
    const int T = D * 24;

    srand(static_cast<unsigned>(time(nullptr)));

    int max_students = rand() % 10 + 1;
    int max_duration = rand() % 24 + 1;
    
    cout << "Instance parameters:\n";
    cout << "Labs: " << L << ", Inspections: " << C << ", Time: " << T << "h\n";
    cout << "Max students: " << max_students << ", Max duration: " << max_duration << "h\n";

    auto durations = generate_random_instance(L, max_students, max_duration);
    
    cout << "\nLab durations:\n";
    for (int i = 0; i < L; ++i) {
        cout << "Lab " << i+1 << ": ";
        for (int d : durations[i]) cout << d << " ";
        cout << "\n";
    }

    // Heuristic solution
    auto start_heuristic = high_resolution_clock::now();
    auto heuristic_times = schedule_inspections(durations, L, C, T);
    int heuristic_idle = calculate_total_unoccupied_time(durations, heuristic_times);
    auto end_heuristic = high_resolution_clock::now();
    
    cout << "\nHeuristic solution (" 
         << duration_cast<microseconds>(end_heuristic - start_heuristic).count() << "μs):\n";
    cout << "Times: ";
    for (int t : heuristic_times) cout << t << " ";
    cout << "\nIdle time: " << heuristic_idle << "\n";

    // Brute-force solution
    auto optimal_times = optimal_brute_force(durations, L, C, T);
    if (!optimal_times.empty()) {
        int optimal_idle = calculate_total_unoccupied_time(durations, optimal_times);
        cout << "\nOptimal solution:\n";
        cout << "Times: ";
        for (int t : optimal_times) cout << t << " ";
        cout << "\nIdle time: " << optimal_idle << "\n";
        
        cout << "\nComparison:\n";
        cout << "Heuristic idle: " << heuristic_idle << "\n";
        cout << "Optimal idle: " << optimal_idle << "\n";
        cout << "Difference: " << (heuristic_idle - optimal_idle) 
             << " (" << 100.0*(heuristic_idle - optimal_idle)/optimal_idle << "% worse)\n";
    } else {
        cout << "\nNo valid optimal schedule found within time limit.\n";
    }

    return 0;
}
