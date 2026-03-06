#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <type_traits>
#include <iostream>

using namespace std;

// helpers for std::string so that Serializer can use them via ADL
inline void serialize(std::ostream &os, const std::string &s) {
    size_t n = s.size();
    os.write(reinterpret_cast<const char*>(&n), sizeof(n));
    os.write(s.data(), n);
}
inline void deserialize(std::istream &is, std::string &s) {
    size_t n;
    is.read(reinterpret_cast<char*>(&n), sizeof(n));
    s.resize(n);
    is.read(&s[0], n);
}

namespace sjtu {

// ---------------------------------------------------------------------------
// serialization support
// ---------------------------------------------------------------------------
// default serializer: copy bytes if the type is trivially copyable.  If the
// type is not trivially copyable the user must either provide a free
// ``serialize``/``deserialize`` pair in the same namespace as the type, or
// specialise ``Serializer`` for the type.  The ADL-based overload below will
// pick the free functions automatically.

template <typename T, typename = void>
struct Serializer {
    static void write(ostream &os, const T &t) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "no serializer defined for non-trivially-copyable type");
        os.write(reinterpret_cast<const char *>(&t), sizeof(T));
    }
    static void read(istream &is, T &t) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "no serializer defined for non-trivially-copyable type");
        is.read(reinterpret_cast<char *>(&t), sizeof(T));
    }
};

// detect member functions serialize()/deserialize() as well as free
// functions.  The priority is:
//   1. member `t.serialize(os)` / `t.deserialize(is)`
//   2. ADL-visible free `serialize(os, t)` / `deserialize(is, t)`
//   3. fallback to trivial memcpy (checked by static_assert above)

// helper meta to test for a member function

template <typename T, typename = void>
struct has_member_serializer : std::false_type {};

template <typename T>
struct has_member_serializer<
    T,
    std::void_t<decltype(std::declval<const T &>().serialize(
        std::declval<ostream &>())),
                decltype(std::declval<T &>().deserialize(
                    std::declval<istream &>()))>> : std::true_type {};

// helper meta to test for ADL free functions

template <typename T, typename = void>
struct has_adl_serializer : std::false_type {};

template <typename T>
struct has_adl_serializer<
    T,
    std::void_t<decltype(serialize(std::declval<ostream &>(),
                                   std::declval<const T &>())),
                decltype(deserialize(std::declval<istream &>(),
                                      std::declval<T &>()))>>
    : std::true_type {};

// serializer when member functions exist

template <typename T>
struct Serializer<
    T,
    std::enable_if_t<has_member_serializer<T>::value>> {
    static void write(ostream &os, const T &t) { t.serialize(os); }
    static void read(istream &is, T &t) { t.deserialize(is); }
};

// ADL-based free-function serializer (falls back when member is absent)

template <typename T>
struct Serializer<
    T,
    std::enable_if_t<!has_member_serializer<T>::value &&
                     has_adl_serializer<T>::value>> {
    static void write(ostream &os, const T &t) { serialize(os, t); }
    static void read(istream &is, T &t) { deserialize(is, t); }
};

// ---------------------------------------------------------------------------

template <class T, int info_len = 2>
class MemoryRiver {
   private:
    fstream file;
    string file_name;
    static constexpr int CURRENT_VERSION = 1;  // Current serialization version
    static constexpr size_t BUFFER_SIZE = 32 * 1024;  // 32KB buffer
    char* buffer;

   public:
    MemoryRiver() : buffer(nullptr) {}
    MemoryRiver(const string& fn) : file_name(fn), buffer(nullptr) {
        buffer = new char[BUFFER_SIZE];
    }
    ~MemoryRiver() {
        if (file.is_open()) {
            file.flush();
            file.close();
        }
        if (buffer) {
            delete[] buffer;
        }
    }

    void initialise(string fn = "") {
        if (!fn.empty()) file_name = fn;
        if (!filesystem::exists(file_name)) {
            // Create new file with current version
            file.open(file_name, ios::binary | ios::in | ios::out | ios::trunc);
            int zero = 0;
            // Write info header (info_len ints)
            for (int i = 0; i < info_len; i++)
                file.write(reinterpret_cast<char*>(&zero), sizeof(int));
            // Write version after the info header
            file.write(reinterpret_cast<const char*>(&CURRENT_VERSION), sizeof(int));
            file.close();
        } else {
            // Check existing file version (version is at position info_len * sizeof(int))
            fstream check_file(file_name, ios::binary | ios::in);
            check_file.seekg(info_len * sizeof(int));
            int stored_version = 0;
            check_file.read(reinterpret_cast<char*>(&stored_version), sizeof(int));
            check_file.close();

            if (stored_version != CURRENT_VERSION) {
                // Version mismatch: clear old file and recreate with current version
                filesystem::remove(file_name);
                file.open(file_name, ios::binary | ios::in | ios::out | ios::trunc);
                int zero = 0;
                for (int i = 0; i < info_len; i++)
                    file.write(reinterpret_cast<char*>(&zero), sizeof(int));
                file.write(reinterpret_cast<const char*>(&CURRENT_VERSION), sizeof(int));
                file.close();
            }
        }
        file.open(file_name, ios::binary | ios::in | ios::out);
        if (!buffer) {
            buffer = new char[BUFFER_SIZE];
        }
        file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);
    }

    void get_info(int& x, int n) {
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char*>(&x), sizeof(int));
    }

    void write_info(int x, int n) {
        file.seekp((n - 1) * sizeof(int));
        file.write(reinterpret_cast<char*>(&x), sizeof(int));
    }

    int write(const T& t) {
        file.seekp(0, ios::end);
        int pos = file.tellp();
        Serializer<T>::write(file, t);
        return pos;
    }

    void read(T& t, int pos) {
        file.seekg(pos);
        Serializer<T>::read(file, t);
    }

    void update(const T& t, int pos) {
        file.seekp(pos);
        Serializer<T>::write(file, t);
    }
    void clean_up() {
        file.close();
        filesystem::remove(file_name);
        initialise(file_name);
    }
};
}  // namespace sjtu