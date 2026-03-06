#pragma once
#include<string>
#include<cstring>
#include<iostream>
using namespace std;
#include "../memoryriver/memoryriver.hpp"  // for Serializer
struct String {
    static constexpr size_t MAX_LEN = 70;
    char s[MAX_LEN + 1];
    String() {
        s[0] = '\0';
    }

    String(const std::string& str) {
        size_t len = str.size() < MAX_LEN ? str.size() : MAX_LEN;
        std::memcpy(s, str.data(), len);
        s[len] = '\0';
    }

    String& operator=(const std::string& str) {
        size_t len = str.size() < MAX_LEN ? str.size() : MAX_LEN;
        std::memcpy(s, str.data(), len);
        s[len] = '\0';
        return *this;
    }

    std::string to_string() const {
        return std::string(s);
    }

    bool operator==(const String& o) const {
        return std::strcmp(s, o.s) == 0;
    }
    bool operator!=(const String& o) const {
        return !(*this == o);
    }
    bool operator<(const String& o) const {
        return std::strcmp(s, o.s) < 0;
    }
    bool operator>(const String& o) const {
        return std::strcmp(s, o.s) > 0;
    }
    bool operator<=(const String& o) const {
        return !(*this > o);
    }
    bool operator>=(const String& o) const {
        return !(*this < o);
    }
    static string FromInt(int x){
        string st="";
        while(x)
        {
            st+=char(x%10+'0');
            x/=10;
        }
        if(st.size()==0) st="00";
        else if(st.size()==1) st="0"+st;
        else swap(st[0],st[1]);
        return st;  
    }
    static String min_value() {
        String ms;
        ms.s[0] = '\0';
        return ms;
    }
    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.s;
        return os;
    }

    // fixed-size serialization
    void serialize(std::ostream &os) const {
        os.write(s, MAX_LEN + 1);
    }
    void deserialize(std::istream &is) {
        is.read(s, MAX_LEN + 1);
    }
};