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

// Функция планировщика SJF (самая короткая работа первой)
void planirovshchik_SJF(vector<Protsess>& protsessy, vector<int>& timeline) {
    int n = protsessy.size();
    int current_time = 0;
    int ptr = 0;
    vector<int> ochered_gotovykh;  // Список готовых (используем vector для поиска минимального по времени процесса)
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
                // Выбираем процесс с минимальной длительностью (невытесняющий)
                int min_dlit = INT_MAX; //начальный минимум (перезапишем)
                int min_idx = -1; //индекс выбранного процесса
                for (int idx : ochered_gotovykh) {
                    int dlit = protsessy[idx].ostavsheesya_vremya;
                    if (dlit < min_dlit) {
                        min_dlit = dlit; //если нашли, то перезаписываем
                        min_idx = idx;
                    }
                }
                if (min_idx != -1) {
                    tekushchiy_protsess = min_idx; 
                    // Удаляем из очереди
                    auto it = find(ochered_gotovykh.begin(), ochered_gotovykh.end(), min_idx);
                    ochered_gotovykh.erase(it);
                }
            }

            // Исполняем один единицу (продолжаем до конца, без вытеснения)
            timeline.push_back(protsessy[tekushchiy_protsess].nomer_protsessa);
            protsessy[tekushchiy_protsess].ostavsheesya_vremya--;
            ++current_time;

            if (protsessy[tekushchiy_protsess].ostavsheesya_vremya == 0) {
                protsessy[tekushchiy_protsess].vremya_zaversheniya = current_time;
                tekushchiy_protsess = -1;
                ++completed;
            }
        }

        if (!sdelali_sho_to) { 
            if (ptr >= n) break; //если все выполнились - останавливаемся
            int next_pribytie = protsessy[ptr].vremya_pribytiya;
            for (int t = current_time; t < next_pribytie; ++t) {
                timeline.push_back(0);
            }
            current_time = next_pribytie;
        }
    }
}