#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <fstream>

using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int start = -1;
    int finish = 0;
    int turnaround = 0;
    int waiting = 0;
};

// Вывод таблицы
void printTable(const vector<Process>& processes, const string& algName) {
    ofstream fout("results.txt");

    cout << "\n==============================\n";
    cout << "Алгоритм: " << algName << "\n";
    cout << "==============================\n";
    cout << "ID\tArr\tBurst\tStart\tFinish\tTurn\tWait\n";

    fout << "Алгоритм: " << algName << "\n";
    fout << "ID\tArr\tBurst\tStart\tFinish\tTurn\tWait\n";

    double sumT = 0, sumM = 0, sumR = 0, sumP = 0;
    for (const auto& p : processes) {
        cout << p.id << "\t" << p.arrival << "\t" << p.burst << "\t"
             << p.start << "\t" << p.finish << "\t"
             << p.turnaround << "\t" << p.waiting << "\n";

        fout << p.id << "\t" << p.arrival << "\t" << p.burst << "\t"
             << p.start << "\t" << p.finish << "\t"
             << p.turnaround << "\t" << p.waiting << "\n";

        double t = p.burst;
        double T = p.turnaround;
        double M = T - t;
        double R = t / T;
        double P = T / t;
        sumT += T; sumM += M; sumR += R; sumP += P;
    }

    int n = processes.size();
    cout << "\nСредние значения:\n";
    cout << "T=" << sumT/n << "\tM=" << sumM/n
         << "\tR=" << sumR/n << "\tP=" << sumP/n << "\n";

    fout << "\nСредние значения:\n";
    fout << "T=" << sumT/n << "\tM=" << sumM/n
         << "\tR=" << sumR/n << "\tP=" << sumP/n << "\n";
    fout.close();
}

// ----------------- RR -----------------
void RR(vector<Process> processes, int quantum) {
    queue<int> q;
    int time = 0, n = processes.size();
    vector<bool> inQueue(n, false);

    while (true) {
        bool done = true;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && !inQueue[i]) {
                q.push(i);
                inQueue[i] = true;
            }
        }
        if (!q.empty()) {
            int i = q.front();
            q.pop();
            inQueue[i] = false;

            if (processes[i].start == -1)
                processes[i].start = time;

            int exec = min(quantum, processes[i].remaining);
            time += exec;
            processes[i].remaining -= exec;

            if (processes[i].remaining == 0) {
                processes[i].finish = time;
                processes[i].turnaround = processes[i].finish - processes[i].arrival;
                processes[i].waiting = processes[i].turnaround - processes[i].burst;
            } else {
                for (int j = 0; j < n; j++) {
                    if (processes[j].arrival <= time && processes[j].remaining > 0 && !inQueue[j]) {
                        q.push(j);
                        inQueue[j] = true;
                    }
                }
                q.push(i);
                inQueue[i] = true;
            }
            done = false;
        } else time++;
        if (done) break;
    }
    printTable(processes, "Round Robin");
}

// ----------------- SJF -----------------
void SJF(vector<Process> processes) {
    int time = 0, done = 0, n = processes.size();

    while (done < n) {
        int idx = -1;
        int shortest = 1e9;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].burst < shortest) {
                shortest = processes[i].burst;
                idx = i;
            }
        }
        if (idx == -1) { time++; continue; }

        if (processes[idx].start == -1)
            processes[idx].start = time;

        time += processes[idx].burst;
        processes[idx].remaining = 0;
        processes[idx].finish = time;
        processes[idx].turnaround = time - processes[idx].arrival;
        processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
        done++;
    }
    printTable(processes, "SJF (Shortest Job First)");
}

// ----------------- PSJF -----------------
void PSJF(vector<Process> processes) {
    int time = 0, done = 0, n = processes.size();

    while (done < n) {
        int idx = -1;
        int shortest = 1e9;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].remaining < shortest) {
                shortest = processes[i].remaining;
                idx = i;
            }
        }
        if (idx == -1) { time++; continue; }

        if (processes[idx].start == -1)
            processes[idx].start = time;

        processes[idx].remaining--;
        time++;

        if (processes[idx].remaining == 0) {
            processes[idx].finish = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            done++;
        }
    }
    printTable(processes, "PSJF (Preemptive SJF)");
}

// ----------------- RR-SJF -----------------
void RR_SJF(vector<Process> processes, int quantum) {
    int time = 0, n = processes.size();
    vector<Process*> ready;

    while (true) {
        bool done = true;

        for (auto &p : processes)
            if (p.remaining > 0) { done = false; break; }

        if (done) break;

        for (auto &p : processes)
            if (p.arrival <= time && p.remaining > 0 && find(ready.begin(), ready.end(), &p) == ready.end())
                ready.push_back(&p);

        if (ready.empty()) { time++; continue; }

        sort(ready.begin(), ready.end(), [](Process* a, Process* b){
            return a->remaining < b->remaining;
        });

        Process* current = ready.front();
        ready.erase(ready.begin());

        if (current->start == -1)
            current->start = time;

        int exec = min(quantum, current->remaining);
        time += exec;
        current->remaining -= exec;

        if (current->remaining == 0) {
            current->finish = time;
            current->turnaround = time - current->arrival;
            current->waiting = current->turnaround - current->burst;
        } else {
            for (auto &p : processes)
                if (p.arrival <= time && p.remaining > 0 && find(ready.begin(), ready.end(), &p) == ready.end())
                    ready.push_back(&p);
            ready.push_back(current);
        }
    }
    printTable(processes, "RR-SJF");
}

// ----------------- MAIN -----------------
int main() {
    setlocale(LC_ALL, "ru");

    int n;
    cout << "Введите количество процессов: ";
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        cout << "Процесс " << i + 1 << " (время поступления и длительность): ";
        cin >> processes[i].arrival >> processes[i].burst;
        processes[i].id = i + 1;
        processes[i].remaining = processes[i].burst;
    }

    cout << "\nВыберите алгоритм:\n";
    cout << "1 - RR (Round Robin)\n";
    cout << "2 - SJF\n";
    cout << "3 - PSJF\n";
    cout << "4 - RR-SJF\n";
    int choice;
    cin >> choice;

    if (choice == 1) {
        int q;
        cout << "Введите квант времени: ";
        cin >> q;
        RR(processes, q);
    } else if (choice == 2) {
        SJF(processes);
    } else if (choice == 3) {
        PSJF(processes);
    } else if (choice == 4) {
        int q;
        cout << "Введите квант времени: ";
        cin >> q;
        RR_SJF(processes, q);
    } else {
        cout << "Неверный выбор!\n";
    }

    cout << "\nРезультаты сохранены в файл results.txt\n";
    return 0;
}
