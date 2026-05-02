#include "showData.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#include <limits>
#endif
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace showdata {

static int visibleTextLength(const string &text)
{
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
{
    vector<string> lines;
    // ������ʽ���У��Ȱ��зֶ�
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
        // ������䱾��Ϊ�գ���������
        if (para.empty()) lines.push_back(string());
    }
    if (lines.empty()) lines.push_back(string());
    return lines;
}

int showCenteredBox(const string &text, bool waitForInput)
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE || hIn == INVALID_HANDLE_VALUE) return -1;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return -1;
    int winLeft = csbi.srWindow.Left;
    int winTop = csbi.srWindow.Top;
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    const int MAX_BOX_WIDTH = max(20, min(60, width - 4));
    const int MAX_BOX_HEIGHT = max(3, height - 4);

    int contentMaxWidth = MAX_BOX_WIDTH - 4;
    if (contentMaxWidth < 10) contentMaxWidth = max(10, width - 6);

    vector<string> lines = wrapText(text, contentMaxWidth);
    if ((int)lines.size() > MAX_BOX_HEIGHT - 2) {
        lines.resize(MAX_BOX_HEIGHT - 3);
        lines.push_back("...");
    }

    int contentWidth = 0;
    for (auto &ln : lines) contentWidth = max<int>(contentWidth, visibleTextLength(ln));
    int boxWidth = contentWidth + 4;
    int boxHeight = (int)lines.size() + 2;

    int startX = winLeft + (width - boxWidth) / 2;
    int startY = winTop + (height - boxHeight) / 2;

    SMALL_RECT readRect;
    readRect.Left = (SHORT)startX;
    readRect.Top = (SHORT)startY;
    readRect.Right = (SHORT)(startX + boxWidth - 1);
    readRect.Bottom = (SHORT)(startY + boxHeight - 1);

    COORD bufSize = { (SHORT)boxWidth, (SHORT)boxHeight };
    COORD bufCoord = { 0, 0 };
    vector<CHAR_INFO> backup(boxWidth * boxHeight);
    if (!ReadConsoleOutput(hOut, backup.data(), bufSize, bufCoord, &readRect)) {
        return -1;
    }

    vector<CHAR_INFO> outBuf(boxWidth * boxHeight);
    WORD attr = csbi.wAttributes;
    for (int y = 0; y < boxHeight; ++y) {
        for (int x = 0; x < boxWidth; ++x) {
            CHAR_INFO &ci = outBuf[y * boxWidth + x];
            ci.Attributes = attr;
            ci.Char.UnicodeChar = L' ';
        }
    }

    outBuf[0 * boxWidth + 0].Char.UnicodeChar = L'+';
    outBuf[0 * boxWidth + boxWidth - 1].Char.UnicodeChar = L'+';
    for (int x = 1; x < boxWidth - 1; ++x) outBuf[0 * boxWidth + x].Char.UnicodeChar = L'-';
    int by = boxHeight - 1;
    outBuf[by * boxWidth + 0].Char.UnicodeChar = L'+';
    outBuf[by * boxWidth + boxWidth - 1].Char.UnicodeChar = L'+';
    for (int x = 1; x < boxWidth - 1; ++x) outBuf[by * boxWidth + x].Char.UnicodeChar = L'-';

    for (int i = 0; i < (int)lines.size(); ++i) {
        int y = 1 + i;
        outBuf[y * boxWidth + 0].Char.UnicodeChar = L'|';
        outBuf[y * boxWidth + boxWidth - 1].Char.UnicodeChar = L'|';
        const string &ln = lines[i];
        int visibleLen = visibleTextLength(ln);
        int padLeft = (contentWidth - visibleLen) / 2;
        int insertX = 1 + 1 + padLeft;
        for (int k = 0; k < (int)ln.size() && insertX + k < boxWidth - 1; ++k) {
            unsigned char ch = ln[k];
            outBuf[y * boxWidth + insertX + k].Char.UnicodeChar = (wchar_t)ch;
        }
    }

    SMALL_RECT writeRect = readRect;
    if (!WriteConsoleOutput(hOut, outBuf.data(), bufSize, bufCoord, &writeRect)) {
        WriteConsoleOutput(hOut, backup.data(), bufSize, bufCoord, &readRect);
        return -1;
    }

    CONSOLE_CURSOR_INFO oldCursorInfo;
    GetConsoleCursorInfo(hOut, &oldCursorInfo);
    CONSOLE_CURSOR_INFO hideCursor = oldCursorInfo;
    hideCursor.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &hideCursor);

    int result = -1;
    if (waitForInput) {
        INPUT_RECORD rec;
        DWORD read = 0;
        while (true) {
            if (!ReadConsoleInput(hIn, &rec, 1, &read)) break;
            if (rec.EventType == KEY_EVENT) {
                KEY_EVENT_RECORD &k = rec.Event.KeyEvent;
                if (!k.bKeyDown) continue;
                wchar_t ch = k.uChar.UnicodeChar;
                if (ch >= L'0' && ch <= L'9') {
                    result = (int)(ch - L'0');
                    break;
                }
                if (k.wVirtualKeyCode >= VK_NUMPAD0 && k.wVirtualKeyCode <= VK_NUMPAD9) {
                    result = k.wVirtualKeyCode - VK_NUMPAD0;
                    break;
                }
                if (k.wVirtualKeyCode == VK_RETURN) {
                    result = -1;
                    break;
                }
            }
        }
    }

    SetConsoleCursorInfo(hOut, &oldCursorInfo);
    WriteConsoleOutput(hOut, backup.data(), bufSize, bufCoord, &readRect);
    return result;
#else
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
#endif
}

} // namespace showdata     
