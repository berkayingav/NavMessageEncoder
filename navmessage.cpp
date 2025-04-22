#include "navmessage.hpp"
#include "rinexParsercpp.hpp"
#include "almanactxtParser.hpp"

int secWeek;
UTC utcconst;
Ionespher ionconst;
Almanac alman;

void secofweek(int gpsWeek, int year, int month, int day, int hour, int min, int sec) {
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
    secWeek =  gps_Time % 604800;  // seconds_in_week
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

std::bitset<24> build_tlm_data() {
    return std::bitset<24>(std::string("100010110000000000000000"));
}

std::bitset<30> create_nav_word(std::bitset<24> data) {
    std::bitset<6> parity = compute_parity(data);
    unsigned long long combined = (data.to_ulong() << 6) | parity.to_ulong();
    return std::bitset<30>(combined);
}

std::bitset<30> create_nav_word_how(std::bitset<24> base_data) {
    for (int i = 0; i < 4; ++i) {  // 2 bit brute force → 0 to 3
        std::bitset<24> test_data = base_data;
        test_data[23] = (i >> 1) & 1;  // bit 23 (MSB side)
        test_data[22] = i & 1;         // bit 24

        std::bitset<6> parity = compute_parity(test_data);
        std::bitset<30> full_word = (test_data.to_ulong() << 6) | parity.to_ulong();

        // parity bits 29,30 (index 1, 0) == 0 mı?
        if (full_word[1] == 0 && full_word[0] == 0) {
            return full_word;
        }
    }

    // Hiçbiri sağlamazsa fallback (çok düşük olasılık)
    return std::bitset<30>(0);
}

std::bitset<30> createHow(int tow,int subframeID){
    int zCount = tow/6;
    zCount = zCount & 0x1FFFF;
    unsigned int howRaw = (zCount << 7) |
                          (0 << 6) |
                          (0 << 5) |
                          (subframeID & 0x07);
    std::bitset<24> howData(howRaw);
    return create_nav_word_how(howData);
}


std::bitset<30> create_tlm(){
    std::bitset<24> word1Data(build_tlm_data());
    return create_nav_word(word1Data);
}

std::bitset<300> create_subframe(std::bitset<30> words[10]){
    std::bitset<300> subframe;
    for (int i=0;i < 10;i++) {
        for(int b = 0; b<30; ++b){
            subframe[i*30+b] = words[i][b];
        }
    }
    return subframe;
}

int uraIndex(double sv_accuracy) {
    const double thresholds[] = {2.4, 3.4, 4.85, 6.85, 9.65, 13.65, 24.0, 48.0};
    for (int i = 0; i < 8; ++i) {
        if (sv_accuracy < thresholds[i]) 
        return i;
    }
    return 8; 
}

std::bitset<300> fillSubframe1(const EphemerisBlock& eph){
    Subframe1 sf1;
    sf1.word1 = create_tlm();
    sf1.word2 = createHow(eph.orbit7.TTM + 6,1);
    sf1.word3 = sf1.createWord3(eph.orbit5.GPSWeek,eph.orbit5.L2P,uraIndex(eph.orbit6.SVAccuracy),eph.orbit6.SVHealth,eph.orbit6.IODC);
    sf1.word4 = sf1.createWord4(eph.orbit5.L2P);
    sf1.word5 = sf1.createWord5();
    sf1.word6 = sf1.createWord6();
    sf1.word7 = sf1.createWord7(eph.orbit6.TGD);
    sf1.word8 = sf1.createWord8(eph.orbit6.IODC);
    sf1.word9 = sf1.createWord9(eph.header.SV_Clock_Drift_Rate,eph.header.SV_Clock_Drift);
    sf1.word10 = sf1.createWord10(eph.header.SV_Clock_Bias);
    std::bitset<30> words[10] = {sf1.word1,sf1.word2,sf1.word3,sf1.word4,sf1.word5,sf1.word6,sf1.word7,sf1.word8,sf1.word9,sf1.word10};
    return create_subframe(words);
}

std::bitset<300> fillSubframe2(const EphemerisBlock& eph){
    Subframe2 sf2;
    sf2.word1 = create_tlm();
    sf2.word2 = createHow(eph.orbit7.TTM + 12,2);
    sf2.word3 = sf2.createWord3(eph.orbit1.Crs,eph.orbit1.IODE);
    sf2.word4 = sf2.createWord4(eph.orbit1.DeltaN,eph.orbit1.M0);
    sf2.word5 = sf2.createWord5(eph.orbit1.M0);
    sf2.word6 = sf2.createWord6(eph.orbit2.Cuc,eph.orbit2.e);
    sf2.word7 = sf2.createWord7(eph.orbit2.e);
    sf2.word8 = sf2.createWord8(eph.orbit2.Cus,eph.orbit2.sqrtA);
    sf2.word9 = sf2.createWord9(eph.orbit2.sqrtA);
    sf2.word10 = sf2.createWord10(eph.orbit3.Toe);
    std::bitset<30> words[10] = {sf2.word1,sf2.word2,sf2.word3,sf2.word4,sf2.word5,sf2.word6,sf2.word7,sf2.word8,sf2.word9,sf2.word10};
    return create_subframe(words);
}

std::bitset<300> fillSubframe3(const EphemerisBlock& eph){
    Subframe3 sf3;
    sf3.word1 = create_tlm();
    sf3.word2 = createHow(eph.orbit7.TTM + 18,3);
    sf3.word3 = sf3.createWord3(eph.orbit3.Cic,eph.orbit3.OMEGA);
    sf3.word4 = sf3.createWord4(eph.orbit3.OMEGA);
    sf3.word5 = sf3.createWord5(eph.orbit3.Cis,eph.orbit4.i0);
    sf3.word6 = sf3.createWord6(eph.orbit4.i0);
    sf3.word7 = sf3.createWord7(eph.orbit4.Crc,eph.orbit4.omega);
    sf3.word8 = sf3.createWord8(eph.orbit4.omega);
    sf3.word9 = sf3.createWord9(eph.orbit4.OMEGA_DOT);
    sf3.word10 = sf3.createWord10(eph.orbit1.IODE,eph.orbit5.IDOT);
    std::bitset<30> words[10] = {sf3.word1,sf3.word2,sf3.word3,sf3.word4,sf3.word5,sf3.word6,sf3.word7,sf3.word8,sf3.word9,sf3.word10};
    return create_subframe(words);
}

std::bitset<300> fillSubframe4(int pageNum){
    Subframe4 sf4;
    if (pageNum == 18){
        utcconst.A0 = 0;
        utcconst.A1 = 0;
        utcconst.deltatLS = 18;
        utcconst.deltatLSF = 18;
        utcconst.WNt = 0;
        utcconst.WNLSF = 0;
        utcconst.DN = 0;
        utcconst.tot = 0;
        ionconst.a0 = 0xE0;
        ionconst.a1 = 0x00;
        ionconst.a2 = 0xF0;
        ionconst.a3 = 0x00;
        ionconst.b0 = 0x90;
        ionconst.b1 = 0x00;
        ionconst.b2 = 0xA0;
        ionconst.b3 = 0x00;
        sf4.word1 = create_tlm();
        sf4.word2 = createHow();
        sf4.page18(4,18,utcconst,ionconst);
    }
    sf4.reserved_page(pageNum);
    sf4.word1 = create_tlm();
    sf4.word2 = createHow();
}

std::vector<int> almanac25Datas;

std::bitset<300> fillSubframe5(const AlmanacSet& alm,int pageNum){
    Subframe5 sf5;
    sf5.word1 = create_tlm();
    sf5.word2 = createHow();
    sf5.word3 = sf5.createWord3(5,pageNum,alm.e);
    sf5.word4 = sf5.createWord4(alm.toa,alm.orbitalinc);
    sf5.word5 = sf5.createWord5(alm.omegaDOT,alm.health);
    sf5.word6 = sf5.createWord6(alm.sqrtA);
    sf5.word7 = sf5.createWord7(alm.omega0);
    sf5.word8 = sf5.createWord8(alm.omega);
    sf5.word9 = sf5.createWord9(alm.M0);
    sf5.word10 = sf5.createWord10(alm.af0,alm.af1);
    if(pageNum == 1){
        almanac25Datas.push_back(alm.toa);
        almanac25Datas.push_back(alm.week);
    }
    almanac25Datas.push_back(alm.health);

    if(pageNum == 25){
        sf5.word1 = create_tlm();
        sf5.word2 = createHow();
        sf5.page25(5,25,almanac25Datas);
    }
}






