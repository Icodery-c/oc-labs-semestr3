#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

// Определение структуры для процесса (нужно для доступа к полям)
struct Protsess {
    int nomer_protsessa;
    int vremya_pribytiya;
    int dlitelnost_protsessa;
    int ostavsheesya_vremya;
    int vremya_zaversheniya = 0;
    double T = 0.0;
    double M = 0.0;
    double R = 0.0;
    double P = 0.0;
};

// Функция планировщика PSJF (прерывает текущий процесс если находится процесс с наименьшим выполненением)
void planirovshchik_PSJF(vector<Protsess>& protsessy, vector<int>& timeline) {
    int n = protsessy.size();
    int current_time = 0;
    int ptr = 0;
    vector<int> ochered_gotovykh;
    int tekushchiy_protsess = -1;
    int completed = 0;

    timeline.clear();

    while (completed < n) {
        // Добавляем прибывшие
        while (ptr < n && protsessy[ptr].vremya_pribytiya <= current_time) {
            ochered_gotovykh.push_back(ptr);
            ++ptr;
        }

        bool sdelali_sho_to = false;
        if (tekushchiy_protsess != -1 || !ochered_gotovykh.empty()) {
            sdelali_sho_to = true;
            if (tekushchiy_protsess == -1) {
                // Выбираем с минимальным оставшимся временем
                int min_ost = INT_MAX;
                int min_idx = -1;
                for (int idx : ochered_gotovykh) {
                    int ost = protsessy[idx].ostavsheesya_vremya;
                    if (ost < min_ost) {
                        min_ost = ost;
                        min_idx = idx;
                    }
                }
                if (min_idx != -1) {
                    tekushchiy_protsess = min_idx;
                    auto it = find(ochered_gotovykh.begin(), ochered_gotovykh.end(), min_idx);
                    ochered_gotovykh.erase(it);
                }
            }

            // Исполняем один единицу
            timeline.push_back(protsessy[tekushchiy_protsess].nomer_protsessa);
            protsessy[tekushchiy_protsess].ostavsheesya_vremya--;
            ++current_time;

            if (protsessy[tekushchiy_protsess].ostavsheesya_vremya == 0) {
                protsessy[tekushchiy_protsess].vremya_zaversheniya = current_time;
                tekushchiy_protsess = -1;
                ++completed;
            }
            else {
                // Проверяем на вытеснение: если в очереди есть короче текущего
                int min_ost_v_ocheredi = INT_MAX;
                for (int idx : ochered_gotovykh) {
                    int ost = protsessy[idx].ostavsheesya_vremya;
                    if (ost < min_ost_v_ocheredi) {
                        min_ost_v_ocheredi = ost;
                    }
                }
                if (!ochered_gotovykh.empty() && min_ost_v_ocheredi < protsessy[tekushchiy_protsess].ostavsheesya_vremya) {
                    // Вытесняем текущий
                    ochered_gotovykh.push_back(tekushchiy_protsess);
                    // Выбираем новый минимум
                    int noviy_min_ost = INT_MAX;
                    int noviy_min_idx = -1;
                    for (int idx : ochered_gotovykh) {
                        int ost = protsessy[idx].ostavsheesya_vremya;
                        if (ost < noviy_min_ost) {
                            noviy_min_ost = ost;
                            noviy_min_idx = idx;
                        }
                    }
                    tekushchiy_protsess = noviy_min_idx;
                    auto it = find(ochered_gotovykh.begin(), ochered_gotovykh.end(), noviy_min_idx);
                    ochered_gotovykh.erase(it);
                }
            }
        }

        if (!sdelali_sho_to) {
            if (ptr >= n) break;
            int next_pribytie = protsessy[ptr].vremya_pribytiya;
            for (int t = current_time; t < next_pribytie; ++t) {
                timeline.push_back(0);
            }
            current_time = next_pribytie;
        }
    }
}