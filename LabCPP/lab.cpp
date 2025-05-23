﻿#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <climits>
#include <algorithm>
#include "queue.h"

using namespace std;

struct BoardState {
    string state;
    int parent_idx;
};

static int step_counter = 0;

vector<string> reconstruct_path(const vector<BoardState>& states, int goal_idx) {
    vector<string> path;
    int current = goal_idx;
    while (current != -1) {
        path.push_back(states[current].state);
        current = states[current].parent_idx;
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<string> solve_puzzle8(const string& initial) {
    const string goal = "123456780";
    Queue* queue = queue_create();
    unordered_map<string, bool> visited;
    vector<BoardState> states;

    if (initial.length() != 9) {
        throw runtime_error("Invalid initial state length");
    }

    states.push_back({ initial, -1 });
    queue_insert(queue, 0);

    while (!queue_empty(queue)) {
        int current_idx = queue_get(queue);
        queue_remove(queue);
        BoardState current = states[current_idx];

        if (current.state == goal) {
            queue_delete(queue);
            return reconstruct_path(states, current_idx);
        }

        if (visited[current.state]) continue;
        visited[current.state] = true;

        size_t zero_pos = current.state.find('0');
        if (zero_pos == string::npos) continue;

        const int directions[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

        for (int i = 0; i < 4; i++) {
            int x = static_cast<int>(zero_pos % 3) + directions[i][0];
            int y = static_cast<int>(zero_pos / 3) + directions[i][1];

            if (x >= 0 && x < 3 && y >= 0 && y < 3) {
                string next = current.state;
                size_t swap_pos = static_cast<size_t>(y * 3 + x);

                if (swap_pos < next.length() && zero_pos < next.length()) {
                    swap(next[zero_pos], next[swap_pos]);

                    if (!visited[next]) {
                        states.push_back({ next, current_idx });
                        if (states.size() > static_cast<size_t>(INT_MAX)) {
                            throw runtime_error("Too many states generated");
                        }
                        queue_insert(queue, static_cast<int>(states.size() - 1));
                    }
                }
            }
        }
    }

    queue_delete(queue);
    return {};
}

void print_board(const string& state) {
    cout << "Шаг " << step_counter++ << ":" << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            const char c = state[i * 3 + j];
            if (c == '0') cout << "  ";
            else cout << c << " ";
        }
        cout << endl;
    }
    cout << "-----" << endl;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    step_counter = 0;

    string initial;
    if (argc >= 2) {
        ifstream inputFile(argv[1]);
        if (!inputFile) {
            cerr << "Ошибка: не удалось открыть файл " << argv[1] << endl;
            return 1;
        }
        inputFile >> initial;
    }
    else {
        cout << "Введите начальное состояние (9 цифр, 0 для пустой клетки): ";
        cin >> initial;
    }

    if (initial.length() != 9) {
        cerr << "Ошибка: требуется ровно 9 цифр (0-8)." << endl;
        return 1;
    }

    try {
        vector<string> solution = solve_puzzle8(initial);
        if (solution.empty()) {
            cout << "Решение не найдено." << endl;
        }
        else {
            cout << "\nПуть решения (" << solution.size() - 1 << " ходов):\n";
            for (const string& state : solution) {
                print_board(state);
            }
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}