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

// ==================== SETTINGS ====================
bool SHOW_EXPRESSION = true;
int MAX_WRONG = 6;
int DIFFICULTY = 2;
int TIMER_PER_TURN = 20;
string HIGHSCORE_MATH = "scores_math.txt";
string HIGHSCORE_WORD = "scores_word.txt";

enum Language { ENGLISH, BULGARIAN };
Language currentLang = ENGLISH;

int MATH_CATEGORY = 2;
int WORD_CATEGORY = 4;

// ==================== COLORS ====================
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BOLD = "\033[1m";

// ==================== UTILITIES ====================
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
    string stages[7] = {
        " +----+\n |    |\n |    \n |    \n |    \n |    \n==========\n",
        " +----+\n |    |\n | O  \n |    \n |    \n |    \n==========\n",
        " +----+\n |    |\n | O  \n | |  \n |    \n |    \n==========\n",
        " +----+\n |    |\n | O  \n | /| \n |    \n |    \n==========\n",
        " +----+\n |    |\n | O  \n | /|\\\n |    \n |    \n==========\n",
        " +----+\n |    |\n | O  \n | /|\\\n | /  \n |    \n==========\n",
        " +----+\n |    |\n | O  \n | /|\\\n | / \\\n |    \n==========\n"
    };
    cout << stages[min(wrong, MAX_WRONG)] << "\n";
}

// ==================== GAME LOGIC ====================
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

    switch (t) {
    case 0: result = a + b; ss << a << " + " << b; break;
    case 1: result = a - b; ss << a << " - " << b; break;
    case 2: result = a * b; ss << a << " * " << b; break;
    case 3: b = (b == 0 ? 1 : b); result = double(a) / b; ss << a << " / " << b; break;
    case 4: { int x = rand() % 10 + 1, e = rand() % 4 + 1; result = pow(x, e); ss << x << " ^ " << e; break; }
    case 5: { int r = rand() % 10 + 1, sq = r * r; result = r; ss << "sqrt(" << sq << ")"; break; }
    }

    exprStr = ss.str();
    return result;
}

int getBestScore(const string& filename) {
    ifstream f(filename);
    int best = 0, s;
    string name;
    while (f >> name >> s) if (s > best) best = s;
    return best;
}

void saveScoreIfBest(const string& filename, const string& name, int score) {
    int best = getBestScore(filename);
    if (score > best) {
        ofstream f(filename, ios::app);
        f << name << " " << score << "\n";
    }
}

bool askInputWithTimer(string& input, int seconds) {
    auto start = chrono::steady_clock::now();
    if (currentLang == ENGLISH) cout << "You have " << seconds << " seconds. Input: ";
    else cout << "Имате " << seconds << " секунди. Въведете: ";
    getline(cin, input);
    auto end = chrono::steady_clock::now();
    long duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    return duration <= seconds;
}

int calculateScore(bool win, int wrong, int difficulty, double timeTaken) {
    if (!win) return 0;
    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (MAX_WRONG - wrong) * 5;
    int timeBonus = max(0, int(max(0.0, 20.0 - timeTaken)) * 2);
    return base + diffBonus + livesBonus + timeBonus;
}

// ==================== CATEGORY SELECTION ====================
void chooseMathCategory() {
    clearScreen();
    if (currentLang == ENGLISH) {
        cout << "=== MATH CATEGORIES ===\n1. Easy (+ -)\n2. Medium (+ - * /)\n3. Hard (+ - * / ^ sqrt)\nChoose: ";
    }
    else {
        cout << "=== КАТЕГОРИИ МАТЕМАТИКА ===\n1. Лесно (+ -)\n2. Средно (+ - * /)\n3. Трудно (+ - * / ^ sqrt)\nИзбор: ";
    }
    int c; cin >> c; cin.ignore();
    if (c >= 1 && c <= 3) MATH_CATEGORY = c;
    else MATH_CATEGORY = 2;
}

void chooseWordCategory() {
    clearScreen();
    if (currentLang == ENGLISH) {
        cout << "=== WORD CATEGORIES ===\n1. Animals\n2. Foods\n3. Technology\n4. Random\nChoose: ";
    }
    else {
        cout << "=== КАТЕГОРИИ ДУМИ ===\n1. Животни\n2. Храни\n3. Технологии\n4. Разни\nИзбор: ";
    }
    int c; cin >> c; cin.ignore();
    if (c >= 1 && c <= 4) WORD_CATEGORY = c;
    else WORD_CATEGORY = 4;
}

// ==================== MATH HANGMAN ====================
void playMathHangman() {
    chooseMathCategory();
    clearScreen();
    int wrong = 0, totalScore = 0;
    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== MATHEMATICAL HANGMAN ===\n";
    else cout << "=== МАТЕМАТИЧЕСКА БЕСИЛКА ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG) {
        string exprStr;
        double answer = generateExpression(exprStr);
        stringstream ss; ss << fixed << setprecision(2) << answer;
        string answerStr = ss.str();

        if (SHOW_EXPRESSION) {
            if (currentLang == ENGLISH) cout << "Expression: " << exprStr << "\n";
            else cout << "Израз: " << exprStr << "\n";
        }
        else {
            if (currentLang == ENGLISH) cout << "Expression is hidden!\n";
            else cout << "Изразът е скрит!\n";
        }

        bool correct = false;
        const double EPS = 1e-4;
        while (!correct && wrong < MAX_WRONG) {
            if (currentLang == ENGLISH) cout << "Remaining attempts: " << (MAX_WRONG - wrong) << "\n";
            else cout << "Остават опити: " << (MAX_WRONG - wrong) << "\n";

            printHangman(wrong);

            string input;
            bool onTime = askInputWithTimer(input, TIMER_PER_TURN);
            if (!onTime) {
                if (currentLang == ENGLISH) cout << RED << "Time over!\n" << RESET;
                else cout << RED << "Времето изтече!\n" << RESET;
                wrong++; break;
            }

            input.erase(input.begin(), find_if(input.begin(), input.end(), [](unsigned char ch) {return !isspace(ch);}));
            input.erase(find_if(input.rbegin(), input.rend(), [](unsigned char ch) {return !isspace(ch);}).base(), input.end());
            if (input.empty()) {
                if (currentLang == ENGLISH) cout << RED << "Invalid input!\n" << RESET;
                else cout << RED << "Невалиден вход!\n" << RESET;
                continue;
            }

            bool parsed = false;
            double value = 0.0;
            try { size_t idx = 0; value = stod(input, &idx); if (idx == input.size()) parsed = true; }
            catch (...) { parsed = false; }

            if (parsed && fabs(value - answer) < EPS) {
                totalScore += calculateScore(true, wrong, DIFFICULTY, 0);
                if (currentLang == ENGLISH) cout << GREEN << "Correct!\n" << RESET;
                else cout << GREEN << "Правилно!\n" << RESET;
                sleepSeconds(1);
                clearScreen();
                correct = true;
            }
            else {
                if (currentLang == ENGLISH) cout << RED << "Wrong!\n" << RESET;
                else cout << RED << "Грешен отговор!\n" << RESET;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }
        }
    }

    clearScreen();
    if (currentLang == ENGLISH) cout << RED << "=== GAME OVER ===\n" << RESET;
    else cout << RED << "=== КРАЙ НА ИГРАТА ===\n" << RESET;

    if (currentLang == ENGLISH) cout << "Your total score: " << totalScore << "\n";
    else cout << "Вашият резултат: " << totalScore << "\n";

    int best = getBestScore(HIGHSCORE_MATH);
    if (totalScore > best) {
        if (currentLang == ENGLISH) cout << "NEW HIGHSCORE!\n";
        else cout << "НОВ РЕКОРД!\n";
        string name; do {
            if (currentLang == ENGLISH) cout << "Enter your name: ";
            else cout << "Въведете името си: ";
            getline(cin, name);
        } while (name.empty());
        saveScoreIfBest(HIGHSCORE_MATH, name, totalScore);
    }
    else {
        if (currentLang == ENGLISH) cout << "Score not high enough. Name not saved.\n";
        else cout << "Резултатът не е достатъчно висок. Името не е записано.\n";
    }

    if (currentLang == ENGLISH) cout << "Press Enter...";
    else cout << "Натиснете Enter...";
    cin.get();
}

// ==================== WORD HANGMAN ====================
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
    int wrong = 0; bool win = false;
    vector<char> guessed;

    cout << BOLD << GREEN;
    if (currentLang == ENGLISH) cout << "=== WORD HANGMAN ===\n";
    else cout << "=== ДУМИ БЕСИЛКА ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG && !win) {
        if (currentLang == ENGLISH) cout << "\nCurrent: " << hidden << "\nRemaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";
        else cout << "\nТекущо: " << hidden << "\nОстават грешни опити: " << (MAX_WRONG - wrong) << "\n";

        printHangman(wrong);
        if (currentLang == ENGLISH) cout << "Enter a letter: ";
        else cout << "Въведете буква: ";

        string in; getline(cin, in);
        if (in.size() != 1 || !isalpha(in[0])) {
            if (currentLang == ENGLISH) cout << RED << "Invalid input!\n" << RESET;
            else cout << RED << "Невалиден вход!\n" << RESET;
            continue;
        }

        char c = tolower(in[0]);
        if (find(guessed.begin(), guessed.end(), c) != guessed.end()) continue;
        guessed.push_back(c);

        bool found = false;
        for (int i = 0; i < (int)word.size(); i++) if (word[i] == c) { hidden[i] = c; found = true; }
        if (!found) { wrong++; if (currentLang == ENGLISH) cout << RED << "Wrong guess!\n" << RESET; else cout << RED << "Грешна буква!\n" << RESET; }

        if (hidden == word) win = true;
    }

    int score = (win ? (word.size() * 10 + (MAX_WRONG - wrong) * 5) : 0);
    clearScreen();

    if (win) {
        if (currentLang == ENGLISH) cout << GREEN << "=== YOU WIN! ===\nWord: " << word << "\nScore: " << score << RESET;
        else cout << GREEN << "=== ПОБЕДА! ===\nДума: " << word << "\nТочки: " << score << RESET;

        int best = getBestScore(HIGHSCORE_WORD);
        if (score > best) {
            if (currentLang == ENGLISH) cout << "\nNEW HIGHSCORE!\n";
            else cout << "\nНОВ РЕКОРД!\n";

            string name; do {
                if (currentLang == ENGLISH) cout << "Enter your name: ";
                else cout << "Въведете името си: ";
                getline(cin, name);
            } while (name.empty());
            saveScoreIfBest(HIGHSCORE_WORD, name, score);
        }
    }
    else {
        printHangman(MAX_WRONG);
        if (currentLang == ENGLISH) cout << RED << "=== YOU LOSE ===\nWord: " << word << RESET;
        else cout << RED << "=== ЗАГУБА ===\nДума: " << word << RESET;
    }

    if (currentLang == ENGLISH) cout << "\nPress Enter...";
    else cout << "\nНатиснете Enter...";
    cin.get();
}

// ==================== HIGH SCORES ====================
struct ScoreEntry { string name; int score; };
bool sortScoresDesc(const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; }

void showHighScores() {
    clearScreen();
    if (currentLang == ENGLISH) cout << "======== HIGH SCORES ========\n\n";
    else cout << "======== РЕЗУЛТАТИ ========\n\n";

    ifstream fw(HIGHSCORE_WORD);
    vector<ScoreEntry> wordScores;
    string n; int s;
    while (fw >> n >> s) wordScores.push_back({ n,s });
    fw.close();
    if (!wordScores.empty()) {
        sort(wordScores.begin(), wordScores.end(), sortScoresDesc);
        if (currentLang == ENGLISH) cout << "--- WORD HANGMAN ---\n";
        else cout << "--- WORD HANGMAN ---\n";
        for (auto& e : wordScores) cout << e.name << " " << e.score << "\n";
    }

    ifstream fm(HIGHSCORE_MATH);
    vector<ScoreEntry> mathScores;
    while (fm >> n >> s) mathScores.push_back({ n,s });
    fm.close();
    if (!mathScores.empty()) {
        sort(mathScores.begin(), mathScores.end(), sortScoresDesc);
        if (currentLang == ENGLISH) cout << "\n--- MATH HANGMAN ---\n";
        else cout << "\n--- MATH HANGMAN ---\n";
        for (auto& e : mathScores) cout << e.name << " " << e.score << "\n";
    }

    if (currentLang == ENGLISH) cout << "\nPress Enter...";
    else cout << "\nНатиснете Enter...";
    cin.get();
}

// ==================== RULES ====================
void showRules() {
    clearScreen();
    if (currentLang == ENGLISH) {
        cout << "=========== RULES ===========\n\n";
        cout << "Math Hangman: Solve the math expression before time runs out.\n";
        cout << "Word Hangman: Guess letters to reveal the word.\n";
        cout << "Max wrong attempts: " << MAX_WRONG << "\n";
        cout << "Press Enter to return...";
    }
    else {
        cout << "=========== ПРАВИЛА ===========\n\n";
        cout << "Math Hangman: Решете израза преди да изтече времето.\n";
        cout << "Word Hangman: Познайте буквите, за да разкриете думата.\n";
        cout << "Макс. грешни опити: " << MAX_WRONG << "\n";
        cout << "Натиснете Enter за връщане...";
    }
    cin.get();
}

// ==================== MAIN ====================
int main() {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand(time(0));
    while (true) {
        clearScreen();
        cout << "=== MIND RIDDLE-HANGMAN ===\n";
        if (currentLang == ENGLISH) cout << "1. Math Hangman\n2. Word Hangman\n3. High Scores\n4. Settings\n5. Rules\n6. Exit\nChoice: ";
        else cout << "1. Math бесилка\n2. Word hangman\n3. Резултати\n4. Настройки\n5. Правила\n6. Изход\nИзбор: ";

        int choice; if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: playMathHangman(); break;
        case 2: playWordHangman(); break;
        case 3: showHighScores(); break;
        case 4: settingsMenu(); break;
        case 5: showRules(); break;
        case 6: if (currentLang == ENGLISH) cout << "Exiting... Goodbye!\n"; else cout << "Изход... Довиждане!\n"; return 0;
        default: if (currentLang == ENGLISH) cout << "Invalid option! Try again.\n"; else cout << "Невалидна опция! Опитайте отново.\n"; break;
        }
    }
}
