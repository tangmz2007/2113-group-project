#pragma once

#include <string>

using namespace std;

namespace showdata {
    // 在屏幕居中显示一个由 '+', '-', '|' 组成边框的对话框，
    // 内部显示传入的 text（支持换行与自动换行）。
    // 如果用户按 Enter：返回 -1；如果用户按数字键（0-9 或数字键盘）：立即返回该数字（0-9）。
    int showCenteredBox(const string &text);
}
