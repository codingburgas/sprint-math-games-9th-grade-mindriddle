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
string LANGUAGE = "BG";
string THEME = "LIGHT";
int TIMER_PER_TURN = 20;

string HIGHSCORE_FILE = "scores.txt";
string SETTINGS_FILE = "settings.txt";

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
    cout << BOLD << CYAN << "====== TOP SCORES ======\n\n" << RESET;

    ifstream f(HIGHSCORE_FILE);
    string line;
    bool empty = true;

    while (getline(f, line)) {
        if (!line.empty()) {
            cout << line << "\n";
            empty = false;
        }
    }

    if (empty) cout << "No scores yet.\n";

    cout << "\nPress Enter...";
    cin.ignore();
    cin.get();
}

//===================== RULES ================================
void showRules() {
    clearScreen();
    cout << BOLD << CYAN << "====== GAME RULES ======\n\n" << RESET;

    cout << "Mind Riddle is a math game where you answer questions to earn points.\n";
    cout << "Each correct answer gives you points, but wrong answers cost points.\n";
    cout << "If you lose too many points or run out of time, the game ends.\n";
    cout << "Try to score as high as possible!\n\n";

    cout << "Press Enter...";
    cin.ignore();
    cin.get();
}

// ===================== ТАЙМЕР ================================
bool askInputWithTimer(string& input, int seconds) {
    auto start = chrono::steady_clock::now();

    cout << "You have " << seconds << " seconds. Input: ";
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

        cout << BOLD << CYAN << "====== SETTINGS ======\n" << RESET;
        cout << "1. Difficulty (current " << DIFFICULTY << ")\n";
        cout << "2. Max wrong attempts (current " << MAX_WRONG << ")\n";
        cout << "3. Show expression (current " << SHOW_EXPRESSION << ")\n";
        cout << "4. Timer per turn (current " << TIMER_PER_TURN << "s)\n";
        cout << "5. Theme (current " << THEME << ")\n";
        cout << "6. Language (current " << LANGUAGE << ")\n";
        cout << "7. Back\n";
        cout << "Choice: ";

        int ch;
        cin >> ch;
        cin.ignore();

        if (ch == 1) {
            cout << "1=Easy  2=Medium  3=Hard: ";
            cin >> DIFFICULTY; cin.ignore();
        }
        else if (ch == 2) {
            cout << "Enter max wrong attempts: ";
            cin >> MAX_WRONG; cin.ignore();
        }
        else if (ch == 3) {
            int x;
            cout << "Show expression? 1=Yes 0=No: ";
            cin >> x; cin.ignore();
            SHOW_EXPRESSION = (x == 1);
        }
        else if (ch == 4) {
            cout << "Enter seconds per turn: ";
            cin >> TIMER_PER_TURN; cin.ignore();
        }
        else if (ch == 5) {
            cout << "Theme LIGHT/DARK: ";
            getline(cin, THEME);
        }
        else if (ch == 6) {
            cout << "Language BG/EN: ";
            getline(cin, LANGUAGE);
        }
        else if (ch == 7) break;
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

    cout << BOLD << GREEN << "=== MATHEMATICAL HANGMAN ===\n" << RESET;

    if (SHOW_EXPRESSION) cout << "Expression: " << exprStr << "\n";
    else cout << "Expression is hidden!\n";

    while (wrong < MAX_WRONG && !win) {
        cout << "\nCurrent: " << hidden << "\n";
        cout << "Remaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";

        printHangman(wrong);

        string input;
        bool onTime = askInputWithTimer(input, TIMER_PER_TURN);

        if (!onTime) {
            cout << RED << "Time over!\n" << RESET;
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
                cout << RED << "Invalid character!\n" << RESET;
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
                cout << RED << "Wrong guess!\n" << RESET;
            }

            if (hidden == answerStr) win = true;
        }
        else {
            cout << RED << "Invalid input!\n" << RESET;
        }
    }

    auto endTime = chrono::steady_clock::now();
    double timeTaken = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();

    int score = calculateScore(win, wrong, MAX_WRONG, DIFFICULTY, timeTaken);

    clearScreen();

    if (win) {
        cout << GREEN << "=== YOU WIN! ===\nAnswer: " << answerStr << "\nScore: " << score << RESET;

        cout << "\nEnter your name: ";
        string name;
        getline(cin, name);

        saveScore(name, score);
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << "=== YOU LOSE ===\nAnswer: " << answerStr << RESET;
    }

    cout << "\nPress Enter...";
    cin.get();
}

// ===================== MAIN ================================
int main() {
    srand(time(0));

    while (true) {
        clearScreen();

        cout << BOLD << CYAN << "=== MATHEMATICAL HANGMAN ULTRA ===\n" << RESET;
        cout << "1. New Game\n";
        cout << "2. High Scores\n";
        cout << "3. Settings\n";
        cout << "4. Rules\n";
        cout << "5. Exit\n";
        cout << "Choice: ";

        int ch;
        cin >> ch;
        cin.ignore();

        if (ch == 1) playGame();
        else if (ch == 2) showHighScores();
        else if (ch == 3) settingsMenu();
        else if (ch == 4) showRules();
        else if (ch == 5) break;
    }

    return 0;
}
