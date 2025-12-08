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
int TIME_LIMIT = 20;
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

void showHighScores();
void showRules();

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

            cout << "Enter answer (or 'quit' to quit): ";

            auto start = chrono::steady_clock::now();
            string input;
            getline(cin, input);
            auto end = chrono::steady_clock::now();
            int elapsed = chrono::duration_cast<chrono::seconds>(end - start).count();

            if (elapsed > TIME_LIMIT) {
                cout << RED << "Time's up!\n" << RESET;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }

            input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());

            if (input.empty()) {
                cout << RED << "Invalid input!\n" << RESET;
                continue;
            }

            if (input == "quit" || input == "QUIT" || input == "Quit") return;

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

        cout << "Enter a letter (or 'quit' to quit): ";
        string in;
        getline(cin, in);

        if (in.empty()) {
            cout << RED << "Invalid input!\n" << RESET;
            continue;
        }

        if (in == "quit" || in == "QUIT" || in == "Quit") return;

        char c = tolower(in[0]);



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

    cout << "======== HIGH SCORES ========\n\n";

    ifstream fw(HIGHSCORE_WORD);
    vector<ScoreEntry> wordScores;
    string n; int s;
    while (fw >> n >> s) wordScores.push_back({ n, s });
    fw.close();

    if (!wordScores.empty()) {
        sort(wordScores.begin(), wordScores.end(), sortScoresDesc);

        cout << "--- WORD HANGMAN ---\n";

        for (auto& e : wordScores) cout << e.name << " " << e.score << "\n";
    }

    ifstream fm(HIGHSCORE_MATH);
    vector<ScoreEntry> mathScores;
    while (fm >> n >> s) mathScores.push_back({ n, s });
    fm.close();

    if (!mathScores.empty()) {
        sort(mathScores.begin(), mathScores.end(), sortScoresDesc);

        cout << "\n--- MATH HANGMAN ---\n";


        for (auto& e : mathScores) cout << e.name << " " << e.score << "\n";
    }

    cout << "\nPress Enter...";


    cin.get();
}

// ==================== RULES ====================
void waitForEnter() {
    string dummy;
    getline(cin, dummy);
}

void showRules() {
    clearScreen();

    cout << "=========== RULES ===========\n\n";
    cout << "Welcome to Mind Riddle - Hangman!\n";
    cout << "There are two game modes: Math Hangman and Word Hangman.\n\n";

    cout << "MATH HANGMAN\n";
    cout << "Solve the displayed math expression before you run out of attempts.\n";
    cout << "Each wrong answer draws another part of the hangman.\n";
    cout << "Harder difficulties give harder expressions and more points.\n\n";

    cout << "WORD HANGMAN\n";
    cout << "Guess the hidden word by entering letters.\n";
    cout << "Each wrong letter adds to the hangman drawing.\n";
    cout << "Repeated guesses do not cost attempts.\n\n";

    cout << "GENERAL RULES\n";
    cout << "You have a limited number of wrong attempts: " << MAX_WRONG << "\n";
    cout << "If the hangman drawing is completed, the game is over.\n";
    cout << "Correct answers and fewer mistakes earn more points.\n";
    cout << "High scores are saved only if they beat the current record.\n\n";

    cout << "Press Enter to return...\n";
    waitForEnter();

}

// ==================== MAIN ====================
int main() {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif


    srand((unsigned)time(nullptr));
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
            cout << "1. Change difficulty (currently: "
                << (DIFFICULTY == 1 ? "Easy" : DIFFICULTY == 2 ? "Medium" : "Hard") << ")\n";
            cout << "2. Change time to answer (currently: " << TIME_LIMIT << " seconds)\n";
            cout << "3. Back\n";


            cout << "Choice: ";
            {

                int sc;
                if (!(cin >> sc)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    break;
                }
                cin.ignore(1000, '\n');

                if (sc == 1) {
                    clearScreen();
                    cout << "Select difficulty:\n";
                    cout << "1. Easy\n2. Medium\n3. Hard\nChoice: ";
                    int diff;
                    if (!(cin >> diff)) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        break;
                    }
                    cin.ignore(1000, '\n');
                    if (diff >= 1 && diff <= 3) DIFFICULTY = diff;
                }
                else if (sc == 2) {
                    clearScreen();
                    cout << "Enter new time limit (seconds): ";
                    int t;
                    if (!(cin >> t)) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        break;
                    }
                    cin.ignore(1000, '\n');
                    if (t > 0) TIME_LIMIT = t;
                }
                else if (sc == 3) break;


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

