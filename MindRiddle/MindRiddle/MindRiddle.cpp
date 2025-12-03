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
        cin >> input;

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



