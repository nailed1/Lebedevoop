#ifndef MOON_H
#define MOON_H
#include "DateTime.h"

struct MoonResult {
    bool ok = true;
    
    bool hasRise = false;
    bool hasCulmination = false;
    bool hasSet = false;

    DateTime riseTime;
    DateTime culminationTime;
    DateTime setTime;
};

MoonResult processMoonData(const DateTime& target);
void printResult(const DateTime& target, const MoonResult& r);

#endif