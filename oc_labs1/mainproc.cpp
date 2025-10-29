//| Функция                | Назначение                                                  |
//| ---------------------- | ----------------------------------------------------------- |
//| `fork()`               | Создаёт новый (дочерний) процесс как копию текущего         |
//| `exec()`               | Замещает код текущего процесса новым исполняемым файлом     |
//| `wait()` / `waitpid()` | Позволяет родительскому процессу ждать завершения дочернего |
//| `exit()`               | Завершает выполнение процесса                               |

#include <iostream>
#include <unistd.h>   // fork, execlp
#include <sys/wait.h> // waitpid
#include <cstdlib>    // exit

int main() {
    std::string str1, str2;

    std::cout << "Введите первую строку: ";
    std::getline(std::cin, str1);
    std::cout << "Введите вторую строку (для поиска): ";
    std::getline(std::cin, str2);

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Ошибка: не удалось создать процесс.\n";
        return 1;
    }

    if (pid == 0) {
        // Дочерний процесс — запускаем программу searchproc
        execlp("./searchproc", "searchproc", str1.c_str(), str2.c_str(), NULL);
        std::cerr << "Ошибка при запуске дочернего процесса.\n";
        exit(1);
    } else {
        // Родительский процесс ждёт завершения
        int status;
        waitpid(pid, &status, 0);
        std::cout << "\nДочерний процесс завершён.\n";
    }

    return 0;
}
