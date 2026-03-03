#include "../include/parser.hpp"

#include <iostream>

namespace sjtu {

void Input::Skip() {
    std::cin.get(las_c_);
}

auto Input::GetTimestamp() -> int {
    assert(las_c_ == '\n');
    std::cin.get(las_c_);
    assert(las_c_ == '[');
    int res = 0;
    std::cin.get(las_c_);
    while (las_c_ != ']') {
        res = res * 10 + las_c_ - '0';
        std::cin.get(las_c_);
    }
    std::cin.get(las_c_);
    assert(las_c_ == ' ');
    return res;
}

auto Input::GetCommand() -> std::string {
    assert(las_c_ == ' ');
    std::cin.get(las_c_);
    std::string res;
    while (las_c_ != ' ' && las_c_ != '\n') {
        res += las_c_;
        std::cin.get(las_c_);
    }
    return res;
}

auto Input::GetKey() -> char {
    if (las_c_ == '\n') {
        return '\n';
    }
    assert(las_c_ == ' ');
    std::cin.get(las_c_);
    assert(las_c_ == '-');
    std::cin.get(las_c_);
    char res = las_c_;
    std::cin.get(las_c_);
    assert(las_c_ == ' ');
    return res;
}

auto Input::GetChar() -> char {
    assert(las_c_ == ' ');
    std::cin.get(las_c_);
    char res = las_c_;
    std::cin.get(las_c_);
    return res;
}

auto Input::GetInteger() -> int {
    assert(las_c_ == ' ' || las_c_ == '|');
    int res = 0;
    std::cin.get(las_c_);
    if (las_c_ == '_') {
        Skip();
        return -1;
    }
    while (las_c_ >= '0' && las_c_ <= '9') {
        res = res * 10 + las_c_ - '0';
        std::cin.get(las_c_);
    }
    return res;
}

auto Input::GetDate() -> int {
    Skip();
    int res = 0;
    std::cin.get(las_c_);
    if (las_c_ == '7') {
        res = 30;
    } else if (las_c_ == '8') {
        res = 61;
    } else if (las_c_ != '6') {
        res = 114514;  // a impossible date
    }
    Skip();
    std::cin.get(las_c_);
    res += 10 * (las_c_ - '0');
    std::cin.get(las_c_);
    res += las_c_ - '0';
    Skip();
    return res;
}

auto Input::GetTime() -> int {
    int res = 0;
    std::cin.get(las_c_);
    res = (las_c_ - '0') * 600;
    std::cin.get(las_c_);
    res += (las_c_ - '0') * 60;
    Skip();
    std::cin.get(las_c_);
    res += (las_c_ - '0') * 10;
    std::cin.get(las_c_);
    res += las_c_ - '0';
    std::cin.get(las_c_);
    return res;
}
auto Input::GetString() -> String {
    // assert(las_c_ == ' ');
    int pos = 0;
    std::string res;
    std::cin.get(las_c_);
    while (las_c_ != ' ' && las_c_ != '\n'&&las_c_!='|') {
        res += las_c_;
        std::cin.get(las_c_);
    }
    return String(res);
}

auto Input::GetIntegerArray() -> vector<int> {
    assert(las_c_ == ' ');
    int pos = 0;
    vector<int> res;
    int val = GetInteger();
    if (val == -1) {
        return res;
    }
    while (las_c_ == '|') {
        res.push_back(val);
        val = GetInteger();
    }
    res.push_back(val);
    return res;
}

auto Input::GetStringArray() -> vector<String> {
    assert(las_c_ == ' ');
    int pos = 0;
    vector<String> res;
    String val = GetString();
    // std::cout<<"get_str:"<<val<<'\n';
    while (las_c_ == '|') {
        res.push_back(val);
        val = GetString();
    }
    res.push_back(val);
    return res;
}

}  // namespace sjtu