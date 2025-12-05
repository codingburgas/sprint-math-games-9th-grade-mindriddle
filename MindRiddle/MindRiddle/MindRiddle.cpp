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
#include <algorithm>
#include <cctype>

#if defined(_WIN32)
#include <windows.h>
#endif

using namespace std;

bool SHOW_EXPRESSION = true;
int MAX_WRONG = 6;
int DIFFICULTY = 2;
string THEME = "LIGHT";
int TIMER_PER_TURN = 20;

string HIGHSCORE_FILE = "scores.txt";
string SETTINGS_FILE = "settings.txt";

enum Language { ENGLISH, BULGARIAN };
Language currentLang = ENGLISH;

// ===== Добавени категории =====
int MATH_CATEGORY = 2; // 1=Easy, 2=Medium, 3=Hard
int WORD_CATEGORY = 4; // 1=Animals, 2=Foods, 3=Tech, 4=Random

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

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

void printHangman(int wrong) {
    cout << "\n";
    cout << "  +----+\n";
    cout << "  |    |\n";
    if (wrong >= 1) cout << "  |    O\n"; else cout << "  |     \n";
    if (wrong == 0) cout << "  |     \n";
    else if (wrong == 1) cout << "  |     \n";
    else if (wrong == 2) cout << "  |    |\n";
    else if (wrong == 3) cout << "  |   /|\n";
    else if (wrong >= 4) cout << "  |   /|\\\n";
    if (wrong == 0) cout << "  |     \n";
    else if (wrong <= 4) cout << "  |     \n";
    else if (wrong == 5) cout << "  |   / \n";
    else if (wrong >= 6) cout << "  |   / \\\n";
    cout << "  |     \n";
    cout << "==========\n\n";
}

// ===== Math Hangman категории =====
void chooseMathCategory() {
    clearScreen();
    cout << "=== MATH CATEGORIES ===\n";
    cout << "1. Easy (+ -)\n";
    cout << "2. Medium (+ - * /)\n";
    cout << "3. Hard (+ - * / ^ sqrt)\n";
    cout << "Choose: ";

    int c;
    cin >> c;
    cin.ignore();
    if (c >= 1 && c <= 3) MATH_CATEGORY = c;
    else MATH_CATEGORY = 2;
}

// ===== Word Hangman категории =====
void chooseWordCategory() {
    clearScreen();
    cout << "=== WORD CATEGORIES ===\n";
    cout << "1. Animals\n";
    cout << "2. Foods\n";
    cout << "3. Technology\n";
    cout << "4. Random\n";
    cout << "Choose: ";

    int c;
    cin >> c;
    cin.ignore();
    if (c >= 1 && c <= 4) WORD_CATEGORY = c;
    else WORD_CATEGORY = 4;
}

// ===== Math Hangman: генератор на изрази с категории =====
double generateExpression(string& exprStr) {
    int a, b;
    if (DIFFICULTY == 1) { a = rand() % 10 + 1; b = rand() % 10 + 1; }
    else if (DIFFICULTY == 2) { a = rand() % 50 + 1; b = rand() % 50 + 1; }
    else { a = rand() % 100 + 1; b = rand() % 100 + 1; }

    int t = 0;
    if (MATH_CATEGORY == 1) t = rand() % 2;
    else if (MATH_CATEGORY == 2) t = rand() % 4;
    else t = rand() % 6;

    double result = 0.0;
    stringstream ss;
    if (t == 0) { result = a + b; ss << a << " + " << b; }
    else if (t == 1) { result = a - b; ss << a << " - " << b; }
    else if (t == 2) { result = a * b; ss << a << " * " << b; }
    else if (t == 3) { if (b == 0) b = 1; result = double(a) / b; ss << a << " / " << b; }
    else if (t == 4) { int x = rand() % 10 + 1; int e = rand() % 4 + 1; result = pow(x, e); ss << x << " ^ " << e; }
    else { int r = rand() % 10 + 1; int sq = r * r; result = r; ss << "sqrt(" << sq << ")"; }

    exprStr = ss.str();
    return result;
}

void saveScore(string name, int score) {
    ofstream f(HIGHSCORE_FILE, ios::app);
    if (f.is_open()) { f << name << " " << score << "\n"; f.close(); }
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
    while (getline(f, line)) { if (!line.empty()) { cout << line << "\n"; empty = false; } }
    if (empty) { if (currentLang == ENGLISH) cout << "No scores yet.\n"; else cout << "Все още няма резултати.\n"; }
    cout << "\n";
    if (currentLang == ENGLISH) cout << "Press Enter..."; else cout << "Натиснете Enter...";
    cin.ignore();
    cin.get();
}

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

bool askInputWithTimer(string& input, int seconds) {
    auto start = chrono::steady_clock::now();
    cout << (currentLang == ENGLISH ? "You have " : "Имате ") << seconds << (currentLang == ENGLISH ? " seconds. Input: " : " секунди. Въведете: ");
    getline(cin, input);
    auto end = chrono::steady_clock::now();
    long duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    return duration <= seconds;
}

int calculateScore(bool win, int wrong, int maxWrong, int difficulty, double timeTaken) {
    if (!win) return 0;
    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (maxWrong - wrong) * 5;
    int timeBonus = max(0, int(max(0.0, 20.0 - timeTaken)) * 2);
    return base + diffBonus + livesBonus + timeBonus;
}

void settingsMenu() {
    while (true) {
        clearScreen();
        if (currentLang == ENGLISH) cout << "====== SETTINGS ======\n";
        else cout << "====== НАСТРОЙКИ ======\n";
        if (currentLang == ENGLISH) {
            cout << "1. Show expression (current " << SHOW_EXPRESSION << ")\n";
            cout << "2. Timer per turn (current " << TIMER_PER_TURN << "s)\n";
            cout << "3. Language (current " << (currentLang == ENGLISH ? "ENGLISH" : "BULGARIAN") << ")\n";
            cout << "4. Back\n";
            cout << "Choice: ";
        }
        else {
            cout << "1. Показвай израза (сегашно " << SHOW_EXPRESSION << ")\n";
            cout << "2. Таймер за ход (сегашни " << TIMER_PER_TURN << "с)\n";
            cout << "3. Език (сегашен " << (currentLang == ENGLISH ? "ENGLISH" : "BULGARIAN") << ")\n";
            cout << "4. Назад\n";
            cout << "Избор: ";
        }
        int ch; cin >> ch; cin.ignore();
        if (ch == 1) { int x; cout << (currentLang == ENGLISH ? "Show expression? 1=Yes 0=No: " : "Показвай израза? 1=Да 0=Не: "); cin >> x; cin.ignore(); SHOW_EXPRESSION = (x == 1); }
        else if (ch == 2) { cout << (currentLang == ENGLISH ? "Enter seconds per turn: " : "Въведи секунди за ход: "); cin >> TIMER_PER_TURN; cin.ignore(); }
        else if (ch == 3) { if (currentLang == ENGLISH) currentLang = BULGARIAN; else currentLang = ENGLISH; }
        else if (ch == 4) break;
        else { cout << (currentLang == ENGLISH ? "Invalid option!" : "Невалидна опция!"); cin.get(); }
    }
}

// ===== Math Hangman игра =====
void playGame() {
    chooseMathCategory(); // избира категория
    clearScreen();
    int wrong = 0, totalScore = 0;
    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== MATHEMATICAL HANGMAN ===\n";
    else cout << "=== МАТЕМАТИЧЕСКИ БЕСИЛКА ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG) {
        string exprStr; double answer = generateExpression(exprStr);
        stringstream ss; ss << fixed << setprecision(2) << answer; string answerStr = ss.str();
        string hidden(answerStr.size(), '_');
        if (SHOW_EXPRESSION) cout << (currentLang == ENGLISH ? "Expression: " : "Израз: ") << exprStr << "\n";
        else cout << (currentLang == ENGLISH ? "Expression is hidden!\n" : "Изразът е скрит!\n");
        bool answeredCorrectly = false;
        const double EPS = 1e-4;

        while (!answeredCorrectly && wrong < MAX_WRONG) {
            cout << (currentLang == ENGLISH ? "Remaining attempts: " : "Остават опити: ") << (MAX_WRONG - wrong) << "\n";
            printHangman(wrong);
            string input;
            bool onTime = askInputWithTimer(input, TIMER_PER_TURN);
            if (!onTime) { cout << RED << (currentLang == ENGLISH ? "Time over!\n" : "Времето изтече!\n") << RESET; wrong++; break; }
            input.erase(input.begin(), find_if(input.begin(), input.end(), [](unsigned char ch) { return !isspace(ch); }));
            input.erase(find_if(input.rbegin(), input.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), input.end());
            if (input.empty()) { cout << RED << (currentLang == ENGLISH ? "Invalid input!\n" : "Невалиден вход!\n") << RESET; continue; }
            bool parsedNumber = false; double parsedValue = 0.0;
            try { size_t idx = 0; parsedValue = stod(input, &idx); if (idx == input.size()) parsedNumber = true; }
            catch (...) { parsedNumber = false; }
            if (parsedNumber && fabs(parsedValue - answer) < EPS) {
                hidden = answerStr;
                totalScore += calculateScore(true, wrong, MAX_WRONG, DIFFICULTY, 0);
                answeredCorrectly = true;
                cout << GREEN << (currentLang == ENGLISH ? "Correct! New question coming...\n" : "Правилно! Нова задача идва...\n") << RESET;
                sleepSeconds(1);
                clearScreen();
            }
            else {
                cout << RED << (currentLang == ENGLISH ? "Wrong answer! New question generated.\n" : "Грешен отговор! Генерира се нова задача.\n") << RESET;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }
        }
    }
    clearScreen();
    cout << RED << (currentLang == ENGLISH ? "=== GAME OVER ===\n" : "=== ИГРАТА СВЪРШИ ===\n") << RESET;
    cout << (currentLang == ENGLISH ? "Your total score: " : "Вашият резултат: ") << totalScore << "\n";
    string name;
    do {
        cout << (currentLang == ENGLISH ? "Enter your name for high scores: " : "Въведете името си за резултатите: "); getline(cin, name);
        if (name.empty()) cout << RED << (currentLang == ENGLISH ? "Name cannot be empty!\n" : "Името не може да е празно!\n") << RESET;
    } while (name.empty());
    saveScore(name, totalScore);
    cout << (currentLang == ENGLISH ? "Press Enter..." : "Натиснете Enter...");
    cin.get();
}

// ===== Word Hangman игра =====
void playWordHangman() {
    chooseWordCategory(); // избира категория
    clearScreen();
    vector<string> animals = { "dog","cat","lion","tiger","eagle","shark" };
    vector<string> foods = { "apple","banana","pizza","bread","cheese" };
    vector<string> tech = { "computer","keyboard","mouse","window","program" };
    vector<string> random = { "school","game","music","planet","forest" };
    vector<string> words;

    if (WORD_CATEGORY == 1) words = animals;
    else if (WORD_CATEGORY == 2) words = foods;
    else if (WORD_CATEGORY == 3) words = tech;
    else words = random;

    string word = words[rand() % words.size()];
    string hidden(word.size(), '_');
    int wrong = 0; bool win = false;
    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== WORD HANGMAN ===\n";
    else cout << "=== ДУМЕН HANGMAN ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG && !win) {
        cout << "\n" << (currentLang == ENGLISH ? "Current: " : "Текущо: ") << hidden << "\n";
        cout << (currentLang == ENGLISH ? "Remaining wrong attempts: " : "Остават грешни опити: ") << (MAX_WRONG - wrong) << "\n";
        printHangman(wrong);
        cout << (currentLang == ENGLISH ? "Enter a letter: " : "Въведете буква: ");
        string input; getline(cin, input);
        if (input.size() != 1 || !isalpha(input[0])) { cout << RED << (currentLang == ENGLISH ? "Invalid input!\n" : "Невалиден вход!\n") << RESET; continue; }
        char c = tolower(input[0]);
        bool found = false;
        for (int i = 0; i < (int)word.size(); i++) if (word[i] == c) { hidden[i] = c; found = true; }
        if (!found) { wrong++; cout << RED << (currentLang == ENGLISH ? "Wrong guess!\n" : "Грешна буква!\n") << RESET; }
        if (hidden == word) win = true;
    }

    int score = (win ? (word.size() * 10 + (MAX_WRONG - wrong) * 5) : 0);
    clearScreen();
    if (win) {
        cout << GREEN << (currentLang == ENGLISH ? "=== YOU WIN! ===\nWord: " : "=== ПОБЕДА! ===\nДума: ") << word << "\n";
        cout << (currentLang == ENGLISH ? "Score: " : "Точки: ") << score << RESET;
        string name;
        do {
            cout << "\n" << (currentLang == ENGLISH ? "Enter your name: " : "Въведете името си: "); getline(cin, name);
            if (name.empty()) cout << RED << (currentLang == ENGLISH ? "Name cannot be empty!\n" : "Името не може да е празно!\n") << RESET;
        } while (name.empty());
        saveScore(name, score);
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << (currentLang == ENGLISH ? "=== YOU LOSE ===\nWord: " : "=== ЗАГУБА ===\nДума: ") << word << RESET;
    }
    cout << "\n" << (currentLang == ENGLISH ? "Press Enter..." : "Натиснете Enter...");
    cin.get();
}

// ===== MAIN MENU =====
int main() {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    srand(time(0));
    while (true) {
        clearScreen();
        if (currentLang == ENGLISH) cout << "=== HANGMAN ULTRA ===\n";
        else cout << "=== HANGMAN УЛТРА ===\n";
        if (currentLang == ENGLISH) {
            cout << "1. Math Hangman\n2. Word Hangman\n3. High Scores\n4. Settings\n5. Rules\n6. Exit\nChoice: ";
        }
        else {
            cout << "1. Математически Hangman\n2. Думен Hangman\n3. Резултати\n4. Настройки\n5. Правила\n6. Изход\nИзбор: ";
        }
        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); cout << (currentLang == ENGLISH ? "Invalid input! Please enter a number.\n" : "Невалиден избор! Въведете число.\n"); cin.ignore(); continue; }
        cin.ignore(1000, '\n');
        switch (choice) {
        case 1: playGame(); break;
        case 2: playWordHangman(); break;
        case 3: showHighScores(); break;
        case 4: settingsMenu(); break;
        case 5: showRules(); break;
        case 6: cout << (currentLang == ENGLISH ? "Exiting... Goodbye!\n" : "Изход... Довиждане!\n"); return 0;
        default: cout << (currentLang == ENGLISH ? "Invalid option! Try again.\n" : "Невалидна опция! Опитайте отново.\n");
        }
    }
}
