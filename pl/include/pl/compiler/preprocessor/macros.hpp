#pragma  once 

#include <string>
#include <vector>
#include <stdexcept>

namespace plc {

enum class MacroType {
    Plain,
    Parameter,
};

template<MacroType T>
struct Macro {

};

template<>
struct Macro<MacroType::Parameter> {

};

} // namespace plc
