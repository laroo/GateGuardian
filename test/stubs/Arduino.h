#pragma once
#include <string>
#include <cstdint>
#include <cstdio>

// ---------------------------------------------------------------------------
// String — thin wrapper around std::string
// ---------------------------------------------------------------------------
class String : public std::string {
public:
    String() = default;
    String(const char* s) : std::string(s ? s : "") {}
    String(const std::string& s) : std::string(s) {}
    String(int v)  : std::string(std::to_string(v)) {}
    String(unsigned long v) : std::string(std::to_string(v)) {}
    const char* c_str() const { return std::string::c_str(); }
    bool operator==(const char* rhs) const { return std::string::compare(rhs) == 0; }
    bool operator!=(const char* rhs) const { return !(*this == rhs); }
    void toUpperCase() {
        for (auto& c : *this) c = (char)std::toupper((unsigned char)c);
    }
    void trim() {
        auto f = find_first_not_of(" \t\r\n");
        if (f == npos) { clear(); return; }
        *this = substr(f, find_last_not_of(" \t\r\n") - f + 1);
    }
    size_t length() const { return std::string::size(); }
};

// ---------------------------------------------------------------------------
// Serial stub — prints to stdout
// ---------------------------------------------------------------------------
struct SerialStub {
    template<typename T>
    void print(T v)   { printf("%s", String(v).c_str()); }
    void print(bool v) { printf("%s", v ? "1" : "0"); }
    void print(const char* v) { if (v) printf("%s", v); }
    template<typename T>
    void println(T v) { print(v); printf("\n"); }
    void println(const char* v) { print(v); printf("\n"); }
    void println() { printf("\n"); }
    void flush() {}
} Serial;

// ---------------------------------------------------------------------------
// GPIO stubs — no-ops
// ---------------------------------------------------------------------------
#define INPUT         0
#define INPUT_PULLUP  2
#define OUTPUT        1
#define HIGH          1
#define LOW           0

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int  digitalRead(int) { return LOW; }

// ---------------------------------------------------------------------------
// millis() — backed by a global the GateTestHarness overrides via _millis()
// ---------------------------------------------------------------------------
inline unsigned long millis() { return 0; }

// ---------------------------------------------------------------------------
// Minimal byte typedef
// ---------------------------------------------------------------------------
typedef uint8_t byte;
