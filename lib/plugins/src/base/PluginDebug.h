#pragma once

#include <stdint.h>
#include <stddef.h>
#include <Print.h>

enum PDebugLevel { NONE, WARN, INFO, DEBUG, TRACE };

class PDebugClass {
    public:
    PDebugClass() {}
#ifdef PDEBUG
    template<typename... Args> size_t printf(PDebugLevel level, const char * f, Args... args) {
        if(print != nullptr && isLevel(level))
           return print->printf(f, args...);
        return 0;
    }   
#else
    template<typename... Args> size_t printf(PDebugLevel level, const char * f, Args... args) {}
#endif

    void setPrint(Print* p) {
        print = p;
    }
    void setLevel(PDebugLevel l) {
        level = l;
    }
    bool isLevel(PDebugLevel l) {
        return (l<=level);
    }
    private:
    PDebugLevel level = PDebugLevel::DEBUG;
    Print* print = nullptr;
};
extern PDebugClass PDebug;
