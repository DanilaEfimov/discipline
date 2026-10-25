#pragma  once

#include "excode.hpp"
#include "runtime.hpp"


namespace pl {

class VM {
    Runtime runtime;

public:
    VM(Runtime rt) : runtime(std::move(rt)) {}

    ExCode run() {
        return this->runtime.run();
    }
    
};

} // namespace pl
