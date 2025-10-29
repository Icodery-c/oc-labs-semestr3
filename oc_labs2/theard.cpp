//| Что делает              | Windows (твой пример)                         | Linux (мой вариант)                        | Комментарий                     |
//| ----------------------- | --------------------------------------------- | ------------------------------------------ | ------------------------------- |
//| Создание потока         | `CreateThread()`                              | `pthread_create()`                         | Аналогичная функция.            |
//| Завершение потока       | `WaitForMultipleObjects()` и `CloseHandle()`  | `pthread_join()`                           | Ждут завершения потоков.        |
//| Приостановка выполнения | `Sleep(ms)`                                   | `usleep(microseconds)` / `sleep(seconds)`  | Аналогичная задержка.           |
//| Защита общих данных     | `CRITICAL_SECTION` + `EnterCriticalSection()` | `pthread_mutex_t` + `pthread_mutex_lock()` | То же самое, но в POSIX-стиле.  |
//| Остановка потока        | флаг `stop` или `TerminateThread()`           | флаг `stop` или `pthread_cancel()`         | На Linux безопаснее через флаг. |
//| Общие переменные        | Структура `ThreadData`                        | Глобальные атомарные или с мьютексом       | Поведение одно и то же.         |



#include <iostream>
#include <pthread.h>
#include <unistd.h>   // sleep(), usleep()
#include <atomic>     // atomic<int>

using namespace std;

// Глобальные счётчики
atomic<int> count1(0), count2(0), count3(0);

// Функции потоков
void* threadFunc1(void*) {
    while (true) {
        count1++;
        usleep(50000); // 50 мс
        pthread_testcancel(); // Точка, где поток может быть отменён
    }
    return nullptr;
}

void* threadFunc2(void*) {
    while (true) {
        count2++;
        usleep(70000); // 70 мс
        pthread_testcancel();
    }
    return nullptr;
}

void* threadFunc3(void*) {
    while (true) {
        count3++;
        usleep(90000); // 90 мс
        pthread_testcancel();
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2, t3;

    // Создаём три потока
    pthread_create(&t1, nullptr, threadFunc1, nullptr);
    pthread_create(&t2, nullptr, threadFunc2, nullptr);
    pthread_create(&t3, nullptr, threadFunc3, nullptr);

    cout << "Запущено 3 потока. Каждый увеличивает свой счётчик до 100.\n";

    while (true) {
        cout << "\rcount1=" << count1 
             << " | count2=" << count2 
             << " | count3=" << count3 << flush;

        if (count1 >= 100 && count2 >= 100 && count3 >= 100) {
            cout << "\nВсе потоки достигли 100. Завершаем их с помощью pthread_cancel().\n";
            pthread_cancel(t1);
            pthread_cancel(t2);
            pthread_cancel(t3);
            break;
        }

        usleep(100000); // Проверяем каждые 0.1 сек
    }

    // Ожидаем завершения всех потоков
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);

    cout << "\nФинальные значения:" << endl;
    cout << "count1 = " << count1 << endl;
    cout << "count2 = " << count2 << endl;
    cout << "count3 = " << count3 << endl;
    cout << "Все потоки завершены." << endl;

    return 0;
}
