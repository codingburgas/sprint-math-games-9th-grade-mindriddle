#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

void printHangman(int wrong) {
    cout << "\n";
    cout << "   +----+\n";
    cout << "   |    |\n";
    cout << (wrong >= 1 ? "   O    |\n" : "        |\n");

    if (wrong == 2) cout << "   |    |\n";
    else if (wrong == 3) cout << "  /|    |\n";
    else if (wrong >= 4) cout << "  /|\\   |\n";
    else cout << "        |\n";

    if (wrong == 5) cout << "  /     |\n";
    else if (wrong >= 6) cout << "  / \\   |\n";
    else cout << "        |\n";

    cout << "        |\n";
    cout << "  ==========\n\n";
}

double generateExpression(string& exprStr) {
    int a = rand() % 10 + 1;
    int b = rand() % 10 + 1;
    int t = rand() % 4;
    double result;

    if (t == 0) { exprStr = to_string(a) + " + " + to_string(b); result = a + b; }
    else if (t == 1) { exprStr = to_string(a) + " - " + to_string(b); result = a - b; }
    else if (t == 2) { exprStr = to_string(a) + " * " + to_string(b); result = a * b; }
    else {
        if (b == 0) b = 1;
        a = a * b;
        exprStr = to_string(a) + " / " + to_string(b);
        result = (double)a / b;
    }
    return result;
}

int main() {
    srand(time(0));

    string exprStr;
    double ans = generateExpression(exprStr);
    string ansStr = to_string((int)round(ans));

    string hidden(ansStr.size(), '_');
    int wrong = 0;

    cout << "Expression: " << exprStr << "\n";

    while (wrong < 7) {
        cout << "Current: " << hidden << "\n";
        printHangman(wrong);
        cout << "Guess: ";

        string input;
       cin>> input;

        if (input == ansStr) {
            cout << "You win!\n";
            return 0;
        }

        if (input.size() == 1) {
            char c = input[0];
            bool found = false;
            for (int i = 0; i < ansStr.size(); i++) {
                if (ansStr[i] == c) {
                    hidden[i] = c;
                    found = true;
                }
            }
            if (!found) wrong++;
        }
        else {
            wrong++;
        }

        if (hidden == ansStr) {
            cout << "You win!\n";
            return 0;
        }
    }

    printHangman(7);
    cout << "You lose! Answer was: " << ansStr << "\n";
}
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;


int DIFFICULTY = 2;
int MAX_WRONG = 8;
int TIMER_PER_TURN = 20;
bool SHOW_EXPRESSION = true;

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

void printHangman(int wrong) {
    cout << "\n   +----+\n";
    cout << "   |    |\n";
    cout << (wrong >= 1 ? "   O    |\n" : "        |\n");
    if (wrong == 2) cout << "   |    |\n";
    else if (wrong == 3) cout << "  /|    |\n";
    else if (wrong >= 4) cout << "  /|\\   |\n";
    else cout << "        |\n";
    if (wrong == 5) cout << "  /     |\n";
    else if (wrong >= 6) cout << "  / \\   |\n";
    else cout << "        |\n";
    cout << "        |\n";
    cout << "  ==========\n\n";
}

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
        a = a * b;
        result = static_cast<double>(a) / b;
        ss << a << " / " << b;
    }
    else if (t == 4) {
        if (b > 10) b = b % 10 + 1;
        result = pow(a, b);
        ss << a << " ^ " << b;
    }
    else {
        a = rand() % 100 + 1;
        result = sqrt(static_cast<double>(a));
        ss << "sqrt(" << a << ")";
    }

    exprStr = ss.str();
    return result;
}

bool askInputWithTimer(string& input, int seconds) {
    auto start = chrono::steady_clock::now();
    cout << "You have " << seconds << " seconds. Input: ";
    getline(cin, input);
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    return duration <= seconds;
}

int calculateScore(bool win, int wrong, int maxWrong, int difficulty, double timeTaken) {
    if (!win) return 0;
    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (maxWrong - wrong) * 5;
    int timeBonus = max(0, static_cast<int>(max(0.0, 20.0 - timeTaken)) * 2);
    return base + diffBonus + livesBonus + timeBonus;
}

void playSingleGame() {
    clearScreen();
    string expr;
    double answer = generateExpression(expr);

    string answerStr;
    double intpart;
    if (modf(answer, &intpart) == 0.0) {
        stringstream ss; ss << static_cast<long long>(llround(answer)); ss >> answerStr;
    }
    else {
        stringstream ss; ss << fixed << setprecision(2) << answer; ss >> answerStr;
    }

    string hidden(answerStr.size(), '_');
    int wrong = 0;
    bool win = false;
    auto startTime = chrono::steady_clock::now();

    cout << "=== MATHEMATICAL HANGMAN (V2) ===\n\n";
    if (SHOW_EXPRESSION) cout << "Expression: " << expr << "\n";
    else cout << "Expression is hidden!\n";

    while (wrong < MAX_WRONG && !win) {
        cout << "\nCurrent: " << hidden << "\n";
        cout << "Remaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";
        printHangman(wrong);

        string input;
        bool onTime = askInputWithTimer(input, TIMER_PER_TURN);
        if (!onTime) { cout << "Time over!\n"; wrong++; continue; }

        if (input == answerStr) { win = true; break; }

        if (input.size() == 1) {
            char c = input[0];
            if (!((c >= '0' && c <= '9') || c == '-' || c == '.')) {
                cout << "Invalid input!\n"; continue;
            }
            bool found = false;
            for (size_t i = 0; i < answerStr.size(); ++i) {
                if (answerStr[i] == c) { hidden[i] = c; found = true; }
            }
            if (!found) { wrong++; cout << "Wrong guess!\n"; }
            if (hidden == answerStr) win = true;
        }
        else {
            wrong++; cout << "Wrong guess!\n";
        }
    }

    auto endTime = chrono::steady_clock::now();
    auto timeTaken = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
    int score = calculateScore(win, wrong, MAX_WRONG, DIFFICULTY, static_cast<double>(timeTaken));

    clearScreen();
    if (win) {
        cout << "=== YOU WIN! ===\nAnswer: " << answerStr << "\nScore: " << score << "\n";
    }
    else {
        printHangman(MAX_WRONG);
        cout << "=== YOU LOSE ===\nAnswer: " << answerStr << "\n";
    }
    cout << "\nPress Enter to continue..."; cin.get();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    while (true) {
        playSingleGame();
        cout << "\nPlay again? (y/n): ";
        string r; getline(cin, r);
        if (r != "y" && r != "Y") break;
    }
    return 0;
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

    using namespace std;

    // ===================== ГЛОБАЛНИ ПАРАМЕТРИ ======================
    int MAX_WRONG = 10;
    int DIFFICULTY = 2;
    string HIGHSCORE_FILE = "scores.txt";

    // ===================== ASCII HANGMAN ==========================
    void printHangman(int wrong) {
        cout << "\n";
        cout << "   +----+\n";
        cout << "   |    |\n";
        cout << (wrong >= 1 ? "   O    |\n" : "        |\n");

        if (wrong == 2) cout << "   |    |\n";
        else if (wrong == 3) cout << "  /|    |\n";
        else if (wrong >= 4) cout << "  /|\\   |\n";
        else cout << "        |\n";

        if (wrong == 5) cout << "  /     |\n";
        else if (wrong >= 6) cout << "  / \\   |\n";
        else cout << "        |\n";

        cout << "        |\n";
        cout << "  ==========\n\n";
    }

    // ===================== ГЕНЕРАТОР НА ИЗРАЗ ===================
    double generateExpression(string & exprStr) {
        int a = rand() % 20 + 1;
        int b = rand() % 20 + 1;
        int op = rand() % 4; // + - * /

        double result = 0;
        stringstream ss;

        switch (op) {
        case 0: result = a + b; ss << a << " + " << b; break;
        case 1: result = a - b; ss << a << " - " << b; break;
        case 2: result = a * b; ss << a << " * " << b; break;
        case 3:
            if (b == 0) b = 1;
            result = static_cast<double>(a) / b;
            ss << a << " / " << b; break;
        }

        exprStr = ss.str();
        return result;
    }

    // ===================== HIGH SCORES ===========================
    void saveScore(const string & name, int score) {
        ofstream f(HIGHSCORE_FILE, ios::app);
        if (f.is_open()) {
            f << name << " " << score << "\n";
            f.close();
        }
    }

    void showHighScores() {
        cout << "====== TOP SCORES ======\n\n";
        ifstream f(HIGHSCORE_FILE);
        string line;
        bool empty = true;
        while (getline(f, line)) {
            if (!line.empty()) { cout << line << "\n"; empty = false; }
        }
        if (empty) cout << "No scores yet.\n";
        cout << "\nPress Enter...";
        cin.ignore();
        cin.get();
    }

    // ===================== GAME ================================
    void playGame() {
        string exprStr;
        double answer = generateExpression(exprStr);

        string answerStr;
        double intpart;
        if (modf(answer, &intpart) == 0.0) {
            stringstream ss; ss << static_cast<long long>(answer); ss >> answerStr;
        }
        else {
            stringstream ss; ss << fixed << setprecision(2) << answer; ss >> answerStr;
        }

        string hidden(answerStr.size(), '_');
        int wrong = 0;
        bool win = false;

        cout << "=== MATHEMATICAL HANGMAN ===\n";
        cout << "Expression: " << exprStr << "\n";

        while (wrong < MAX_WRONG && !win) {
            cout << "\nCurrent: " << hidden << "\n";
            cout << "Remaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";
            printHangman(wrong);

            string input;
            cout << "Your guess: ";
            getline(cin, input);

            if (input == answerStr) { win = true; break; }

            if (input.size() == 1) {
                char c = input[0];
                bool found = false;
                for (size_t i = 0; i < answerStr.size(); i++) {
                    if (answerStr[i] == c) { hidden[i] = c; found = true; }
                }
                if (!found) { wrong++; cout << "Wrong guess!\n"; }
                if (hidden == answerStr) win = true;
            }
            else {
                wrong++;
                cout << "Wrong guess!\n";
            }
        }

        if (win) {
            cout << "=== YOU WIN! ===\nAnswer: " << answerStr << "\n";
            cout << "Enter your name for High Scores: ";
            string name;
            getline(cin, name);
            if (name.empty()) name = "Anonymous";
            saveScore(name, 100);
        }
        else {
            printHangman(MAX_WRONG);
            cout << "=== YOU LOSE ===\nAnswer: " << answerStr << "\n";
        }

        cout << "\nPress Enter...";
        cin.get();
    }

    // ===================== MAIN MENU =============================
    int main() {
        srand(static_cast<unsigned int>(time(0)));

        while (true) {
            cout << "\n=== MATHEMATICAL HANGMAN ===\n";
            cout << "1. New Game\n2. High Scores\n3. Rules\n4. Settings\n5. Exit\nChoice: ";

            int ch;
            if (!(cin >> ch)) {
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(10000, '\n');

            if (ch == 1) playGame();
            else if (ch == 2) showHighScores();
            else if (ch == 5) break;
        }

        return 0;
    }