#pragma once
#ifndef ALMANACTXT_PARSER_HPP
#define ALMANACTXT_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct AlmanacSet {
    int PRN;
    int health;
    double e;
    double toa;
    double orbitalinc;
    double sqrtA;
    double omega;
    double omegaDOT;
    double omega0;
    double M0;
    double af0;
    double af1;
    int week;
};

std::vector<AlmanacSet> parse_almanac_txt(const std::string& filename);


#endif