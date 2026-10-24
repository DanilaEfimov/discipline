#pragma  once

#include "runtime.hpp"


namespace pl {

class VM {
    Runtime runtime;

public:
    VM(Runtime rt) : runtime(std::move(rt)) {}

    int run() {
        return this->runtime.run();
    }
    
};

} // namespace pl
