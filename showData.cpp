#include "showData.h"

#include <windows.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace showdata {

static vector<string> wrapText(const string &text, int maxWidth)
{
    vector<string> lines;
    // 保留显式换行，先按行分段
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
            if ((int)cur.size() + (cur.empty() ? 0 : 1) + (int)word.size() > maxWidth) {
                if (!cur.empty()) {
                    lines.push_back(cur);
                    cur.clear();
                }
            }
            if (!cur.empty()) cur += " ";
            cur += word;
        }
        if (!cur.empty()) lines.push_back(cur);
        // 如果段落本身为空，保留空行
        if (para.empty()) lines.push_back(string());
    }
    if (lines.empty()) lines.push_back(string());
    return lines;
}

int showCenteredBox(const string &text)
{
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
    for (auto &ln : lines) contentWidth = max<int>(contentWidth, (int)ln.size());
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

    // 顶部/底部边框
    outBuf[0 * boxWidth + 0].Char.UnicodeChar = L'+';
    outBuf[0 * boxWidth + boxWidth - 1].Char.UnicodeChar = L'+';
    for (int x = 1; x < boxWidth - 1; ++x) outBuf[0 * boxWidth + x].Char.UnicodeChar = L'-';
    int by = boxHeight - 1;
    outBuf[by * boxWidth + 0].Char.UnicodeChar = L'+';
    outBuf[by * boxWidth + boxWidth - 1].Char.UnicodeChar = L'+';
    for (int x = 1; x < boxWidth - 1; ++x) outBuf[by * boxWidth + x].Char.UnicodeChar = L'-';

    // 内容行与左右竖线
    for (int i = 0; i < (int)lines.size(); ++i) {
        int y = 1 + i;
        outBuf[y * boxWidth + 0].Char.UnicodeChar = L'|';
        outBuf[y * boxWidth + boxWidth - 1].Char.UnicodeChar = L'|';
        const string &ln = lines[i];
        int padLeft = (contentWidth - (int)ln.size()) / 2;
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

    // 隐藏光标
    CONSOLE_CURSOR_INFO oldCursorInfo;
    GetConsoleCursorInfo(hOut, &oldCursorInfo);
    CONSOLE_CURSOR_INFO hideCursor = oldCursorInfo;
    hideCursor.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &hideCursor);

    // 等待输入：按数字键立即返回对应数字；按 Enter 返回 -1；其他键忽略
    int result = -1;
    INPUT_RECORD rec;
    DWORD read = 0;
    while (true) {
        if (!ReadConsoleInput(hIn, &rec, 1, &read)) break;
        if (rec.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD &k = rec.Event.KeyEvent;
            if (!k.bKeyDown) continue; // 只在按下时处理一次
            // 优先使用字符形式（支持主键盘数字）
            wchar_t ch = k.uChar.UnicodeChar;
            if (ch >= L'0' && ch <= L'9') {
                result = (int)(ch - L'0');
                break;
            }
            // 支持数字键盘 (VK_NUMPAD0..VK_NUMPAD9)
            if (k.wVirtualKeyCode >= VK_NUMPAD0 && k.wVirtualKeyCode <= VK_NUMPAD9) {
                result = k.wVirtualKeyCode - VK_NUMPAD0;
                break;
            }
            // Enter -> 取消/返回 -1
            if (k.wVirtualKeyCode == VK_RETURN) {
                result = -1;
                break;
            }
        }
    }

    // 恢复光标显示
    SetConsoleCursorInfo(hOut, &oldCursorInfo);

    // 恢复原区域
    WriteConsoleOutput(hOut, backup.data(), bufSize, bufCoord, &readRect);

    return result;
}

} // namespace showdata     