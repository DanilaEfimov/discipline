#include "../../include/pl/vm/vm.hpp"
#include "../../include/pl/vm/runtime.hpp"
#include "../../include/pl/vm/version.h"
#include "../../include/pl/vm/loading.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  po::options_description visible("Allowed options");
  visible.add_options()
    ("help,h",    "produce help message")
    ("version,v", "print version")
  ;

  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("modules", po::value<std::vector<std::string>>()->required(),
     "module files")
  ;

  po::options_description all;
  all.add(visible).add(hidden);

  po::positional_options_description pos;
  pos.add("modules", -1); 

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv)
                .options(all)
                .positional(pos)
                .run(), vm);
    po::notify(vm);
  } catch (const po::error& e) {
    std::cerr << "error: " << e.what() << "\n\n" << visible << "\n";
    return 1;
  }

  if (vm.count("help")) {
    std::cout << "usage: plvm [options] <module> [<module>...]\n\n"
              << visible << "\n";
    return 0;
  }
  if (vm.count("version")) {
    std::cout << PLVM_VERSION_STRING << "\n" 
            << PLVM_BUILD_INFO << " " << plvm_build_date();
    return 0;
  }

  const auto paths = vm["modules"].as<std::vector<std::string>>();

  std::vector<pl::Module> modules = pl::load_modules(paths);

  pl::VM machine(modules, std::cout, std::cin);
  auto result = machine.run();

  if (result.code != pl::ExCode::Success) {
    std::cerr << result.error << "\n";
    return static_cast<int>(result.code);
  }
  return 0;
}
