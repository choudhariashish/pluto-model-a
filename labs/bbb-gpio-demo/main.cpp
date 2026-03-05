#include "bbb_pin_map.h"
#include <iostream>
#include <string>
#include <vector>

using namespace bbb;

int cmd_help(const std::vector<std::string>& args) {
    PinMap::PrintHelp();
    return 0;
}

int cmd_pinmap(const std::vector<std::string>& args) {
    PinMap::PrintPinMap();
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cmd_help({});
        return 1;
    }
    
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    std::string command = args[1];
    
    if (command == "help") {
        return cmd_help(args);
    } else if (command == "pinmap") {
        return cmd_pinmap(args);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        cmd_help(args);
        return 1;
    }
    
    return 0;
}
