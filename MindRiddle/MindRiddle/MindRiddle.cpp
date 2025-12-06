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

string HIGHSCORE_MATH = "scores_math.txt";
string HIGHSCORE_WORD = "scores_word.txt";
string SETTINGS_FILE = "settings.txt";

enum Language { ENGLISH, BULGARIAN };
Language currentLang = ENGLISH;

int MATH_CATEGORY = 2;
int WORD_CATEGORY = 4;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";


int getBestScore(const string& filename) {
    ifstream f(filename);
    int best = 0, s;
    string name;
    while (f >> name >> s) {
        if (s > best) best = s;
    }
    return best;
}

void saveScoreIfBest(const string& filename, const string& name, int score) {
    int best = getBestScore(filename);
    if (score > best) {
        ofstream f(filename, ios::app);
        f << name << " " << score << "\n";
    }
}

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

struct ScoreEntry {
    string name;
    int score;
};

bool sortScoresDesc(const ScoreEntry& a, const ScoreEntry& b) {
    return a.score > b.score; 
}

void showHighScores() {
    clearScreen();
    cout << "======== HIGH SCORES ========\n\n";

    cout << "--- WORD HANGMAN ---\n";
    ifstream fw(HIGHSCORE_WORD);
    if (!fw.is_open()) cout << "No scores yet.\n";
    else {
        string n;
        int s;
        vector<ScoreEntry> wordScores;

        while (fw >> n >> s) {
            wordScores.push_back({ n, s });
        }
        fw.close();

        if (wordScores.empty()) {
            cout << "No scores yet.\n";
        }
        else {
            sort(wordScores.begin(), wordScores.end(), sortScoresDesc);
            for (auto& e : wordScores)
                cout << e.name << " " << e.score << "\n";
        }
    }

    cout << "\n--- MATH HANGMAN ---\n";
    ifstream fm(HIGHSCORE_MATH);
    if (!fm.is_open()) cout << "No scores yet.\n";
    else {
        string n;
        int s;
        vector<ScoreEntry> mathScores;

        while (fm >> n >> s) {
            mathScores.push_back({ n, s });
        }
        fm.close();

        if (mathScores.empty()) {
            cout << "No scores yet.\n";
        }
        else {
            sort(mathScores.begin(), mathScores.end(), sortScoresDesc);
            for (auto& e : mathScores)
                cout << e.name << " " << e.score << "\n";
        }
    }

    cout << "\nPress Enter...";
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

void playGame() {
    chooseMathCategory();
    clearScreen();
    int wrong = 0, totalScore = 0;
    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== MATHEMATICAL HANGMAN ===\n";
    else cout << "=== МАТЕМАТИЧЕСКИ БЕСИЛКА ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG) {
        string exprStr;
        double answer = generateExpression(exprStr);

        stringstream ss;
        ss << fixed << setprecision(2) << answer;
        string answerStr = ss.str();

        if (SHOW_EXPRESSION)
            cout << (currentLang == ENGLISH ? "Expression: " : "Израз: ") << exprStr << "\n";
        else
            cout << (currentLang == ENGLISH ? "Expression is hidden!\n" : "Изразът е скрит!\n");
        bool correct = false;
        const double EPS = 1e-4;

        while (!correct && wrong < MAX_WRONG) {
            cout << (currentLang == ENGLISH ? "Remaining attempts: " : "Остават опити: ") << (MAX_WRONG - wrong) << "\n";
            printHangman(wrong);
            string input;
            bool onTime = askInputWithTimer(input, TIMER_PER_TURN);
            if (!onTime) {
                cout << RED << (currentLang == ENGLISH ? "Time over!\n" : "Времето изтече!\n") << RESET;
                wrong++;
                break;
            }
            input.erase(input.begin(), find_if(input.begin(), input.end(), [](unsigned char ch) { return !isspace(ch); }));
            input.erase(find_if(input.rbegin(), input.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), input.end());
            if (input.empty()) continue;
            bool parsed = false;
            double value = 0.0;
            try {
                size_t idx = 0;
                value = stod(input, &idx);
                if (idx == input.size()) parsed = true;
            }
            catch (...) { parsed = false; }
            if (parsed && fabs(value - answer) < EPS) {
                totalScore += calculateScore(true, wrong, MAX_WRONG, DIFFICULTY, 0);
                cout << GREEN << "Correct!\n" << RESET;
                sleepSeconds(1);
                clearScreen();
                correct = true;
            }
            else {
                cout << RED << "Wrong!\n" << RESET;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }
        }
    }

    clearScreen();
    cout << RED << "=== GAME OVER ===\n" << RESET;
    cout << "Your total score: " << totalScore << "\n";

    int best = getBestScore(HIGHSCORE_MATH);

    if (totalScore > best) {
        cout << "NEW HIGHSCORE!\n";
        string name;
        do {
            cout << "Enter your name: ";
            getline(cin, name);
        } while (name.empty());

        saveScoreIfBest(HIGHSCORE_MATH, name, totalScore);
    }
    else {
        cout << "Score not high enough. Name not saved.\n";
    }

    cout << "Press Enter...";
    cin.get();
}

void playWordHangman() {
    chooseWordCategory();
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

    int wrong = 0;
    bool win = false;

    cout << BOLD << GREEN;
    cout << "=== WORD HANGMAN ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG && !win) {
        cout << "\nCurrent: " << hidden << "\n";
        cout << "Remaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";
        printHangman(wrong);

        cout << "Enter a letter: ";
        string in;
        getline(cin, in);
        if (in.size() != 1 || !isalpha(in[0])) continue;

        char c = tolower(in[0]);
        bool found = false;

        for (int i = 0; i < word.size(); i++)
            if (word[i] == c) { hidden[i] = c; found = true; }

        if (!found) wrong++;
        if (hidden == word) win = true;
    }

    int score = win ? (word.size() * 10 + (MAX_WRONG - wrong) * 5) : 0;

    clearScreen();

    if (win) {
        cout << GREEN << "=== YOU WIN! ===\nWord: " << word << "\nScore: " << score << RESET;

        int best = getBestScore(HIGHSCORE_WORD);

        if (score > best) {
            cout << "\nNEW HIGHSCORE!\n";
            string name;
            do {
                cout << "Enter your name: ";
                getline(cin, name);
            } while (name.empty());

            saveScoreIfBest(HIGHSCORE_WORD, name, score);
        }
        else {
            cout << "\nScore not high enough. Name not saved.\n";
        }
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << "=== YOU LOSE ===\nWord: " << word << RESET;
    }

    cout << "\nPress Enter...";
    cin.get();
}

int main() {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand(time(0));

    while (true) {
        clearScreen();
        cout << "=== HANGMAN ULTRA ===\n";
        cout << "1. Math Hangman\n2. Word Hangman\n3. High Scores\n4. Settings\n5. Rules\n6. Exit\nChoice: ";

        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: playGame(); break;
        case 2: playWordHangman(); break;
        case 3: showHighScores(); break;
        case 4: settingsMenu(); break;
        case 5: cout << "Rules placeholder...\n"; cin.get(); break;
        case 6: return 0;
        default: break;
        }
    }
}
