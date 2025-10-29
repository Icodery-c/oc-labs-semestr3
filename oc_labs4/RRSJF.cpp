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

// Функция планировщика RR SJF (RR с переупорядочиванием по оставшемуся времени)
void planirovshchik_RRSJF(vector<Protsess>& protsessy, int kvant, vector<int>& timeline) {
    int n = protsessy.size();
    int current_time = 0;
    int ptr = 0;
    vector<int> ochered_gotovykh;  //для сортировки
    int tekushchiy_protsess = -1;
    int schetchik_kvant = 0;
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
                // Сортируем очередь по оставшемуся времени (ascending)
                sort(ochered_gotovykh.begin(), ochered_gotovykh.end(), [&](int a, int b) {
                    return protsessy[a].ostavsheesya_vremya < protsessy[b].ostavsheesya_vremya;
                    });
                if (!ochered_gotovykh.empty()) {
                    tekushchiy_protsess = ochered_gotovykh[0];
                    ochered_gotovykh.erase(ochered_gotovykh.begin());
                    schetchik_kvant = 0;
                }
            }

            // Исполняем один единицу
            timeline.push_back(protsessy[tekushchiy_protsess].nomer_protsessa);
            protsessy[tekushchiy_protsess].ostavsheesya_vremya--;
            ++current_time;

            schetchik_kvant++;

            if (protsessy[tekushchiy_protsess].ostavsheesya_vremya == 0) {
                protsessy[tekushchiy_protsess].vremya_zaversheniya = current_time;
                tekushchiy_protsess = -1;
                ++completed;
            }
            else if (schetchik_kvant == kvant) {
                // Квант истек, добавляем в очередь и сортируем позже
                ochered_gotovykh.push_back(tekushchiy_protsess);
                tekushchiy_protsess = -1;
                schetchik_kvant = 0;
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