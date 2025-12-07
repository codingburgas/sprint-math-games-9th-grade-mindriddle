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
    string hangmanPic[7] = {
        " +----+\n"
        " |    |\n"
        " |    \n"
        " |    \n"
        " |    \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |    \n"
        " |    \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |    |\n"
        " |    \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |   /|\n"
        " |    \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |   /|\\\n"
        " |    \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |   /|\\\n"
        " |   / \n"
        " |    \n"
        "==========\n",

        " +----+\n"
        " |    |\n"
        " |    O\n"
        " |   /|\\\n"
        " |   / \\\n"
        " |    \n"
        "==========\n"
    };

    int idx = wrong;
    if (idx < 0) idx = 0;
    if (idx > 6) idx = 6;
    cout << hangmanPic[idx] << "\n";  
}




// ==================== GAME LOGIC ====================
double generateExpression(string& exprStr) {
    int a, b;

    if (DIFFICULTY == 1) {
        a = rand() % 10 + 1;
        b = rand() % 10 + 1;
    }
    else if (DIFFICULTY == 2) {
        a = rand() % 50 + 1;
        b = rand() % 50 + 1;
    }
    else {
        a = rand() % 100 + 1;
        b = rand() % 100 + 1;
    }

    int t = 0;

    if (MATH_CATEGORY == 1) {
        t = rand() % 2;
    }
    else if (MATH_CATEGORY == 2) {
        t = rand() % 4;
    }
    else {
        t = rand() % 6;
    }

    double result = 0.0;
    stringstream ss;

    switch (t) {
    case 0:
        result = a + b;
        ss << a << " + " << b;
        break;
    case 1:
        result = a - b;
        ss << a << " - " << b;
        break;
    case 2:
        result = a * b;
        ss << a << " * " << b;
        break;
    case 3:
        b = (b == 0 ? 1 : b);
        result = double(a) / b;
        ss << a << " / " << b;
        break;
    case 4: {
        int x = rand() % 10 + 1;
        int e = rand() % 4 + 1;
        result = pow(x, e);
        ss << x << " ^ " << e;
        break;
    }
    case 5: {
        int r = rand() % 10 + 1;
        int sq = r * r;
        result = r;
        ss << "sqrt(" << sq << ")";
        break;
    }
    }

    exprStr = ss.str();
    return result;
}

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

int calculateScore(bool win, int wrong, int difficulty) {
    if (!win) return 0;
    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (MAX_WRONG - wrong) * 5;
    return base + diffBonus + livesBonus;
}

// ==================== CATEGORY SELECTION ====================
void chooseMathCategory() {
    clearScreen();
    cout << "=== MATH CATEGORIES ===\n";
    cout << "1. Easy (+ -)\n";
    cout << "2. Medium (+ - * /)\n";
    cout << "3. Hard (+ - * / ^ sqrt)\n";
    cout << "Choose: ";

    int c;
    if (!(cin >> c)) {
        cin.clear();
        cin.ignore(1000, '\n');
        c = 2;
    }
    cin.ignore();

    if (c >= 1 && c <= 3) {
        MATH_CATEGORY = c;
    }
    else {
        MATH_CATEGORY = 2;
    }
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
    if (!(cin >> c)) {
        cin.clear();
        cin.ignore(1000, '\n');
        c = 4;
    }
    cin.ignore();

    if (c >= 1 && c <= 4) {
        WORD_CATEGORY = c;
    }
    else {
        WORD_CATEGORY = 4;
    }
}

// Forward declarations
void showHighScores();
void showRules();

// ==================== MATH HANGMAN ====================
void playMathHangman() {
    chooseMathCategory();
    clearScreen();
    int wrong = 0;
    int totalScore = 0;

    cout << BOLD << GREEN;
    cout << "=== MATHEMATICAL HANGMAN ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG) {
        string exprStr;
        double answer = generateExpression(exprStr);

        if (SHOW_EXPRESSION) {
            cout << "Expression: " << exprStr << "\n";
        }

        bool correct = false;
        const double EPS = 1e-4;

        while (!correct && wrong < MAX_WRONG) {
            cout << "Remaining attempts: " << (MAX_WRONG - wrong) << "\n";
            printHangman(wrong);

            cout << "Enter answer (or 'q' to quit): ";
            string input;
            getline(cin, input);
            input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());

            if (input.empty()) {
                cout << RED << "Invalid input!\n" << RESET;
                continue;
            }

            if (input == "q" || input == "Q") return;

            bool parsed = false;
            double value = 0.0;

            try {
                size_t idx = 0;
                value = stod(input, &idx);
                if (idx == input.size()) parsed = true;
            }
            catch (...) {
                parsed = false;
            }

            if (!parsed) {
                cout << RED << "Invalid input!\n" << RESET;
                continue;
            }

            if (fabs(value - answer) < EPS) {
                totalScore += calculateScore(true, wrong, DIFFICULTY);
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

    cout << "Press Enter...";
    cin.get();
}

// ==================== WORD HANGMAN ====================
void playWordHangman() {
    chooseWordCategory();
    clearScreen();

    vector<string> animals = { "dog","cat","lion","tiger","eagle","shark","bear","wolf","fox","giraffe",
    "zebra","kangaroo","panda","monkey","rabbit","owl","horse","cow","sheep",
    "dolphin","whale","penguin","alligator","crocodile","snake","frog","elephant",
    "hippopotamus","raccoon","squirrel","otter","lobster","crab","turtle","parrot" };
    vector<string> foods = { "apple","banana","pizza","bread","cheese","tomato","carrot","onion","potato",
    "chicken","beef","pork","salad","sandwich","rice","pasta","soup","milk",
    "yogurt","butter","cake","cookie","chocolate","grape","strawberry","orange",
    "lemon","cucumber","pepper","spinach","broccoli","mushroom","pancake","waffle" };
    vector<string> tech = { "computer","keyboard","mouse","window","program","internet","server","laptop",
    "tablet","smartphone","printer","monitor","router","software","hardware",
    "database","algorithm","application","network","password","firewall","email",
    "browser","keyboard","chip","sensor","robot","drone","usb","bluetooth","scanner" };
    vector<string> random = { "school","game","music","planet","forest","river","mountain","city","village",
    "book","movie","friend","family","garden","car","bicycle","train","airport",
    "holiday","beach","storm","flower","bridge","lamp","painting","museum","computer",
    "chair","table","clock","puzzle","camera","key","window" };
    vector<string> words;

    if (WORD_CATEGORY == 1) words = animals;
    else if (WORD_CATEGORY == 2) words = foods;
    else if (WORD_CATEGORY == 3) words = tech;
    else words = random;

    string word = words[rand() % words.size()];
    string hidden(word.size(), '_');
    int wrong = 0;
    bool win = false;
    vector<char> guessed;

    cout << BOLD << GREEN;
    cout << "=== WORD HANGMAN ===\n";
    cout << RESET;

    while (wrong < MAX_WRONG && !win) {
        cout << "\nCurrent: " << hidden << "\n";
        cout << "Remaining wrong attempts: " << (MAX_WRONG - wrong) << "\n";

        printHangman(wrong);

        cout << "Enter a letter (or 'q' to quit): ";
        string in;
        getline(cin, in);

        if (in.empty()) {
            cout << RED << "Invalid input!\n" << RESET;
            continue;
        }

        char c = tolower(in[0]);

        if (c == 'q') return;

        if (!isalpha(c) || in.size() != 1) {
            cout << RED << "Invalid input!\n" << RESET;
            continue;
        }

        if (find(guessed.begin(), guessed.end(), c) != guessed.end()) continue;

        guessed.push_back(c);

        bool found = false;
        for (int i = 0; i < (int)word.size(); i++) {
            if (word[i] == c) {
                hidden[i] = c;
                found = true;
            }
        }

        if (!found) {
            wrong++;
            cout << RED << "Wrong guess!\n" << RESET;
        }

        if (hidden == word) win = true;
    }

    int score = (win ? (word.size() * 10 + (MAX_WRONG - wrong) * 5) : 0);
    clearScreen();

    if (win) {
        cout << GREEN << "=== YOU WIN! ===\n";
        cout << "Word: " << word << "\n";
        cout << "Score: " << score << RESET << "\n";

        int best = getBestScore(HIGHSCORE_WORD);
        if (score > best) {
            cout << "NEW HIGHSCORE!\n";
            string name;
            do {
                cout << "Enter your name: ";
                getline(cin, name);
            } while (name.empty());
            saveScoreIfBest(HIGHSCORE_WORD, name, score);
        }
    }
    else {
        printHangman(MAX_WRONG);
        cout << RED << "=== YOU LOSE ===\n";
        cout << "Word: " << word << RESET << "\n";
    }

    cout << "Press Enter...";
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
    while (fw >> n >> s) wordScores.push_back({ n, s });
    fw.close();

    if (!wordScores.empty()) {
        sort(wordScores.begin(), wordScores.end(), sortScoresDesc);

        if (currentLang == ENGLISH) cout << "--- WORD HANGMAN ---\n";
        else cout << "--- WORD HANGMAN ---\n";

        for (auto& e : wordScores) cout << e.name << " " << e.score << "\n";
    }

    ifstream fm(HIGHSCORE_MATH);
    vector<ScoreEntry> mathScores;
    while (fm >> n >> s) mathScores.push_back({ n, s });
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

    srand((unsigned)time(nullptr));

    if (currentLang == ENGLISH) cout << "\nPress Enter...";
    else cout << "\nНатиснете Enter...";
    cin.get();

    while (true) {
        clearScreen();
        cout << "=== MIND RIDDLE-HANGMAN ===\n";
        cout << "1. Math Hangman\n";
        cout << "2. Word Hangman\n";
        cout << "3. High Scores\n";
        cout << "4. Settings\n";
        cout << "5. Rules\n";
        cout << "6. Exit\n";
        cout << "Choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        cin.ignore(1000, '\n');

        switch (choice) {
        case 1:
            playMathHangman();
            break;
        case 2:
            playWordHangman();
            break;
        case 3:
            showHighScores();
            break;
        case 4:
 
            clearScreen();
            cout << "=== SETTINGS ===\n";
            cout << "1. Toggle show expression (currently " << (SHOW_EXPRESSION ? "ON" : "OFF") << ")\n";
 
            cout << "2. Back\n";
            cout << "Choice: ";
            {
                int sc;
                if (!(cin >> sc)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    break;
                }
                cin.ignore(1000, '\n');
                if (sc == 1) SHOW_EXPRESSION = !SHOW_EXPRESSION;
             
            }
            break;
        case 5:
            showRules();
            break;
        case 6:
            cout << "Exiting... Goodbye!\n";
            return 0;
        default:
            cout << "Invalid option! Try again.\n";
            sleepSeconds(1);
            break;
        }
    }

    return 0;
}

