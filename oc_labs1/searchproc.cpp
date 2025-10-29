//| Функция                | Назначение                                                  |
//| ---------------------- | ----------------------------------------------------------- |
//| `fork()`               | Создаёт новый (дочерний) процесс как копию текущего         |
//| `exec()`               | Замещает код текущего процесса новым исполняемым файлом     |
//| `wait()` / `waitpid()` | Позволяет родительскому процессу ждать завершения дочернего |
//| `exit()`               | Завершает выполнение процесса                               |


#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Префикс-функция (KMP)
vector<int> prefixFunction(const string& pattern) {
    vector<int> pi(pattern.size());
    for (size_t i = 1; i < pattern.size(); ++i) {
        int j = pi[i - 1];
        while (j > 0 && pattern[i] != pattern[j])
            j = pi[j - 1];
        if (pattern[i] == pattern[j])
            ++j;
        pi[i] = j;
    }
    return pi;
}

// Поиск подстроки (KMP)
int KMPSearch(const string& text, const string& pattern) {
    if (pattern.empty()) return -1;
    vector<int> pi = prefixFunction(pattern);
    int j = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j])
            j = pi[j - 1];
        if (text[i] == pattern[j])
            ++j;
        if (j == (int)pattern.size())
            return i - j + 1; // нашли вхождение
    }
    return -1; // не найдено
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Недостаточно аргументов.\n";
        return 1;
    }

    string text = argv[1];
    string pattern = argv[2];

    cout << "Поиск подстроки \"" << pattern << "\" в строке \"" << text << "\"...\n";

    int index = KMPSearch(text, pattern);
    cout << "Результат поиска: " << index << endl;

    return 0;
}
