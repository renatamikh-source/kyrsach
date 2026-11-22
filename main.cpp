#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Participant {
public:
    string fio;
    int birthYear;
    string country;
    string instrument;
    int place;

    Participant() : birthYear(0), place(0) {}

    Participant(string f, int y, string c, string i, int p)
        : fio(f), birthYear(y), country(c), instrument(i), place(p) {}

    int age() const { return 2026 - birthYear; }
};

vector<Participant> participants;
const string FILE_NAME = "participants.txt";

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            clearInput();
            return value;
        }
        cout << "Ошибка. Введите число от " << minVal
             << " до " << maxVal << ".\n";
        clearInput();
    }
}

string readLine(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "Поле не может быть пустым.\n";
    }
}

string cutString(const string& s, size_t width) {
    if (s.length() <= width) return s;
    return s.substr(0, width - 3) + "...";
}

void loadFromFile() {
    participants.clear();
    ifstream fin(FILE_NAME);
    if (!fin.is_open()) return;

    string line;
    int lineNo = 0;
    while (getline(fin, line)) {
        lineNo++;
        if (line.empty()) continue;

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 == string::npos ? 0 : p1 + 1);
        size_t p3 = line.find('|', p2 == string::npos ? 0 : p2 + 1);
        size_t p4 = line.find('|', p3 == string::npos ? 0 : p3 + 1);

        if (p1 == string::npos || p2 == string::npos ||
            p3 == string::npos || p4 == string::npos) {
            cout << "Пропущена битая строка " << lineNo << " в файле.\n";
            continue;
        }

        try {
            Participant p;
            p.fio        = line.substr(0, p1);
            p.birthYear  = stoi(line.substr(p1 + 1, p2 - p1 - 1));
            p.country    = line.substr(p2 + 1, p3 - p2 - 1);
            p.instrument = line.substr(p3 + 1, p4 - p3 - 1);
            p.place      = stoi(line.substr(p4 + 1));

            if (p.birthYear < 1900 || p.birthYear > 2026 ||
                p.place < 1 || p.place > 100) {
                cout << "Пропущена строка " << lineNo << ": неверные данные.\n";
                continue;
            }
            participants.push_back(p);
        }
        catch (...) {
            cout << "Пропущена строка " << lineNo << ": ошибка разбора.\n";
        }
    }
    fin.close();
}

void saveToFile() {
    ofstream fout(FILE_NAME);
    if (!fout.is_open()) {
        cout << "Не удалось сохранить файл.\n";
        return;
    }
    for (const auto& p : participants) {
        fout << p.fio << "|" << p.birthYear << "|"
             << p.country << "|" << p.instrument << "|"
             << p.place << "\n";
    }
    fout.close();
}

void sortParticipants() {
    sort(participants.begin(), participants.end(),
         [](const Participant& a, const Participant& b) {
             return a.fio < b.fio;
         });
}

void printHeader() {
    cout << left
         << setw(4)  << "№"
         << setw(25) << "Ф.И.О."
         << setw(6)  << "Год"
         << setw(5)  << "Возр"
         << setw(14) << "Страна"
         << setw(14) << "Инструмент"
         << setw(6)  << "Место" << "\n";
    cout << string(74, '-') << "\n";
}

void printAll() {
    if (participants.empty()) { cout << "Список пуст.\n"; return; }
    
    sortParticipants();
    saveToFile();

    printHeader();
    int num = 1;
    for (const auto& p : participants) {
        cout << left
             << setw(4)  << num++
             << setw(25) << cutString(p.fio, 22)
             << setw(6)  << p.birthYear
             << setw(5)  << p.age()
             << setw(14) << cutString(p.country, 13)
             << setw(14) << cutString(p.instrument, 13)
             << setw(6)  << p.place << "\n";
    }
}

void showTop3ByInstrument() {
    if (participants.empty()) { cout << "Список пуст.\n"; return; }

    vector<string> instruments;
    for (const auto& p : participants) {
        if (find(instruments.begin(), instruments.end(), p.instrument)
            == instruments.end())
            instruments.push_back(p.instrument);
    }

    for (const string& instr : instruments) {
        cout << "\nИнструмент: " << instr << "\n";
        printHeader();

        vector<Participant> winners;
        for (const auto& p : participants)
            if (p.instrument == instr && p.place >= 1 && p.place <= 3)
                winners.push_back(p);

        sort(winners.begin(), winners.end(),
             [](const Participant& a, const Participant& b) {
                 return a.place < b.place;
             });

        if (winners.empty()) {
            cout << "  нет призёров\n";
        } else {
            int num = 1;
            for (const auto& p : winners) {
                cout << left
                     << setw(4)  << num++
                     << setw(25) << cutString(p.fio, 22)
                     << setw(6)  << p.birthYear
                     << setw(5)  << p.age()
                     << setw(14) << cutString(p.country, 13)
                     << setw(14) << cutString(p.instrument, 13)
                     << setw(6)  << p.place << "\n";
            }
        }
    }
}

void showYoungWinners() {
    if (participants.empty()) { cout << "Список пуст.\n"; return; }

    vector<Participant> young;
    for (const auto& p : participants) {
        if (p.place >= 1 && p.place <= 3 && p.age() < 12)
            young.push_back(p);
    }

    if (young.empty()) {
        cout << "Победителей младше 12 лет нет.\n";
        return;
    }

    sort(young.begin(), young.end(),
         [](const Participant& a, const Participant& b) {
             return a.age() < b.age();
         });

    cout << "\nПобедители до 12 лет по возрастанию возраста\n";
    printHeader();
    int num = 1;
    for (const auto& p : young) {
        cout << left
             << setw(4)  << num++
             << setw(25) << cutString(p.fio, 22)
             << setw(6)  << p.birthYear
             << setw(5)  << p.age()
             << setw(14) << cutString(p.country, 13)
             << setw(14) << cutString(p.instrument, 13)
             << setw(6)  << p.place << "\n";
    }
}

bool isDuplicate(const Participant& p) {
    for (const auto& x : participants) {
        if (x.fio == p.fio && x.birthYear == p.birthYear &&
            x.instrument == p.instrument)
            return true;
    }
    return false;
}

void addParticipant() {
    Participant p;
    p.fio        = readLine("Ф.И.О.: ");
    p.birthYear  = readInt("Год рождения (1900-2026): ", 1900, 2026);
    p.country    = readLine("Страна: ");
    p.instrument = readLine("Инструмент: ");
    p.place      = readInt("Занятое место (1-100): ", 1, 100);

    if (isDuplicate(p)) {
        cout << "Такой участник уже есть. Не добавлен.\n";
        return;
    }

    participants.push_back(p);
    saveToFile();
    cout << "Запись добавлена и сохранена.\n";
}

void deleteParticipant() {
    if (participants.empty()) { cout << "Список пуст.\n"; return; }
    
    sortParticipants();
    printAll();
    
    int n = readInt("Введите номер записи для удаления: ",
                    1, (int)participants.size());
    
    participants.erase(participants.begin() + (n - 1));
    saveToFile();
    cout << "Удалено.\n";
}

int login() {
    string login_, pass;
    cout << "Логин: ";  getline(cin, login_);
    cout << "Пароль: "; getline(cin, pass);

    if (login_ == "admin" && pass == "admin123") return 1;
    if (login_ == "user"  && pass == "user123")  return 2;
    return 0;
}

void adminMenu() {
    int choice;
    do {
        cout << "\nМЕНЮ АДМИНИСТРАТОРА\n"
             << "1. Показать всех участников\n"
             << "2. Добавить участника\n"
             << "3. Удалить участника\n"
             << "4. Отчёт: топ-3 по инструментам\n"
             << "5. Отчёт: молодые победители до 12\n"
             << "0. Выход\n";
        choice = readInt("Выбор: ", 0, 5);
        switch (choice) {
            case 1: printAll(); break;
            case 2: addParticipant(); break;
            case 3: deleteParticipant(); break;
            case 4: showTop3ByInstrument(); break;
            case 5: showYoungWinners(); break;
            case 0: cout << "Выход.\n"; break;
        }
    } while (choice != 0);
}

void userMenu() {
    int choice;
    do {
        cout << "\nМЕНЮ ПОЛЬЗОВАТЕЛЯ\n"
             << "1. Показать всех участников\n"
             << "2. Отчёт: топ-3 по инструментам\n"
             << "3. Отчёт: молодые победители до 12\n"
             << "0. Выход\n";
        choice = readInt("Выбор: ", 0, 3);
        switch (choice) {
            case 1: printAll(); break;
            case 2: showTop3ByInstrument(); break;
            case 3: showYoungWinners(); break;
            case 0: cout << "Выход.\n"; break;
        }
    } while (choice != 0);
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    setlocale(LC_ALL, "Russian");

    loadFromFile();

    cout << "УЧЁТ МУЗЫКАЛЬНОГО КОНКУРСА\n";
    int role = login();

    if (role == 0) {
        cout << "Неверный логин или пароль. Доступ закрыт.\n";
        return 0;
    }
    cout << (role == 1 ? "Вы вошли как АДМИНИСТРАТОР.\n"
                       : "Вы вошли как ПОЛЬЗОВАТЕЛЬ.\n");

    if (role == 1) adminMenu();
    else           userMenu();

    return 0;
}