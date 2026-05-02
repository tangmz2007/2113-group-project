#include "showdata.h"

#include <iostream>
#include <limits>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace showdata {

static int visibleTextLength(const string &text)
{
    // calculates the length of the text excluding ANSI escape codes
    //inputs a string with possible ANSI escape codes, returns the length of the visible text
    int length = 0;
    bool inAnsi = false;
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char ch = text[i];
        if (!inAnsi) {
            if (ch == '\033' && i + 1 < text.size() && text[i + 1] == '[') {
                inAnsi = true;
                ++i;
            } else {
                ++length;
            }
        } else {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                inAnsi = false;
            }
        }
    }
    return length;
}

static vector<string> wrapText(const string &text, int maxWidth)
{ // wraps the text into lines of at most maxWidth visible characters, breaking at word boundaries
    //inputs a string and a maximum width, 
    //returns a vector of strings where each string is a line of text that fits within the maximum width, breaking at word boundaries
    vector<string> lines;
    
    vector<string> paragraphs;
    {
        istringstream s(text);
        string line;
        while (getline(s, line)) paragraphs.push_back(line);
    }

    for (const auto &para : paragraphs) {
        istringstream words(para);
        string word;
        string cur;
        while (words >> word) {
            if (visibleTextLength(cur) + (cur.empty() ? 0 : 1) + visibleTextLength(word) > maxWidth) {
                if (!cur.empty()) {
                    lines.push_back(cur);
                    cur.clear();
                }
            }
            if (!cur.empty()) cur += " ";
            cur += word;
        }
        if (!cur.empty()) lines.push_back(cur);
        // If the paragraph is empty, we should still add an empty line to represent it.
        if (para.empty()) lines.push_back(string());
    }
    if (lines.empty()) lines.push_back(string());
    return lines;
}

int showCenteredBox(const string &text, bool waitForInput)
{
    //displays a box with the given text centered inside, wrapped to fit within the box.
    vector<string> lines = wrapText(text, 60);
    int contentWidth = 0;
    for (auto &ln : lines) contentWidth = max<int>(contentWidth, visibleTextLength(ln));
    int boxWidth = contentWidth + 4;

    cout << "+" << string(boxWidth - 2, '-') << "+\n";
    for (const auto &ln : lines) {
        int visibleLen = visibleTextLength(ln);
        cout << "| " << ln << string(contentWidth - visibleLen, ' ') << " |\n";
    }
    cout << "+" << string(boxWidth - 2, '-') << "+\n";

    if (waitForInput) {
        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return -1;
}

} // namespace showdata     
