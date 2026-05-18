#ifndef MOON_DATA_H
#define MOON_DATA_H

#include "DateTime.h"
#include <string>
#include <vector>

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
void printResult(const DateTime& target, const MoonResult& result);

void sortDates(std::vector<DateTime>& dates);

#endif // MOON_DATA_H