#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>


#if defined(_WIN32)
#include <windows.h>
#endif

using namespace std;

// global settings
bool showExpression = true;
int maxWrong = 6;
int difficulty = 2;
int timeLimit = 20;
int mathCategory = 2;
int wordCategory = 4;
string highscoreMath = "scores_math.txt";
string highscoreWord = "scores_word.txt";

// colors
const string reset = "\033[0m";
const string red = "\033[31m";
const string green = "\033[32m";
const string bold = "\033[1m";

// functions
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




// math logic
double generateExpression(string& exprStr) {
    int firstNumber, secondNumber;

    if (difficulty == 1) {
        firstNumber = rand() % 10 + 1;
        secondNumber = rand() % 10 + 1;
    }
    else if (difficulty == 2) {
        firstNumber = rand() % 50 + 1;
        secondNumber = rand() % 50 + 1;
    }
    else {
        firstNumber = rand() % 100 + 1;
        secondNumber = rand() % 100 + 1;
    }

    int operationType = 0;

    if (mathCategory == 1) {
        operationType = rand() % 2;
    }
    else if (mathCategory == 2) {
        operationType = rand() % 4;
    }
    else {
        operationType = rand() % 6;
    }

    double result = 0.0;
    stringstream exprStream;

    switch (operationType) {
    case 0:
        result = firstNumber + secondNumber;
        exprStream << firstNumber << " + " << secondNumber;
        break;
    case 1:
        result = firstNumber - secondNumber;
        exprStream << firstNumber << " - " << secondNumber;
        break;
    case 2:
        result = firstNumber * secondNumber;
        exprStream << firstNumber << " * " << secondNumber;
        break;
    case 3:
        secondNumber = (secondNumber == 0 ? 1 : secondNumber);
        result = double(firstNumber) / secondNumber;
        exprStream << firstNumber << " / " << secondNumber;
        break;
    case 4: {
        int baseNumber = rand() % 10 + 1;
        int exponent = rand() % 4 + 1;
        result = pow(baseNumber, exponent);
        exprStream << baseNumber << " ^ " << exponent;
        break;
    }
    case 5: {
        int rootNumber = rand() % 10 + 1;
        int squaredValue = rootNumber * rootNumber;
        result = rootNumber;
        exprStream << "sqrt(" << squaredValue << ")";
        break;
    }
    }

    exprStr = exprStream.str();
    return result;
}

int getBestScore(const string& filename) {
    ifstream inputFile(filename);
    int best = 0, score;
    string name;
    while (inputFile >> name >> score) {
        if (score > best) best = score;
    }
    return best;
}

void saveScoreIfBest(const string& filename, const string& name, int score) {
    int best = getBestScore(filename);
    if (score > best) {
        ofstream outputFile(filename, ios::app);
        outputFile << name << " " << score << "\n";
    }
}

int calculateScore(bool win, int wrong, int difficulty) {
    if (!win) return 0;
    int base = 50;
    int diffBonus = difficulty * 25;
    int livesBonus = (maxWrong - wrong) * 5;
    return base + diffBonus + livesBonus;
}

// category selection
void chooseMathCategory() {
    clearScreen();
    cout << "=== MATH CATEGORIES ===\n";
    cout << "1. Easy (+ -)\n";
    cout << "2. Medium (+ - * /)\n";
    cout << "3. Hard (+ - * / ^ sqrt)\n";
    cout << "Choose: ";

    int categoryChoice;
    if (!(cin >> categoryChoice)) {
        cin.clear();
        cin.ignore(1000, '\n');
        categoryChoice = 2;
    }
    cin.ignore();

    if (categoryChoice >= 1 && categoryChoice <= 3) {
        mathCategory = categoryChoice;
    }
    else {
        mathCategory = 2;
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

    int categoryChoice;
    if (!(cin >> categoryChoice)) {
        cin.clear();
        cin.ignore(1000, '\n');
        categoryChoice = 4;
    }
    cin.ignore();

    if (categoryChoice >= 1 && categoryChoice <= 4) {
        wordCategory = categoryChoice;
    }
    else {
        wordCategory = 4;
    }
}

void showHighScores();
void showRules();

// math hangman
void playMathHangman() {
    chooseMathCategory();
    clearScreen();
    int wrong = 0;
    int totalScore = 0;

    cout << bold << green;
    cout << "=== MATHEMATICAL HANGMAN ===\n";
    cout << reset;

    while (wrong < maxWrong) {
        string exprStr;
        double answer = generateExpression(exprStr);

        if (showExpression) {
            cout << "Expression: " << exprStr << "\n";
        }

        bool correct = false;
        const double EPS = 1e-4;

        while (!correct && wrong < maxWrong) {
            cout << "Remaining attempts: " << (maxWrong - wrong) << "\n";
            printHangman(wrong);

            cout << "Enter answer (or 'quit' to quit): ";

            auto start = chrono::steady_clock::now();
            string input;
            getline(cin, input);
            auto end = chrono::steady_clock::now();
            int elapsed = chrono::duration_cast<chrono::seconds>(end - start).count();

            if (elapsed > timeLimit) {
                cout << red << "Time's up!\n" << reset;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }

            input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());

            if (input.empty()) {
                cout << red << "Invalid input!\n" << reset;
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
                cout << red << "Invalid input!\n" << reset;
                continue;
            }

            if (fabs(value - answer) < EPS) {
                totalScore += calculateScore(true, wrong, difficulty);
                cout << green << "Correct!\n" << reset;
                sleepSeconds(1);
                clearScreen();
                correct = true;
            }
            else {
                cout << red << "Wrong!\n" << reset;
                wrong++;
                sleepSeconds(1);
                clearScreen();
                break;
            }
        }
    }

    clearScreen();
    cout << red << "=== GAME OVER ===\n" << reset;
    cout << "Your total score: " << totalScore << "\n";

    int best = getBestScore(highscoreMath);
    if (totalScore > best) {
        cout << "NEW HIGHSCORE!\n";
        string name;
        do {
            cout << "Enter your name: ";
            getline(cin, name);
        } while (name.empty());
        saveScoreIfBest(highscoreMath, name, totalScore);
    }

    cout << "Press Enter...";
    cin.get();
}


// word hangman
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

    if (wordCategory == 1) words = animals;
    else if (wordCategory == 2) words = foods;
    else if (wordCategory == 3) words = tech;
    else words = random;

    string word = words[rand() % words.size()];
    string hidden(word.size(), '_');
    int wrong = 0;
    bool win = false;
    vector<char> guessed;

    cout << bold << green;
    cout << "=== WORD HANGMAN ===\n";
    cout << reset;

    while (wrong < maxWrong && !win) {
        cout << "\nCurrent: " << hidden << "\n";
        cout << "Remaining wrong attempts: " << (maxWrong - wrong) << "\n";

        printHangman(wrong);

        cout << "Enter a letter (or 'quit' to quit): ";
        string userInput;
        getline(cin, userInput);

        if (userInput.empty()) {
            cout << red << "Invalid input!\n" << reset;
            continue;
        }

        if (userInput == "quit" || userInput == "QUIT" || userInput == "Quit") return;

        char guessedChar = tolower(userInput[0]);



        if (!isalpha(guessedChar) || userInput.size() != 1) {
            cout << red << "Invalid input!\n" << reset;
            continue;
        }

        if (find(guessed.begin(), guessed.end(), guessedChar) != guessed.end()) continue;

        guessed.push_back(guessedChar);

        bool found = false;
        for (int i = 0; i < (int)word.size(); i++) {
            if (word[i] == guessedChar) {
                hidden[i] = guessedChar;
                found = true;
            }
        }

        if (!found) {
            wrong++;
            cout << red << "Wrong guess!\n" << reset;
        }

        if (hidden == word) win = true;
    }

    int score = (win ? (word.size() * 10 + (maxWrong - wrong) * 5) : 0);
    clearScreen();

    if (win) {
        cout << green << "=== YOU WIN! ===\n";
        cout << "Word: " << word << "\n";
        cout << "Score: " << score << reset << "\n";

        int best = getBestScore(highscoreWord);
        if (score > best) {
            cout << "NEW HIGHSCORE!\n";
            string name;
            do {
                cout << "Enter your name: ";
                getline(cin, name);
            } while (name.empty());
            saveScoreIfBest(highscoreWord, name, score);
        }
    }
    else {
        printHangman(maxWrong);
        cout << red << "=== YOU LOSE ===\n";
        cout << "Word: " << word << reset << "\n";
    }

    cout << "Press Enter...";
    cin.get();
}

// high scores
struct ScoreEntry { string name; int score; };

bool sortScoresDesc(const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; }

void showHighScores() {
    clearScreen();

    cout << "======== HIGH SCORES ========\n\n";

    ifstream fw(highscoreWord);
    vector<ScoreEntry> wordScores;
    string playerName; int playerScore;
    while (fw >> playerName >> playerScore) wordScores.push_back({ playerName, playerScore });
    fw.close();

    if (!wordScores.empty()) {
        sort(wordScores.begin(), wordScores.end(), sortScoresDesc);

        cout << "--- WORD HANGMAN ---\n";

        for (auto& e : wordScores) cout << e.name << " " << e.score << "\n";
    }

    ifstream fm(highscoreMath);
    vector<ScoreEntry> mathScores;
    while (fm >> playerName >> playerScore) mathScores.push_back({ playerName, playerScore });
    fm.close();

    if (!mathScores.empty()) {
        sort(mathScores.begin(), mathScores.end(), sortScoresDesc);

        cout << "\n--- MATH HANGMAN ---\n";


        for (auto& e : mathScores) cout << e.name << " " << e.score << "\n";
    }

    cout << "\nPress Enter...";


    cin.get();
}

// rules
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
    cout << "You have a limited number of wrong attempts: " << maxWrong << "\n";
    cout << "If the hangman drawing is completed, the game is over.\n";
    cout << "Correct answers and fewer mistakes earn more points.\n";
    cout << "High scores are saved only if they beat the current record.\n\n";

    cout << "Press Enter to return...\n";
    waitForEnter();

}

// main
int main() {
    srand((unsigned)time(nullptr));
    while (true) {
        clearScreen();
        cout << "=== MIND RIDDLE - HANGMAN ===\n";
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
                << (difficulty == 1 ? "Easy" : difficulty == 2 ? "Medium" : "Hard") << ")\n";
            cout << "2. Change time to answer (currently: " << timeLimit << " seconds)\n";
            cout << "3. Back\n";


            cout << "Choice: ";
            {

                int settingsChoice;
                if (!(cin >> settingsChoice)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    break;
                }
                cin.ignore(1000, '\n');

                if (settingsChoice == 1) {
                    clearScreen();
                    cout << "Select difficulty:\n";
                    cout << "1. Easy\n2. Medium\n3. Hard\nChoice: ";
                    int difficulty;
                    if (!(cin >> difficulty)) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        break;
                    }
                    cin.ignore(1000, '\n');
                    if (difficulty >= 1 && difficulty <= 3) difficulty = difficulty;
                }
                else if (settingsChoice == 2) {
                    clearScreen();
                    cout << "Enter new time limit (seconds): ";
                    int newTimeLimit;
                    if (!(cin >> newTimeLimit)) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        break;
                    }
                    cin.ignore(1000, '\n');
                    if (newTimeLimit > 0) timeLimit = newTimeLimit;
                }
                else if (settingsChoice == 3) break;


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
