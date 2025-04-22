#include "almanactxtParser.hpp"

std::vector<AlmanacSet> parse_almanac_txt(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Almanac dosyası açılamadı.");

    std::vector<AlmanacSet> list;
    std::string line;
    AlmanacSet current;

    while (std::getline(file, line)) {
        if (line.find("********") != std::string::npos) continue;

        if (line.find("ID:") != std::string::npos)
            current.PRN = std::stoi(line.substr(29));
        else if (line.find("Health:") != std::string::npos)
            current.health = std::stoi(line.substr(29));
        else if (line.find("Eccentricity:") != std::string::npos)
            current.e = std::stod(line.substr(29));
        else if (line.find("Time of Applicability(s)") != std::string::npos)
            current.toa = std::stod(line.substr(29));
        else if (line.find("Orbital Inclination(rad)") != std::string::npos)
            current.toa = std::stod(line.substr(29));
        else if (line.find("SQRT(A)") != std::string::npos)
            current.sqrtA = std::stod(line.substr(29));
        else if (line.find("Argument of Perigee") != std::string::npos)
            current.omega = std::stod(line.substr(29));
        else if (line.find("Rate of Right Ascen") != std::string::npos)
            current.omegaDOT = std::stod(line.substr(29));
        else if (line.find("Right Ascen at Week") != std::string::npos)
            current.omega0 = std::stod(line.substr(29));
        else if (line.find("Mean Anom") != std::string::npos)
            current.M0 = std::stod(line.substr(29));
        else if (line.find("Af0") != std::string::npos)
            current.af0 = std::stod(line.substr(29));
        else if (line.find("Af1") != std::string::npos)
            current.af1 = std::stod(line.substr(29));
        else if (line.find("week:") != std::string::npos) {
            current.week = std::stoi(line.substr(29));
            list.push_back(current); // PRN tamamlandı → ekle
            current = AlmanacSet();  // new AlmanacSet object
        }
    }

    return list;
}

