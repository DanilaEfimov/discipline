#define PLVM_STRINGIFY_(x) #x
#define PLVM_STRINGIFY(x) PLVM_STRINGIFY_(x)

#if defined(__clang__)
#define PLVM_COMPILER "clang " __clang_version__
#elif defined(__GNUC__)
#define PLVM_COMPILER "GCC " __VERSION__
#elif defined(_MSC_VER)
#define PLVM_COMPILER "MSVC " PLVM_STRINGIFY(_MSC_VER)
#else
#define PLVM_COMPILER "unknown compiler"
#endif

#define PLVM_VERSION_STRING "0.1.0"
#define PLVM_BUILD_INFO PLVM_COMPILER

inline const char* plvm_build_date() { return __DATE__; } 
