#ifndef PARTICLE_H__INCLUDED
#define PARTICLE_H__INCLUDED

#include <map>
#include <vector>
#include <stdio.h>

// forward declarations
class Gluon;
class Boson;
class Molecule;

typedef string BosonId;


enum PropertyType {
    e_price_property,
    e_size_property
};

enum TypeOfTick {
    e_bid,
    e_ask,
    e_last,
    e_high,
    e_low,
    e_close,
    e_typeoftick_size
};


inline static void printTimeStamp() {
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    int secs = now->tm_hour*3600 + now->tm_min*60 + now->tm_sec;
    printf("%04d%02d%02d.%d",(now->tm_year + 1900),(now->tm_mon + 1),now->tm_mday,secs);
//    std::cout << (now->tm_year + 1900)
//              << (now->tm_mon + 1)
//              <<  now->tm_mday 
//              << "." 
//              << secs
//              << std::endl;
}

// forward declarations
//class Boson;
//class Gluon;

#endif