#include <fstream>
#include "json.hpp"
#include "iostream"
using json = nlohmann::json;

int main(int argc, char *argv[]) {

    std::ifstream f(argv[1]);
    json data = json::parse(f);
    std::string name = data["zygote32_Inject_So"];
    std::string name2 = data["zygote32_Inject_So"];
    std::cout <<name.c_str() <<std::endl;
    std::cout <<name2.c_str() <<std::endl;
    return 0;
}