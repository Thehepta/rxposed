#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

int main(int argc, char *argv[]) {

    std::ifstream f(argv[4]);
    json data = json::parse(f);
    std::string name = data["libc_path"];
    printf("%s",name.c_str());
    return 0;
}