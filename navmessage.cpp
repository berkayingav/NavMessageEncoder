#include "navmessage.hpp"
#include "rinexParsercpp.hpp"

//Ionospher and UTC Default Simulation Parameters
#define a0 (0xE0)
#define a1 (0x00)
#define a2 (0xF0)
#define a3 (0x00) 
#define b0 (0x90)
#define b1 (0x00)
#define b2 (0xA0)
#define b3 (0x00)
//UTC
#define A0 (0)
#define A1 (0)
#define deltatLS (18) 
#define deltatLSF (18)
#define WNt (0)
#define WNLSF (0) 
#define DN (0)
#define tot (0)

int tow(int gpsWeek, int year, int month, int day, int hour, int min, int sec) {
    std::tm gps_time = {};
    gps_time.tm_year = 80;
    gps_time.tm_mon  = 0;
    gps_time.tm_mday = 6;

    std::tm nav_time = {};
    nav_time.tm_year = year - 1900;
    nav_time.tm_mon  = month - 1;
    nav_time.tm_mday = day;
    nav_time.tm_hour = hour;
    nav_time.tm_min  = min;
    nav_time.tm_sec  = sec;

    time_t t_epoch = std::mktime(&gps_time);
    time_t t_now   = std::mktime(&nav_time);

    gps_Time = static_cast<int>(difftime(t_now, t_epoch)) + 18;  // +18s leap second
    double seconds_in_week =  gps_Time % 604800;  // seconds_in_week
    return (gpsWeek * 604800 + seconds_in_week);
}  

std::bitset<6> compute_parity(std::bitset<24> data) {
    std::bitset<6> parity;
    for (int i = 0; i < 6; ++i) {
        int p = 0;
        for (int j = 0; j < 24; ++j) {
            p ^= (parity_matrix[i][j] & data[j]);
        }
        parity[i] = p;
    }
    return parity;
}

std::bitset<30> create_nav_word(std::bitset<24> data) {
    std::bitset<6> parity = compute_parity(data);
    unsigned long long combined = (data.to_ulong() << 6) | parity.to_ulong();
    return std::bitset<30>(combined);
}

//Subframe1 sat1;
//Subframe2 sat2;





