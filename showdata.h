#pragma once

#include <string>

using namespace std;

namespace showdata {
    // 显示居中的文本框，支持自动换行
    // 在内部显示文本 text，支持段落自动换行
    // 如果 waitForInput 为 true，等待用户按 Enter 返回 -1，或按数字键 0-9 返回对应数字
    // 如果 waitForInput 为 false，显示后立即返回 -1
    int showCenteredBox(const string &text, bool waitForInput = true);
}
