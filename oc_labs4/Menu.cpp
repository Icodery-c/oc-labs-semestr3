#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

// Объявление структуры для процесса
struct Protsess {
    int nomer_protsessa;
    int vremya_pribytiya;
    int dlitelnost_protsessa;
    int ostavsheesya_vremya;
    int vremya_zaversheniya = 0;
    double T = 0.0; //общее время пребывания процесса в системе
    double M = 0.0; //потерянное время
    double R = 0.0; //доля процессорного времени
    double P = 0.0; //во сколько раз общее время выполнения процесса превышает  процессорное время
};

// Объявления функций из других файлов
void planirovshchik_RR(vector<Protsess>& protsessy, int kvant, vector<int>& timeline);
void planirovshchik_SJF(vector<Protsess>& protsessy, vector<int>& timeline);
void planirovshchik_PSJF(vector<Protsess>& protsessy, vector<int>& timeline);
void planirovshchik_RRSJF(vector<Protsess>& protsessy, int kvant, vector<int>& timeline);

// Функция для расчета показателей эффективности
void raschet_pokazateley(vector<Protsess>& protsessy) {
    // Для каждого процесса считаем показатели
    for (Protsess& pr : protsessy) {
        pr.T = pr.vremya_zaversheniya - pr.vremya_pribytiya;
        pr.M = pr.T - pr.dlitelnost_protsessa;
        pr.R = (double)pr.dlitelnost_protsessa / pr.T;
        pr.P = pr.T / (double)pr.dlitelnost_protsessa;
    }
}

// Функция для вывода таблицы (как в примере: Г - готовность, И - исполнение)
void vyvod_tablitsy(const vector<Protsess>& protsessy, const vector<int>& timeline) {
    if (timeline.empty()) return;
    int max_vremya = timeline.size();  //timeline - это общее время

    // Вывод заголовка: ручное центрирование (цифра на 2-й позиции)
    cout << "   ";  // 3 символа для столбца номеров (центр пустой)
    for (int t = 0; t < max_vremya; ++t) {
        int num = t + 1;
        if (num < 10) {
            cout << " " << num << " ";  // " 1 " — цифра на 2-й позиции
        }
        else {
            cout << (num / 10) << (num % 10) << " ";  // "10 " — вторая цифра на 2-й позиции
        }
    }
    cout << endl;

    // Вывод строк для каждого процесса
    for (size_t i = 0; i < protsessy.size(); ++i) {
        int nom = protsessy[i].nomer_protsessa;
        if (nom < 10) {
            cout << " " << nom << " ";  // " 1 " — номер на 2-й позиции
        }
        else {
            cout << (nom / 10) << (nom % 10) << " ";  // Для больших номеров
        }
        for (int t = 0; t < max_vremya; ++t) {
            if (protsessy[i].vremya_pribytiya > t) {
                cout << " H ";  // Буква на 2-й позиции
            }
            else if (protsessy[i].vremya_zaversheniya <= t) {
                cout << " Z ";  // Буква на 2-й позиции
            }
            else {
                if (timeline[t] == protsessy[i].nomer_protsessa) {
                    cout << " И ";  // Буква на 2-й позиции
                }
                else {
                    cout << " Г ";  // Буква на 2-й позиции
                }
            }
        }
        cout << endl;
    }
    cout << endl;
}

// Функция для вывода показателей
void vyvod_pokazateley(const vector<Protsess>& protsessy) {
    cout << "Показатели для каждого процесса:" << endl;
    for (const Protsess& pr : protsessy) {
        cout << "Процесс " << pr.nomer_protsessa
            << ": T=" << fixed << setprecision(2) << pr.T
            << ", M=" << pr.M
            << ", R=" << pr.R
            << ", P=" << pr.P << endl;
    }

    // Средние значения
    double sum_T = 0.0;
    double sum_M = 0.0;
    double sum_R = 0.0;
    double sum_P = 0.0;
    for (const Protsess& pr : protsessy) {
        sum_T += pr.T;
        sum_M += pr.M;
        sum_R += pr.R;
        sum_P += pr.P;
    }
    int n = protsessy.size();
    cout << "Средние значения: T=" << sum_T / n
        << ", M=" << sum_M / n
        << ", R=" << sum_R / n
        << ", P=" << sum_P / n << endl;
    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");
    int kolichestvo_protsessov;
    cout << "Введите количество процессов: ";
    cin >> kolichestvo_protsessov;

    vector<Protsess> protsessy(kolichestvo_protsessov);
    for (int i = 0; i < kolichestvo_protsessov; ++i) {
        protsessy[i].nomer_protsessa = i + 1;
        cout << "Для процесса " << (i + 1) << ":" << endl;
        cout << "  Время прибытия: ";
        cin >> protsessy[i].vremya_pribytiya;
        cout << "  Длительность: ";
        cin >> protsessy[i].dlitelnost_protsessa;
        protsessy[i].ostavsheesya_vremya = protsessy[i].dlitelnost_protsessa;
    }

    // Сортируем процессы по времени прибытия
    sort(protsessy.begin(), protsessy.end(), [](const Protsess& a, const Protsess& b) {
        return a.vremya_pribytiya < b.vremya_pribytiya;
        });

    int nomer_algoritma;
    cout << "Выберите алгоритм (1 - RR, 2 - SJF, 3 - PSJF, 4 - RR SJF): ";
    cin >> nomer_algoritma;

    int kvant = 0;
    if (nomer_algoritma == 1 || nomer_algoritma == 4) {
        cout << "Введите кванта времени для RR: ";
        cin >> kvant;
    }

    vector<int> timeline;  // Хронология исполнения (номер процесса в каждом слоте времени)

    // Вызов соответствующего планировщика
    if (nomer_algoritma == 1) {
        planirovshchik_RR(protsessy, kvant, timeline);
    }
    else if (nomer_algoritma == 2) {
        planirovshchik_SJF(protsessy, timeline);
    }
    else if (nomer_algoritma == 3) {
        planirovshchik_PSJF(protsessy, timeline);
    }
    else if (nomer_algoritma == 4) {
        planirovshchik_RRSJF(protsessy, kvant, timeline);
    }
    else {
        cout << "Неверный алгоритм!" << endl;
        return 1;
    }

    // Расчет и вывод результатов
    raschet_pokazateley(protsessy);
    vyvod_tablitsy(protsessy, timeline);
    vyvod_pokazateley(protsessy);

    return 0;
}