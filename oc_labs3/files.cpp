//| Задача                 | Windows API      | Linux/С++ аналог                                     |
//| ---------------------- | ---------------- | ---------------------------------------------------- |
//| Создать / открыть файл | `CreateFile`     | `fopen`, `ofstream`, `open`                          |
//| Записать данные        | `WriteFile`      | `fprintf`, `ofstream <<`, `write`                    |
//| Прочитать данные       | `ReadFile`       | `fscanf`, `ifstream >>`, `read`                      |
//| Переместить указатель  | `SetFilePointer` | `fseek` / `seekg`, `seekp`                           |
//| Закрыть файл           | `CloseHandle`    | `fclose`, автоматическое закрытие `fstream`          |
//| Удалить файл           | `DeleteFile`     | `remove("filename")`                                 |
//| Скопировать файл       | `CopyFile`       | вручную считать и записать / `std::filesystem::copy` |


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

struct CallInfo {
    std::string phoneNumber; // номер телефона
    double duration;          // время разговора (в минутах)
    double rate;              // тариф (руб/минута)
};

// Проверка формата номера телефона
bool isValidPhoneNumber(const std::string& phone) {
    std::regex pattern(R"(^(\+?\d{10,15})$)"); // допускает + и 10–15 цифр
    return std::regex_match(phone, pattern);
}

// Проверка уникальности номера
bool isUniqueNumber(const std::vector<CallInfo>& calls, const std::string& number) {
    for (const auto& c : calls) {
        if (c.phoneNumber == number)
            return false;
    }
    return true;
}

// Сохранение данных в файл
void saveCallsToFile(const std::string& filename) {
    int n;
    std::cout << "Введите количество записей о разговорах: ";
    std::cin >> n;

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Ошибка открытия файла для записи!\n";
        return;
    }

    std::vector<CallInfo> calls;
    for (int i = 0; i < n; ++i) {
        CallInfo call;
        std::cout << "\nЗапись #" << i + 1 << ":\n";

        // Проверка номера телефона
        while (true) {
            std::cout << "Номер телефона: ";
            std::cin >> call.phoneNumber;

            if (!isValidPhoneNumber(call.phoneNumber)) {
                std::cout << "❌ Ошибка: номер должен содержать только цифры (и опционально +) и быть длиной 10–15 символов.\n";
                continue;
            }
            if (!isUniqueNumber(calls, call.phoneNumber)) {
                std::cout << "❌ Ошибка: этот номер уже был введён!\n";
                continue;
            }
            break;
        }

        std::cout << "Время разговора (мин): ";
        std::cin >> call.duration;
        while (call.duration <= 0) {
            std::cout << "❌ Ошибка: время должно быть положительным!\n";
            std::cout << "Введите время разговора (мин): ";
            std::cin >> call.duration;
        }

        std::cout << "Тариф (руб/мин): ";
        std::cin >> call.rate;
        while (call.rate <= 0) {
            std::cout << "❌ Ошибка: тариф должен быть положительным!\n";
            std::cout << "Введите тариф (руб/мин): ";
            std::cin >> call.rate;
        }

        calls.push_back(call);
        fout << call.phoneNumber << " " << call.duration << " " << call.rate << "\n";
    }

    fout.close();
    std::cout << "\n✅ Данные успешно сохранены в файл '" << filename << "'.\n";
}

// Чтение данных из файла
std::vector<CallInfo> readCallsFromFile(const std::string& filename) {
    std::vector<CallInfo> calls;
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Ошибка открытия файла для чтения!\n";
        return calls;
    }

    CallInfo temp;
    while (fin >> temp.phoneNumber >> temp.duration >> temp.rate) {
        calls.push_back(temp);
    }

    fin.close();
    return calls;
}

// Расчёт суммы для заданного номера
void calculatePayment(const std::vector<CallInfo>& calls, const std::string& resultFile) {
    std::string searchNumber;
    std::cout << "\nВведите номер телефона для расчета суммы: ";
    std::cin >> searchNumber;

    if (!isValidPhoneNumber(searchNumber)) {
        std::cout << "❌ Некорректный формат номера телефона.\n";
        return;
    }

    double total = 0.0;
    bool found = false;

    for (const auto& c : calls) {
        if (c.phoneNumber == searchNumber) {
            total += c.duration * c.rate;
            found = true;
        }
    }

    std::ofstream fout(resultFile);
    if (!fout.is_open()) {
        std::cerr << "Ошибка открытия файла результатов!\n";
        return;
    }

    if (found) {
        std::cout << "\nАбонент " << searchNumber << " должен оплатить: " << total << " руб.\n";
        fout << "Абонент " << searchNumber << " должен оплатить: " << total << " руб.\n";
    } else {
        std::cout << "\nАбонент с номером " << searchNumber << " не найден.\n";
        fout << "Абонент с номером " << searchNumber << " не найден.\n";
    }

    fout.close();
    std::cout << "✅ Результаты записаны в файл '" << resultFile << "'.\n";
}

int main() {
    setlocale(LC_ALL, "ru");

    const std::string dataFile = "calls.txt";
    const std::string resultFile = "result.txt";

    int choice;
    std::cout << "Меню:\n"
              << "1. Ввести новые данные и сохранить в файл\n"
              << "2. Прочитать данные из файла и рассчитать оплату\n"
              << "Выберите действие (1 или 2): ";
    std::cin >> choice;

    if (choice == 1) {
        saveCallsToFile(dataFile);
    } else if (choice == 2) {
        auto calls = readCallsFromFile(dataFile);
        if (!calls.empty())
            calculatePayment(calls, resultFile);
        else
            std::cout << "Нет данных для обработки.\n";
    } else {
        std::cout << "Неверный выбор!\n";
    }

    return 0;
}
