#pragma once
#ifndef RINEX_PARSER_HPP
#define RINEX_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

struct prnSvClock {
    std::string PRN;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;
    double SV_Clock_Bias;
    double SV_Clock_Drift;
    double SV_Clock_Drift_Rate;
};

struct broadcastOrbit1 {
    double IODE;
    double Crs;
    double DeltaN;
    double M0;
};

struct broadcastOrbit2 {
    double Cuc;
    double e;
    double Cus;
    double sqrtA;
};

struct broadcastOrbit3 {
    double Toe;
    double Cic;
    double OMEGA;
    double Cis;
};

struct broadcastOrbit4 {
    double i0;
    double Crc;
    double omega;
    double OMEGA_DOT;
};

struct broadcastOrbit5 {
    double IDOT;
    double CodesL2Channel;
    double GPSWeek;
    double L2P;
};

struct broadcastOrbit6 {
    double SVAccuracy;
    double SVHealth;
    double TGD;
    double IODC;
};

struct broadcastOrbit7 {
    double TTM;
    double fitInterval;
};

struct EphemerisBlock {
    prnSvClock header;
    broadcastOrbit1 orbit1;
    broadcastOrbit2 orbit2;
    broadcastOrbit3 orbit3;
    broadcastOrbit4 orbit4;
    broadcastOrbit5 orbit5;
    broadcastOrbit6 orbit6;
    broadcastOrbit7 orbit7;
};

std::vector<std::string> split_neg_num(const std::string& input, size_t start_index = 0);
std::vector<std::string> fix_negative_nums(const std::vector<std::string>& nums);
int skip_header(std::ifstream& file);
prnSvClock parse_PRN_EPOCH_SV_CLK(const std::vector<std::string>& nums);
broadcastOrbit1 parse_BROADCAST_ORBIT_1(const std::vector<std::string>& nums);
broadcastOrbit2 parse_BROADCAST_ORBIT_2(const std::vector<std::string>& nums);
broadcastOrbit3 parse_BROADCAST_ORBIT_3(const std::vector<std::string>& nums);
broadcastOrbit4 parse_BROADCAST_ORBIT_4(const std::vector<std::string>& nums);
broadcastOrbit5 parse_BROADCAST_ORBIT_5(const std::vector<std::string>& nums);
broadcastOrbit6 parse_BROADCAST_ORBIT_6(const std::vector<std::string>& nums);
broadcastOrbit7 parse_BROADCAST_ORBIT_7(const std::vector<std::string>& nums);
std::vector<std::string> next_line(std::ifstream& file);
std::map<std::string,EphemerisBlock> extract_data(std::ifstream& file);
std::map<std::string, EphemerisBlock> read_rinex(const std::string& filename);

#endif