#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace std;

// ===================== ГЛОБАЛНИ ПАРАМЕТРИ ======================
bool SHOW_EXPRESSION = true;
int MAX_WRONG = 10;
int DIFFICULTY = 2;
string THEME = "LIGHT";
int TIMER_PER_TURN = 20;

string HIGHSCORE_FILE = "scores.txt";
string SETTINGS_FILE = "settings.txt";

enum Language { ENGLISH, BULGARIAN };
Language currentLang = ENGLISH;

// ===================== ANSI COLOR ===========================
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

// ===================== КОНЗОЛНИ ФУНКЦИИ ====================
void clearScreen() {
#if defined(_WIN32)
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

void sleepSeconds(int sec) {
    this_thread::sleep_for(chrono::seconds(sec));
}

// ===================== ASCII HANGMAN ==========================
void printHangman(int wrong) {
    cout << "\n";
    cout << " +----+\n";
    cout << " |    |\n";

    if (wrong >= 1) cout << " O    |\n";
    else cout << "      |\n";

    if (wrong >= 3) cout << " |    |\n";
    else cout << "      |\n";

    if (wrong >= 4) cout << " /";
    else cout << "  ";

    if (wrong >= 5) cout << "|";
    else cout << " ";

    if (wrong >= 6) cout << "\\   |\n";
    else cout << "    |\n";

    if (wrong >= 7) cout << " /";
    else cout << "  ";

    if (wrong >= 8) cout << "\\   |\n";
    else cout << "    |\n";

    cout << "      |\n";
    cout << " ==========\n\n";
}

// ===================== ГЕНЕРАТОР НА ИЗРАЗ ===================
double generateExpression(string& exprStr) {
    int a, b;
    int diff = DIFFICULTY;

    if (diff == 1) { a = rand() % 10 + 1; b = rand() % 10 + 1; }
    else if (diff == 2) { a = rand() % 50 + 1; b = rand() % 50 + 1; }
    else { a = rand() % 100 + 1; b = rand() % 100 + 1; }

    int t = rand() % 6;
    double result = 0.0;
    stringstream ss;

    if (t == 0) { result = a + b; ss << a << " + " << b; }
    else if (t == 1) { result = a - b; ss << a << " - " << b; }
    else if (t == 2) { result = a * b; ss << a << " * " << b; }
    else if (t == 3) {
        if (b == 0) b = 1;
        a *= b;
        result = double(a) / b;
        ss << a << " / " << b;
    }
    else if (t == 4) {
        result = pow(a, b);
        ss << a << " ^ " << b;
    }
    else {
        a = rand() % 100 + 1;
        result = sqrt(a);
        ss << "sqrt(" << a << ")";
    }

    exprStr = ss.str();
    return result;
}

// ===================== HIGH SCORES ===========================
void saveScore(string name, int score) {
    ofstream f(HIGHSCORE_FILE, ios::app);
    if (f.is_open()) {
        f << name << " " << score << "\n";
        f.close();
    }
}

void showHighScores() {
    clearScreen();
    cout << BOLD << CYAN;
    if (currentLang == ENGLISH) cout << "====== TOP SCORES ======\n\n";
    else cout << "====== РЕЗУЛТАТИ ======\n\n";
    cout << RESET;

    ifstream f(HIGHSCORE_FILE);
    string line;
    bool empty = true;

    while (getline(f, line)) {
        if (!line.empty()) {
            cout << line << "\n";
            empty = false;
        }
    }

    if (empty) {
        if (currentLang == ENGLISH) cout << "No scores yet.\n";
        else cout << "Все още няма резултати.\n";
    }

    cout << "\n";
    if (currentLang == ENGLISH) cout << "Press Enter...";
    else cout << "Натиснете Enter...";
    cin.ignore();
    cin.get();
}

//===================== RULES ================================
void showRules() {
    clearScreen();
    cout << BOLD << CYAN;
    if (currentLang == ENGLISH) cout << "====== GAME RULES ======\n\n";
    else cout << "====== ПРАВИЛА НА ИГРАТА ======\n\n";
    cout << RESET;

    if (currentLang == ENGLISH) {
        cout << "Mind Riddle is a math game where you answer questions to earn points.\n";
        cout << "Each correct answer gives you points, but wrong answers cost points.\n";
        cout << "If you lose too many points or run out of time, the game ends.\n";
        cout << "Try to score as high as possible!\n\n";
    }
    else {
        cout << "Mind Riddle е математическа игра, където отговаряш на въпроси, за да печелиш точки.\n";
        cout << "Всяка правилна отговор дава точки, а грешните отнемат точки.\n";
        cout << "Ако загубиш твърде много точки или времето свърши, играта приключва.\n";
        cout << "Опитай се да получиш най-висок резултат!\n\n";
    }

    cout << (currentLang == ENGLISH ? "Press Enter..." : "Натиснете Enter...");
    cin.ignore();
    cin.get();
}

// ===================== ТАЙМЕР ================================
bool askInputWithTimer(string& input, int seconds) {
    auto start = chrono::steady_clock::now();

    cout << (currentLang == ENGLISH ? "You have " : "Имате ") << seconds << (currentLang == ENGLISH ? " seconds. Input: " : " секунди. Въведете: ");
    getline(cin, input);

    auto end = chrono::steady_clock::now();
    long duration = chrono::duration_cast<chrono::seconds>(end - start).count();

    return duration <= seconds;
}

// ===================== СКОР ================================
int calculateScore(bool win, int wrong, int maxWrong, int difficulty, double timeTaken) {
    if (!win) return 0;

    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (maxWrong - wrong) * 5;
    int timeBonus = max(0, int(max(0.0, 20.0 - timeTaken)) * 2);

    return base + diffBonus + livesBonus + timeBonus;
}

// ===================== НАСТРОЙКИ =============================
void settingsMenu() {
    while (true) {
        clearScreen();

        if (currentLang == ENGLISH) cout << "====== SETTINGS ======\n";
        else cout << "====== НАСТРОЙКИ ======\n";

        if (currentLang == ENGLISH) {
            cout << "1. Difficulty (current " << DIFFICULTY << ")\n";
            cout << "2. Max wrong attempts (current " << MAX_WRONG << ")\n";
            cout << "3. Show expression (current " << SHOW_EXPRESSION << ")\n";
            cout << "4. Timer per turn (current " << TIMER_PER_TURN << "s)\n";
            cout << "5. Theme (current " << THEME << ")\n";
            cout << "6. Language (current " << (currentLang == ENGLISH ? "ENGLISH" : "BULGARIAN") << ")\n";
            cout << "7. Back\n";
            cout << "Choice: ";
        }
        else {
            cout << "1. Трудност (сегашна " << DIFFICULTY << ")\n";
            cout << "2. Максимум грешки (сегашни " << MAX_WRONG << ")\n";
            cout << "3. Показвай израза (сегашно " << SHOW_EXPRESSION << ")\n";
            cout << "4. Таймер за ход (сегашни " << TIMER_PER_TURN << "с)\n";
            cout << "5. Тема (сегашна " << THEME << ")\n";
            cout << "6. Език (сегашен " << (currentLang == ENGLISH ? "ENGLISH" : "BULGARIAN") << ")\n";
            cout << "7. Назад\n";
            cout << "Избор: ";
        }

        int ch;
        cin >> ch;
        cin.ignore();

        if (ch == 1) {
            cout << (currentLang == ENGLISH ? "1=Easy 2=Medium 3=Hard: " : "1=Лесно 2=Средно 3=Трудно: ");
            cin >> DIFFICULTY; cin.ignore();
        }
        else if (ch == 2) {
            cout << (currentLang == ENGLISH ? "Enter max wrong attempts: " : "Въведи максимален брой грешки: ");
            cin >> MAX_WRONG; cin.ignore();
        }
        else if (ch == 3) {
            int x;
            cout << (currentLang == ENGLISH ? "Show expression? 1=Yes 0=No: " : "Показвай израза? 1=Да 0=Не: ");
            cin >> x; cin.ignore();
            SHOW_EXPRESSION = (x == 1);
        }
        else if (ch == 4) {
            cout << (currentLang == ENGLISH ? "Enter seconds per turn: " : "Въведи секунди за ход: ");
            cin >> TIMER_PER_TURN; cin.ignore();
        }
        else if (ch == 5) {
            cout << (currentLang == ENGLISH ? "Theme LIGHT/DARK: " : "Тема LIGHT/DARK: ");
            getline(cin, THEME);
        }
        else if (ch == 6) {
            // Смяна на езика
            if (currentLang == ENGLISH) currentLang = BULGARIAN;
            else currentLang = ENGLISH;
        }
        else if (ch == 7) {
            break;
        }
        else {
            cout << (currentLang == ENGLISH ? "Invalid option!" : "Невалидна опция!");
            cin.get();
        }
    }
}

// ===================== ИГРА ================================
void playGame() {
    clearScreen();

    string exprStr;
    double answer = generateExpression(exprStr);

    stringstream ss;
    ss << fixed << setprecision(2) << answer;
    string answerStr = ss.str();

    string hidden(answerStr.size(), '_');

    int wrong = 0;
    bool win = false;

    auto startTime = chrono::steady_clock::now();

    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== MATHEMATICAL HANGMAN ===\n";
    else cout << "=== МАТЕМАТИЧЕСКИ БЕСИЛКА ===\n";
    cout << RESET;

    if (SHOW_EXPRESSION) cout << (currentLang == ENGLISH ? "Expression: " : "Израз: ") << exprStr << "\n";
    else cout << (currentLang == ENGLISH ? "Expression is hidden!\n" : "Изразът е скрит!\n");

    while (wrong < MAX_WRONG && !win) {
        cout << "\n" << (currentLang == ENGLISH ? "Current: " : "Текущо: ") << hidden << "\n";
        cout << (currentLang == ENGLISH ? "Remaining wrong attempts: " : "Остават грешни опити: ") << (MAX_WRONG - wrong) << "\n";

        printHangman(wrong);

        string input;
        bool onTime = askInputWithTimer(input, TIMER_PER_TURN);

        if (!onTime) {
            cout << RED << (currentLang == ENGLISH ? "Time over!\n" : "Времето изтече!\n") << RESET;
            wrong++;
            continue;
        }
        if (input == answerStr) {
            win = true;
            break;
        }
        if (input.size() == 1) {
            char c = input[0];

            if (!((c >= '0' && c <= '9') || c == '.' || c == '-')) {
                cout << RED << (currentLang == ENGLISH ? "Invalid character!\n" : "Невалиден символ!\n") << RESET;
                continue;
            }

            bool found = false;

            for (int i = 0; i < answerStr.size(); i++) {
                if (answerStr[i] == c) {
                    hidden[i] = c;
                    found = true;
                }
            }

            if (!found) {
                wrong++;
                cout << RED << (currentLang == ENGLISH ? "Wrong guess!\n" : "Грешна буква!\n") << RESET;
            }

            if (hidden == answerStr) win = true;
        }
        else {
            cout << RED << (currentLang == ENGLISH ? "Invalid input!\n" : "Невалиден вход!\n") << RESET;
        }
    }

    auto endTime = chrono::steady_clock::now();
    double timeTaken = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();

    int score = calculateScore(win, wrong, MAX_WRONG, DIFFICULTY, timeTaken);

    clearScreen();

    if (win) {
        cout << GREEN << (currentLang == ENGLISH ? "=== YOU WIN! ===\nAnswer: " : "=== ПОБЕДА! ===\nОтговор: ") << answerStr << "\n";
        cout << (currentLang == ENGLISH ? "Score: " : "Точки: ") << score << RESET;

        cout << "\n" << (currentLang == ENGLISH ? "Enter your name: " : "Въведете името си: ");
        string name;
        getline(cin, name);

        saveScore(name, score);
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << (currentLang == ENGLISH ? "=== YOU LOSE ===\nAnswer: " : "=== ЗАГУБА ===\nОтговор: ") << answerStr << RESET;
    }

    cout << "\n" << (currentLang == ENGLISH ? "Press Enter..." : "Натиснете Enter...");
    cin.get();
}

// ===================== КЛАСИЧЕСКИ HANGMAN =====================
void playWordHangman() {
    clearScreen();
    vector<string> words = { "apple", "banana", "computer", "program", "hangman", "keyboard", "mouse", "window", "school", "game" };
    string word = words[rand() % words.size()];

    string hidden(word.size(), '_');
    int wrong = 0;
    bool win = false;

    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== WORD HANGMAN ===\n";
    else cout << "=== ДУМЕН HANGMAN ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG && !win) {
        cout << "\n" << (currentLang == ENGLISH ? "Current: " : "Текущо: ") << hidden << "\n";
        cout << (currentLang == ENGLISH ? "Remaining wrong attempts: " : "Остават грешни опити: ") << (MAX_WRONG - wrong) << "\n";

        printHangman(wrong);

        cout << (currentLang == ENGLISH ? "Enter a letter: " : "Въведете буква: ");
        string input;
        getline(cin, input);

        if (input.size() != 1 || !isalpha(input[0])) {
            cout << RED << (currentLang == ENGLISH ? "Invalid input!\n" : "Невалиден вход!\n") << RESET;
            continue;
        }

        char c = tolower(input[0]);
        bool found = false;
        for (int i = 0; i < word.size(); i++) {
            if (word[i] == c) {
                hidden[i] = c;
                found = true;
            }
        }

        if (!found) {
            wrong++;
            cout << RED << (currentLang == ENGLISH ? "Wrong guess!\n" : "Грешна буква!\n") << RESET;
        }

        if (hidden == word) win = true;
    }

    int score = (win ? (word.size() * 10 + (MAX_WRONG - wrong) * 5) : 0);

    clearScreen();
    if (win) {
        cout << GREEN << (currentLang == ENGLISH ? "=== YOU WIN! ===\nWord: " : "=== ПОБЕДА! ===\nДума: ") << word << "\n";
        cout << (currentLang == ENGLISH ? "Score: " : "Точки: ") << score << RESET;

        cout << "\n" << (currentLang == ENGLISH ? "Enter your name: " : "Въведете името си: ");
        string name;
        getline(cin, name);

        saveScore(name, score);
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << (currentLang == ENGLISH ? "=== YOU LOSE ===\nWord: " : "=== ЗАГУБА ===\nДума: ") << word << RESET;
    }

    cout << "\n" << (currentLang == ENGLISH ? "Press Enter..." : "Натиснете Enter...");
    cin.get();
}

// ===================== MAIN ================================
int main() {
    srand(time(0));

    while (true) {
        clearScreen();

        if (currentLang == ENGLISH)
            cout << "=== HANGMAN ULTRA ===\n";
        else
            cout << "=== HANGMAN УЛТРА ===\n";

        if (currentLang == ENGLISH) {
            cout << "1. Math Hangman\n";
            cout << "2. Word Hangman\n";
            cout << "3. High Scores\n";
            cout << "4. Settings\n";
            cout << "5. Rules\n";
            cout << "6. Exit\n";
            cout << "Choice: ";
        }
        else {
            cout << "1. Математически Hangman\n";
            cout << "2. Думен Hangman\n";
            cout << "3. Резултати\n";
            cout << "4. Настройки\n";
            cout << "5. Правила\n";
            cout << "6. Изход\n";
            cout << "Избор: ";
        }

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << (currentLang == ENGLISH ? "Invalid input! Please enter a number.\n" : "Невалиден избор! Въведете число.\n");
            cin.get();
            continue;
        }

        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: playGame(); break;
        case 2: playWordHangman(); break;
        case 3: showHighScores(); break;
        case 4: settingsMenu(); break;
        case 5: showRules(); break;
        case 6:
            cout << (currentLang == ENGLISH ? "Exiting... Goodbye!\n" : "Изход... Довиждане!\n");
            return 0;
        default:
            cout << (currentLang == ENGLISH ? "Invalid option! Try again.\n" : "Невалидна опция! Опитайте отново.\n");
        }

        cout << "\n" << (currentLang == ENGLISH ? "Press ENTER to return to the menu..." : "Натиснете ENTER, за да се върнете в менюто...");
        cin.get();
    }

    return 0;
}
 