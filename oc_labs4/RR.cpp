#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

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

// Функция планировщика Round Robin (карусель)
void planirovshchik_RR(vector<Protsess>& protsessy, int kvant, vector<int>& timeline) {
    int n = protsessy.size(); //кол-вл процессов
    int current_time = 0;
    int ptr = 0;  // Указатель на следующий прибывший процесс
    deque<int> ochered_gotovykh;  // Очередь готовых процессов (их индексов)
    int tekushchiy_protsess = -1;
    int schetchik_kvant = 0;
    int completed = 0;

    timeline.clear();

    while (completed < n) { //пока не завершились процессы - работаем
        // Добавляем прибывшие процессы в очередь
        while (ptr < n && protsessy[ptr].vremya_pribytiya <= current_time) {
            ochered_gotovykh.push_back(ptr);
            ++ptr;
        }

        bool sdelali_sho_to = false; //по умолчанию обрабатываем действие, иначе - обрабатываем застой
        if (tekushchiy_protsess != -1 || !ochered_gotovykh.empty()) {
            sdelali_sho_to = true;
            if (tekushchiy_protsess == -1) {
                // Выбираем следующий из очереди 
                tekushchiy_protsess = ochered_gotovykh.front();
                ochered_gotovykh.pop_front();
                schetchik_kvant = 0;
            }

            // Исполняем один единицу времени (записываем какой процесс работал и меняем оставшееся время)
            timeline.push_back(protsessy[tekushchiy_protsess].nomer_protsessa);
            protsessy[tekushchiy_protsess].ostavsheesya_vremya--;
            ++current_time;

            schetchik_kvant++;

            if (protsessy[tekushchiy_protsess].ostavsheesya_vremya == 0) { //процесс закончен
                protsessy[tekushchiy_protsess].vremya_zaversheniya = current_time;
                tekushchiy_protsess = -1;
                ++completed;
            }
            else if (schetchik_kvant == kvant) {
                // Квант истек, возвращаем в конец очереди
                ochered_gotovykh.push_back(tekushchiy_protsess);
                tekushchiy_protsess = -1;
                schetchik_kvant = 0;
            }
        }

        if (!sdelali_sho_to) { //если ничего не сделали
            if (ptr >= n) break;
            int next_pribytie = protsessy[ptr].vremya_pribytiya;
            for (int t = current_time; t < next_pribytie; ++t) {
                timeline.push_back(0);  // Простой
            }
            current_time = next_pribytie;
        }
    }
}