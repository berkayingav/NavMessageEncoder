#pragma once

#include <iostream>
#include <bitset>
#include <vector>
#include <ctime>
#include <math.h>

int gps_Time = 0;

const int parity_matrix[6][24] = {
    {1,1,1,0,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,0,0},
    {1,0,1,1,1,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,1,0,0},
    {1,1,0,1,0,1,1,0,0,0,0,1,0,1,1,1,0,0,1,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,0,1},
    {1,1,1,1,0,1,0,1,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,0},
    {0,1,1,0,1,0,1,1,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0}
};

int tow(int gpsWeek, int year, int month, int day, int hour, int min, int sec);
std::bitset<6> compute_parity(std::bitset<24> data);
std::bitset<30> create_nav_word(std::bitset<24> data);

struct UTC{
    double A0;
    double A1;
    int deltatLS;
    double tot;
    int WNt;
    int WNLSF;
    int DN;
    int deltatLSF;
};

struct Ionespher{
    double a0;
    double a1;
    double a2;
    double a3;
    double b0;
    double b1;
    double b2;
    double b3;
};

struct Subframe1 {
    std::bitset<30> word1;
    std::bitset<30> word2;
    std::bitset<30> word3;
    std::bitset<30> word4;
    std::bitset<30> word5;
    std::bitset<30> word6;
    std::bitset<30> word7;
    std::bitset<30> word8;
    std::bitset<30> word9;
    std::bitset<30> word10;

    std::bitset<30> createWord3(int GPSWeek,int L2P,int uraIndex,int svHealth,int IODC){
        unsigned int word3raw = (((GPSWeek/1024) & 0x3FF) << 14) |
                                ((L2P & 0x3) << 12) |
                                ((uraIndex & 0xF) << 8) |
                                ((svHealth & 0x3F) << 2) | 
                                ((IODC >> 8) & 0x3);
        std::bitset<24> word3Data(word3raw);
        return create_nav_word(word3Data);
    }
    std::bitset<30> createWord4(int l2pFlag){
        unsigned int word4raw = (l2pFlag & 0x1) << 23;
        std::bitset<24> word4Data(word4raw);
        return create_nav_word(word4Data);
    }
    std::bitset<30> createWord5(){
        std::bitset<24> word5Data(0);
        return create_nav_word(word5Data);
    }
    std::bitset<30> createWord6(){
        std::bitset<24> word6Data(0);
        return create_nav_word(word6Data);
    }
    std::bitset<30> createWord7(double TGD){
        int rTGD= static_cast<int>(round(TGD/pow(2,-31)));
        unsigned int word7raw = ((rTGD) & 0xFF);
        std::bitset<24> word7Data(word7raw);
        return create_nav_word(word7Data); 
    }
    std::bitset<30> createWord8(int IODC){
        int toc = static_cast<int>(round(gps_Time/16));
        unsigned int word8raw = ((IODC & 0xFF) << 16) |
                                (toc & 0xFFFF);
        std::bitset<24> word8Data(word8raw);
        return create_nav_word(word8Data);
    }
    std::bitset<30> createWord9(double af2, double af1){
        int raf1= static_cast<int>(round(af1/pow(2,-43)));
        int raf2= static_cast<int>(round(af2/pow(2,-55)));
        unsigned int word9raw = ((raf2 & 0xFF) << 16) |
                                ((raf1 & 0xFFFF));
        std::bitset<24> word9Data(word9raw);
        return create_nav_word(word9Data);
    }
    std::bitset<30> createWord10(double af0){
        int raf0 = static_cast<int>(round(af0/pow(2,-31)));
        unsigned int word10raw = ((raf0 & 0x3FFFFF) << 2);
        std::bitset<24> word10Data(word10raw);
        return create_nav_word(word10Data);
    }

};

struct Subframe2 {
    std::bitset<30> word1;
    std::bitset<30> word2;
    std::bitset<30> word3;
    std::bitset<30> word4;
    std::bitset<30> word5;
    std::bitset<30> word6;
    std::bitset<30> word7;
    std::bitset<30> word8;
    std::bitset<30> word9;
    std::bitset<30> word10;

    std::bitset<30> createWord3(double crs,int iode){
        int rcrs = static_cast<int>(round(crs/pow(2,-5)));
        unsigned int word3raw = (((iode) & 0xFF) << 16) |
                                ((rcrs & 0xFFFF)); 
        std::bitset<24> word3Data(word3raw);
        return create_nav_word(word3Data);
    }

    std::bitset<30> createWord4(double deltaN,double M0){
        int rdeltaN = static_cast<int>(round(deltaN/pow(2,-43))); 
        int rM0 = static_cast<int>(round(M0/pow(2,-31)));
        unsigned int word4raw = ((rdeltaN & 0xFFFF) << 8) |
                                ((rM0 >> 24) & 0xFF);
        std::bitset<24> word4Data(word4raw);
        return create_nav_word(word4Data);
    }
    
    std::bitset<30> createWord5(double M0){
        int rM0 = static_cast<int>(round(M0/pow(2,-31)));
        unsigned int word5raw =  (rM0 & 0xFFFFFF) ;
        std::bitset<24> word5Data(word5raw);
        return create_nav_word(word5Data);
    }

    std::bitset<30> createWord6(double cuc,double eccentirity){
        int rcuc = static_cast<int>(round(cuc/pow(2,-29))); 
        int re = static_cast<int>(round(eccentirity/pow(2,-33)));
        unsigned int word6raw = ((rcuc & 0xFFFF) << 8) |
                                ((re >> 24) & 0xFF);
        std::bitset<24> word6Data(word6raw);
        return create_nav_word(word6Data);
    }

    std::bitset<30> createWord7(double eccentirity){
        int re = static_cast<int>(round(eccentirity/pow(2,-33)));
        unsigned int word7raw = (re & 0xFFFFFF);
        std::bitset<24> word7Data(word7raw);
        return create_nav_word(word7Data);
    }

    std::bitset<30> createWord8(double cus,double sqrtA){
        int rcus  = static_cast<int>(round(cus/pow(2,-29)));
        int rsqrtA = static_cast<int>(round(sqrtA/pow(2,-19)));
        unsigned int word8raw = ((rcus & 0XFFFF) << 8) |
                                ((rsqrtA >> 24) & 0XFF);
        std::bitset<24> word8Data(word8raw);
        return create_nav_word(word8Data);
    }

    std::bitset<30> createWord9(double sqrtA){
        int rsqrtA = static_cast<int>(round(sqrtA/pow(2,-19)));
        unsigned int word9raw = (rsqrtA & 0XFFFFFF);
        std::bitset<24> word9Data(word9raw);
        return create_nav_word(word9Data);
    }

    std::bitset<30> createWord10(int toe,int fit_i_flag = 0,int aodo = 0){
        int rtoe = (round(toe / 16.0));
        unsigned int word10raw = ((rtoe & 0xFFFF) << 8) | 
                                 (fit_i_flag & 0x1) << 7 |
                                 ((aodo & 0x1F) << 2); 
        std::bitset<24> word10Data(word10raw);
        return create_nav_word(word10Data);
    }
};

struct Subframe3 {
    std::bitset<30> word1;
    std::bitset<30> word2;
    std::bitset<30> word3;
    std::bitset<30> word4;
    std::bitset<30> word5;
    std::bitset<30> word6;
    std::bitset<30> word7;
    std::bitset<30> word8;
    std::bitset<30> word9;
    std::bitset<30> word10;

    std::bitset<30> createWord3(double cic,double omega0){
        int rcic = static_cast<int>(round(cic/pow(2,-29)));
        int romega0 = static_cast<int>(round(omega0/pow(2,-31)));
        unsigned int word3raw = ((rcic & 0XFFFF) << 8) |
                                ((romega0 >> 24 ) & 0xFF);
        std::bitset<24> word3Data(word3raw);
        return create_nav_word(word3Data);
    }

    std::bitset<30> createWord4(double omega0){
        int romega0 = static_cast<int>(round(omega0/pow(2,-31)));
        unsigned int word4raw = (romega0 & 0xFFFFFF);
        std::bitset<24> word4Data(word4raw);
        return create_nav_word(word4Data);
    }

    std::bitset<30> createWord5(double cis,double i0){
        int rcis = static_cast<int>(round(cis/pow(2,-29))); 
        int ri0 = static_cast<int>(round(ri0/pow(2,-31)));
        unsigned int word5raw = ((rcis & 0xFFFF) << 8) |
                                ((ri0 >> 24) & 0xFF);
        std::bitset<24> word5Data(word5raw);
        return create_nav_word(word5Data);
    }

    std::bitset<30> createWord6(double i0){
        int ri0 = static_cast<int>(round(i0/pow(2,-31)));
        unsigned int word6raw = ((ri0 & 0xFFFFFF));
        std::bitset<24> word6Data(word6raw);
        return create_nav_word(word6Data);
    }

    std::bitset<30> createWord7(double crc,double w){
        int rcrc = static_cast<int>(round(crc/pow(2,-5)));  
        int rw = static_cast<int>(round(w/pow(2,-31)));
        unsigned int word7raw = ((rcrc & 0xFFFF) << 8) |
                                ((rw >> 24) & 0xFF);
        std::bitset<24> word7Data(word7raw);
        return create_nav_word(word7Data);
    }

    std::bitset<30> createWord8(double w){
        int rw = static_cast<int>(round(w/pow(2,-31)));
        unsigned int word8raw = (rw & 0XFFFFFF);
        std::bitset<24> word8Data(word8raw);
        return create_nav_word(word8Data);
    }

    std::bitset<30> createWord9(double omegaDot){
        int romegaDot = static_cast<int>(round(omegaDot/pow(2,-43)));
        unsigned int word9raw = (romegaDot & 0xFFFFFF);
        std::bitset<24> word9Data(word9raw);
        return create_nav_word(word9Data);
    }

    std::bitset<30> createWord10(int iode,double idot){
        int ridot = static_cast<int>(round(idot/pow(2,-43)));
        unsigned int word10raw = ((iode & 0XFF) << 16)| 
                                  ((ridot & 0x3FFF) << 2);
        std::bitset<24> word10Data(word10raw); 
        return create_nav_word(word10Data);
    }
};

struct Subframe4 {
    std::bitset<30> word1;
    std::bitset<30> word2;
    std::bitset<30> word3;
    std::bitset<30> word4;
    std::bitset<30> word5;
    std::bitset<30> word6;
    std::bitset<30> word7;
    std::bitset<30> word8;
    std::bitset<30> word9;
    std::bitset<30> word10;

    std::bitset<30> createWord3(int dataID = 4,int pageID = 18,double a0,double a1){
        int ra0 = static_cast<int>(round(a0/pow(2,-30)));
        int ra1 = static_cast<int>(round(a1/pow(2,-27)));
        unsigned int word3raw = ((dataID & 0x02) << 22) |
                                ((pageID & 0x06 ) << 16) |
                                ((ra0 & 0xFF) << 8) |
                                ((ra1 & 0xFF));
        std::bitset<24> word3Data(word3raw);
        return create_nav_word(word3Data);
    }

    std::bitset<30> createWord4(double a2,double a3,double b0){
        int ra2 = static_cast<int>(round(a2/pow(2,-24))); 
        int ra3 = static_cast<int>(round(a3/pow(2,-24)));
        int rb0 = static_cast<int>(round(b0/pow(2,11)));
        unsigned int word4raw = ((ra2 & 0xFF) << 16) |
                                ((ra3 & 0xFF) << 8) |
                                (rb0 & 0xFF);
        std::bitset<24> word4Data(word4raw);
        return create_nav_word(word4Data);
    }

    std::bitset<30> createWord5(double b1,double b2,double b3){
        int rb1 = static_cast<int>(round(b1/pow(2,14)));
        int rb2 = static_cast<int>(round(b2/pow(2,16)));
        int rb3 = static_cast<int>(round(b3/pow(2,16)));
        unsigned int word5raw = ((rb1 & 0xFF) << 16) |
                                ((rb2 & 0xFF) << 8) |
                                (rb3 & 0xFF);
        std::bitset<24> word5Data(word5raw);
        return create_nav_word(word5Data);
    }

    std::bitset<30> createWord6(double A1){
        int rA1 = static_cast<int>(round(A1/pow(2,-50)));
        unsigned int word6raw = (rA1 & 0xFFFFFF);
        std::bitset<24> word6Data(word6raw);
        return create_nav_word(word6Data);
    }

    std::bitset<30> createWord7(double A0){
        int rA0 = static_cast<int>(round(A0/pow(2,-30)));
        unsigned int word7raw = ((rA0 >> 8) & 0xFFFFFF);
        std::bitset<24> word7Data(word7raw);
        return create_nav_word(word7Data);
    }

    std::bitset<30> createWord8(double A0,double tot,int WNt){
        int rA0 = static_cast<int>(round(A0/pow(2,-30)));
        int rtot = static_cast<int>(round(tot/pow(2,12)));
        unsigned int word8raw = ((rA0 & 0xFF) << 16) |
                                ((rtot & 0xFF) << 8) |
                                (WNt & 0xFF);
        std::bitset<24> word8Data(word8raw);
        return create_nav_word(word8Data);
    }

    std::bitset<30> createWord9(int deltatLS,int WNLSF,int DN){
        unsigned int word9raw = ((deltatLS & 0xFF) << 16) |
                                ((WNLSF & 0xFF) << 8) |
                                (DN & 0xFF);
        std::bitset<24> word9Data(word9raw);
        return create_nav_word(word9Data);
    }

    std::bitset<30> createWord10(int deltatLSF){
        unsigned int word10raw = (deltatLSF & 0xFF << 16);
        std::bitset<24> word10Data(word10raw);
        return create_nav_word(word10Data);
    }

    void page18(int dataID = 4,int pageID = 18,UTC utc, Ionespher ion){
        word3 = createWord3(dataID,pageID,ion.a0,ion.a1);
        word4 = createWord4(ion.a2,ion.a3,ion.b0);
        word5 = createWord5(ion.b1,ion.b2,ion.b3);
        word6 = createWord6(utc.A1);
        word7 = createWord7(utc.A0);
        word8 = createWord8(utc.A0,utc.tot,utc.WNt);
        word9 = createWord9(utc.deltatLS,utc.WNLSF,utc.DN);
        word10 = createWord10(utc.deltatLSF);
    }

};

//Almanac Data 
struct Subframe5 {
    std::bitset<30> word1;
    std::bitset<30> word2;
    std::bitset<30> word3;
    std::bitset<30> word4;
    std::bitset<30> word5;
    std::bitset<30> word6;
    std::bitset<30> word7;
    std::bitset<30> word8;
    std::bitset<30> word9;
    std::bitset<30> word10;

    std::bitset<30> createWord3(int dataID = 5,int svID,double eccentrity){
        int re = static_cast<int>(round(eccentrity/pow(2,-21)));
        unsigned int word3raw = ((dataID & 0x02) << 22) |
                                ((svID & 0x06 ) << 16) |
                                (re & 0xFFFF);
        std::bitset<24> word3Data(word3raw);
        return create_nav_word(word3Data);
    }

    std::bitset<30> createWord4(double toa,double alfai){
        int rtoa = static_cast<int>(round(toa/pow(2,12)));
        int ralfai = static_cast<int>(round(alfai/pow(2,-19)));
        unsigned int word4raw = ((rtoa & 0xFF) << 16) | 
                                (ralfai & 0xFFFF);
        std::bitset<24> word4Data(word4raw);
        return create_nav_word(word4Data);
    }

    std::bitset<30> createWord5(double omegaDOT,int svHealth){
        int romegaDOT = static_cast<int>(round(omegaDOT/pow(2,-38)));
        unsigned int word5raw = ((romegaDOT & 0xFFFF) << 8) | 
                                (svHealth & 0xFF);
        std::bitset<24> word5Data(word5raw);
        return create_nav_word(word5Data);
    }

    std::bitset<30> createWord6(double sqrtA){
        int rsqrtA = static_cast<int>(round(sqrtA/pow(2,-11)));
        unsigned int word6raw = (rsqrtA & 0xFFFFFF);
        std::bitset<24> word6Data(word6raw);
        return create_nav_word(word6Data);
    }

    std::bitset<30> createWord7(double omega0){
        int romega0 = static_cast<int>(round(omega0/pow(2,-23)));
        unsigned int word7raw = (romega0 & 0xFFFFFF);
        std::bitset<24> word7Data(word7raw);
        return create_nav_word(word7Data);
    }

    std::bitset<30> createWord8(double w){
        int rw = static_cast<int>(round(w/pow(2,-23)));
        unsigned int word8raw = (rw & 0xFFFFFF);
        std::bitset<24> word8Data(word8raw);
        return create_nav_word(word8Data);
    }

    std::bitset<30> createWord9(double M0){
        int rM0 = static_cast<int>(round(M0/pow(2,-23)));
        unsigned int word9raw = (rM0 & 0xFFFFFF);
        std::bitset<24> word9Data(word9raw);
        return create_nav_word(word9Data);
    }

    std::bitset<30> createWord10(double af0,double af1){
        int raf0 = static_cast<int>(round(af0/pow(2,-20)));
        int raf1 = static_cast<int>(round(af1/pow(2,-38)));
        unsigned int word10raw = (((raf0 >> 3) & 0xFF) << 16) |
                                    ((raf1 & 0x0B) << 5) | 
                                    (raf0 & 0x03);
        std::bitset<24> word10Data(word10raw);
        return create_nav_word(word10Data);
    }
};
