#include "rinexParsercpp.hpp"
#include "json.hpp" 
using json = nlohmann::json;


std::vector<std::string> split_neg_num(const std::string& input, size_t start_index) {
    std::vector<std::string> result;
    size_t index_minus = input.find('-', start_index);

    if (index_minus != std::string::npos && index_minus > 0 && !isalpha(input[index_minus - 1])) {
        std::string num1 = input.substr(0, index_minus);
        std::string num2 = input.substr(index_minus);

        auto fix1 = split_neg_num(num1);
        auto fix2 = split_neg_num(num2);

        if (!fix1.empty()) result.insert(result.end(), fix1.begin(), fix1.end());
        else result.push_back(num1);

        if (!fix2.empty()) result.insert(result.end(), fix2.begin(), fix2.end());
        else result.push_back(num2);

        return result;
    }

    if (index_minus != std::string::npos)
        return split_neg_num(input, index_minus + 1);

    return {};
}

std::vector<std::string> fix_negative_nums(const std::vector<std::string>& nums) {
    std::vector<std::string> fixed;
    for (const auto& num : nums) {
        auto sub_fixed = split_neg_num(num);
        if (!sub_fixed.empty())
            fixed.insert(fixed.end(), sub_fixed.begin(), sub_fixed.end());
        else
            fixed.push_back(num);
    }
    return fixed;
}

int skip_header(std::ifstream& file) {
    std::string line;
    int line_count = 0;

    while (std::getline(file, line)) {
        line_count++;
        if (line.find("END") != std::string::npos)
            break;
    }

    return line_count;
}

prnSvClock parse_PRN_EPOCH_SV_CLK(const std::vector<std::string>& nums) {
    if (nums.size() != 10)
        throw std::runtime_error("PRN_EPOCH_SV_CLK parse error");

    prnSvClock result;
    result.PRN = nums[0];
    result.year = std::stoi(nums[1]);
    result.month = std::stoi(nums[2]);   //stoi -> string to integer
    result.day = std::stoi(nums[3]);    //stod -> string to double 
    result.hour = std::stoi(nums[4]);
    result.minute = std::stoi(nums[5]);
    result.second = std::stod(nums[6]);
    result.SV_Clock_Bias = std::stod(nums[7]);
    result.SV_Clock_Drift = std::stod(nums[8]);
    result.SV_Clock_Drift_Rate = std::stod(nums[9]);

    return result;
}

broadcastOrbit1 parse_BROADCAST_ORBIT_1(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_1 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit2 parse_BROADCAST_ORBIT_2(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_2 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit3 parse_BROADCAST_ORBIT_3(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_3 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit4 parse_BROADCAST_ORBIT_4(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_4 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit5 parse_BROADCAST_ORBIT_5(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_5 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit6 parse_BROADCAST_ORBIT_6(const std::vector<std::string>& nums) {
    if (nums.size() != 4)
        throw std::runtime_error("BROADCAST_ORBIT_6 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1]),
        std::stod(nums[2]),
        std::stod(nums[3])
    };
}

broadcastOrbit7 parse_BROADCAST_ORBIT_7(const std::vector<std::string>& nums) {
    if (nums.size() != 2)
        throw std::runtime_error("BROADCAST_ORBIT_7 parse error");

    return {
        std::stod(nums[0]),
        std::stod(nums[1])
    };
}

std::vector<std::string> next_line(std::ifstream& file) {
    std::string line;
    if (!std::getline(file, line) || line.empty())
        throw std::runtime_error("End of file or empty line");

    // D → e dönüşümü
    std::replace(line.begin(), line.end(), 'D', 'e');

    // Satırı kelimelere ayır
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token)
        tokens.push_back(token);

    // Negatifleri düzelt
    return fix_negative_nums(tokens);
}

std::map<std::string,EphemerisBlock> extract_data(std::ifstream& file){
    std::map<std::string,EphemerisBlock> all_data;
    int satCount = 0;

    while (true) {
        try {
            std::vector<std::vector<std::string>> lines;  // Matrix , iç içe vektör 
            for (int i = 0; i < 8; ++i)
                lines.push_back(next_line(file));
            
            prnSvClock header = parse_PRN_EPOCH_SV_CLK(lines[0]);
            std::string key = std::to_string(satCount) + "_" + header.PRN;

            EphemerisBlock block;
            block.header = header;
            block.orbit1 = parse_BROADCAST_ORBIT_1(lines[1]);
            block.orbit2 = parse_BROADCAST_ORBIT_2(lines[2]);
            block.orbit3 = parse_BROADCAST_ORBIT_3(lines[3]);
            block.orbit4 = parse_BROADCAST_ORBIT_4(lines[4]);
            block.orbit5 = parse_BROADCAST_ORBIT_5(lines[5]);
            block.orbit6 = parse_BROADCAST_ORBIT_6(lines[6]);
            block.orbit7 = parse_BROADCAST_ORBIT_7(lines[7]);

            all_data[key] = block;
            satCount++;
        }
        catch (const std::runtime_error& e){
            break;
        }
    }
    return all_data;
}

std::map<std::string, EphemerisBlock> read_rinex(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file");

    skip_header(file);
    return extract_data(file);
}

void jsonOut(const std::map<std::string,EphemerisBlock>& data,int year,int month,int day,int hour,int minute,const std::string& file_name){
    json j_array = json::array();
    const int short_year = year % 100;

    for (const auto& [key, eph] : data) {
        if (eph.header.year == short_year &&
            eph.header.month == month &&
            eph.header.day == day &&
            eph.header.hour == hour &&
            eph.header.minute == minute) {

                json j = json::object({
                    {"Epoch", {
                        {"-PRN",eph.header.PRN},
                        {"Year", eph.header.year},
                        {"Month", eph.header.month},
                        {"Day", eph.header.day},
                        {"Hour", eph.header.hour},
                        {"Minute", eph.header.minute},
                        {"Second", eph.header.second}
                    }},
                    {"SV_Clock", {
                        {"Bias", eph.header.SV_Clock_Bias},
                        {"Drift", eph.header.SV_Clock_Drift},
                        {"DriftRate", eph.header.SV_Clock_Drift_Rate}
                    }},
                    {"Orbit1", {
                        {"IODE", eph.orbit1.IODE},
                        {"Crs", eph.orbit1.Crs},
                        {"DeltaN", eph.orbit1.DeltaN},
                        {"M0", eph.orbit1.M0}
                    }},
                    {"Orbit2", {
                        {"Cuc", eph.orbit2.Cuc},
                        {"e", eph.orbit2.e},
                        {"Cus", eph.orbit2.Cus},
                        {"sqrtA", eph.orbit2.sqrtA}
                    }},
                    {"Orbit3", {
                        {"Toe", eph.orbit3.Toe},
                        {"Cic", eph.orbit3.Cic},
                        {"OMEGA", eph.orbit3.OMEGA},
                        {"Cis", eph.orbit3.Cis}
                    }},
                    {"Orbit4", {
                        {"i0", eph.orbit4.i0},
                        {"Crc", eph.orbit4.Crc},
                        {"omega", eph.orbit4.omega},
                        {"OMEGA_DOT", eph.orbit4.OMEGA_DOT}
                    }},
                    {"Orbit5", {
                        {"IDOT", eph.orbit5.IDOT},
                        {"CodesL2Channel", eph.orbit5.CodesL2Channel},
                        {"GPSWeek", eph.orbit5.GPSWeek},
                        {"L2P", eph.orbit5.L2P}
                    }},
                    {"Orbit6", {
                        {"SVAccuracy", eph.orbit6.SVAccuracy},
                        {"SVHealth", eph.orbit6.SVHealth},
                        {"TGD", eph.orbit6.TGD},
                        {"IODC", eph.orbit6.IODC}
                    }},
                    {"Orbit7", {
                        {"TTM", eph.orbit7.TTM},
                        {"FitInterval", eph.orbit7.fitInterval}
                    }}
                });

            j_array.push_back(j);
        }
    }

    std::ofstream out(file_name);
    if (!out) {
        throw std::runtime_error("JSON dosyası oluşturulamadı: " + file_name);
    }
    out << std::setw(4) << j_array;
    std::cout << "Başarıyla kaydedildi: " << file_name << " (" 
              << j_array.size() << " uydu)\n";

}

int main() {
    auto data = read_rinex("nav_211.nav");
    jsonOut(data,2025,4,1,0,0,"output.json");

    return 0;
}




