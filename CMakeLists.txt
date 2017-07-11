cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# Force external build
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT WIN32)
    message(FATAL_ERROR "You can not use CMake to build from the root of it's source tree! Remove the CMakeCache.txt file from this directory, then create a separate directory (either below this directory or elsewhere), and then re-run CMake from there.")
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT WIN32)

# Project version
SET(PROJECT_MAJOR_VERSION 1)
SET(PROJECT_MINOR_VERSION 0)
SET(PROJECT_PATCH_LEVEL 0)

# Cmake module path (if exists)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
# include our function checks
include(systemfuncs)

# Get the git revision location for the branch we're on
if(EXISTS "${CMAKE_SOURCE_DIR}/.git/HEAD")
        file(READ ${CMAKE_SOURCE_DIR}/.git/HEAD GIT_HEAD_LOC)
        string(LENGTH ${GIT_HEAD_LOC} HEAD_LEN)
        math(EXPR LEN "${HEAD_LEN} - 5")
        string(SUBSTRING ${GIT_HEAD_LOC} 5 ${LEN} GIT_HEAD)
        # Weird nastery to remove newlines which screw up the if statement below.
        set(GIT_SHA_PATH "${CMAKE_SOURCE_DIR}/.git/${GIT_HEAD}")
        string(REGEX REPLACE "(\r?\n)+$" "" GIT_SHA_PATH "${GIT_SHA_PATH}")
endif(EXISTS "${CMAKE_SOURCE_DIR}/.git/HEAD")

# Get the git revision we're on for the version string
if(EXISTS "${GIT_SHA_PATH}")
        file(READ "${GIT_SHA_PATH}" VERSION_STR)
        string(REGEX REPLACE "(\r?\n)+$" "" VERSION_STR "${VERSION_STR}")
        # Get the length of the string
        string(LENGTH ${VERSION_STR} VERSION_LEN)
        # Subtract 7 from the string's length
        math(EXPR VERSION_NUM_LEN "${VERSION_LEN} - ${VERSION_LEN} + 7")
        # Extract the value from the string
        string(SUBSTRING ${VERSION_STR} 0 ${VERSION_NUM_LEN} VERSION_GIT)
endif(EXISTS "${GIT_SHA_PATH}")

# Set our version strings
SET(VERSION_SIMPLE "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_LEVEL}")
SET(VERSION_FULL "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_LEVEL}-${VERSION_GIT}")

message(STATUS "${PROJECT_NAME} version: ${VERSION_FULL}")

file(GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
list(SORT SOURCE_FILES)

# Find clang (if the system has it)
find_program(CLANG clang)
find_program(CLANGXX clang++)

# prefer clang instead of gcc (or whatever shit compiler they're using) This can be disabled with
# the NO_CLANG option
option(NO_CLANG "Don't prefer clang for compilation" OFF)
option(CLANG_LIBCPP "Use of libc++ instead of glibc++" ON)
if (NOT NO_CLANG)
	if (CLANG)
		set(CMAKE_C_COMPILER ${CLANG})
	endif (CLANG)

	# In case we need to compile C++ in the future (which I doubt we will)
	if (CLANGXX)
		set(CMAKE_CXX_COMPILER ${CLANGXX})
	endif (CLANGXX)
endif (NOT NO_CLANG)

# Finally initialize our project
project(BottyMcBotFace CXX)
enable_language(C)

include (CheckCXXCompilerFlag)

# Make sure the compiler accepts C11
#check_cxx_compiler_flag(-std=c++11 HAVE_C11_FLAG)
check_cxx_compiler_flag(-std=c++1z HAVE_CXX1Z_FLAG)
set(CFLAGS "${CFLAGS} -g -Werror=implicit-function-declaration -Wall -Wextra -Wno-unused-parameter")

if (CLANG_LIBCPP)
    # Find Libc++ and set appropriate variables
    find_package(LIBCXX REQUIRED)
    include_directories(${LIBCXX_INCLUDE_DIR})
    #target_link_libraries(${LIBCXX_LIBRARY})
    set(CFLAGS "${CFLAGS} ${LIBCXX_OPTIONS}")
endif (CLANG_LIBCPP)

if (NOT HAVE_CXX1Z_FLAG)
	message(FATAL_ERROR "Your compiler (${CMAKE_C_COMPILER}) MUST support C++17. Try using CXX=<alternative compiler>")
else (NOT HAVE_CXX1Z_FLAG)
	set(CFLAGS "${CFLAGS} -std=c++1z")
endif (NOT HAVE_CXX1Z_FLAG)

# Add some feature test macro definitions.
add_definitions(-D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=200809L)

set(CMAKE_CXX_FLAGS ${CFLAGS})

# Check for platform-specific things we need
include (CheckTypeSize)
include (CheckIncludeFile)
include (CheckLibraryExists)
include (CheckFunctionExists)
include (CheckCXXSourceCompiles)

check_type_size(int8_t HAVE_INT8_T)
check_type_size(uint8_t HAVE_UINT8_T)
check_type_size(int16_t HAVE_INT16_T)
check_type_size(uint16_t HAVE_UINT16_T)
check_type_size(uint32_t HAVE_UINT32_T)
check_type_size(int64_t HAVE_INT64_T)
check_type_size(uint64_t HAVE_UINT64_T)
check_type_size("long long" HAVE_LONG_LONG)
check_type_size("unsigned long long" HAVE_UNSIGNED_LONG_LONG)
check_type_size(size_t HAVE_SIZE_T)
check_type_size(time_t HAVE_TIME_T)
check_type_size("long double" HAVE_LONG_DOUBLE)

check_type_size(u_int8_t HAVE_U_INT8_T)
check_type_size(u_int16_t HAVE_U_INT16_T)
check_type_size(u_int32_t HAVE_U_INT32_T)
check_type_size(u_int64_t HAVE_U_INT64_T)

check_function_exists(strnlen HAVE_STRNLEN)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(select HAVE_SELECT)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(poll HAVE_POLL)
check_function_exists(wcsftime HAVE_WCSFTIME)
check_function_exists(stat HAVE_STAT)
check_function_exists(realpath HAVE_REALPATH)
check_function_exists(readlink HAVE_READLINK)
check_function_exists(dirname HAVE_DIRNAME)
check_function_exists(basename HAVE_BASENAME)
check_function_exists(dlopen HAVE_DLOPEN)
check_function_exists(dlclose HAVE_DLCLOSE)
check_function_exists(dlsym HAVE_DLSYM)
check_function_exists(dlerror HAVE_DLERROR)
check_function_exists(signal HAVE_SIGNAL)
check_function_exists(uname HAVE_UNAME)
check_function_exists(backtrace HAVE_BACKTRACE)
check_function_exists(backtrace_symbols HAVE_BACKTRACE_SYMBOLS)
check_function_exists(setjmp HAVE_SETJMP)
check_function_exists(longjmp HAVE_LONGJMP)
check_function_exists(inet_ntop HAVE_INET_NTOP)
check_function_exists(inet_pton HAVE_INET_PTON)
check_function_exists(ntohs HAVE_NTOHS)
check_function_exists(ntohl HAVE_NTOHL)
check_function_exists(bind HAVE_BIND)
check_function_exists(accept HAVE_ACCEPT)
check_function_exists(connect HAVE_CONNECT)
check_function_exists(close HAVE_CLOSE)
check_function_exists(listen HAVE_LISTEN)
check_function_exists(send HAVE_SEND)
check_function_exists(recv HAVE_RECV)
check_function_exists(getsockopt HAVE_GETSOCKOPT)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(wcstombs HAVE_WCSTOMBS)
check_function_exists(wcslen HAVE_WCSLEN)

check_include_file(sys/epoll.h HAVE_SYS_EPOLL_H)
check_include_file(setjmp.h HAVE_SETJMP_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(linux/limits.h HAVE_LINUX_LIMITS_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(sys/utsname.h HAVE_SYS_UTSNAME_H)
check_include_file(execinfo.h HAVE_EXECINFO_H)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)

# check for c++ abi, ussually present in GNU compilers
# Because there is a bug in check_include_file, we must
# use check_cxx_source_compiles instead.
check_cxx_source_compiles(
  "#include <cxxabi.h>
  int main(int argc, char* argv[])
  { char * type; int status;
	char * r = abi::__cxa_demangle(type, 0, 0, &status);
	return 0;
  }"
  HAS_CXXABI_H)

# Find the path for json.hpp. Arch linux stores it in /usr/include/nlohmann/json.hpp
# but Fedora stores it in /usr/include/json.hpp.
#find_path(JSON_INCLUDE json.hpp)
#if (NOT JSON_INCLUDE)
#    find_path(JSON_INCLUDE2 nlohmann/json.hpp)
#    if (NOT JSON_INCLUDE2)
#        message(FATAL_ERROR "Cannot find nlohmann's json.hpp. Please install it from here: https://github.com/nlohmann/json")
#    else (NOT JSON_INCLUDE2)
#        message(STATUS "Found Niels Lohmann's JSON: nlohmann/json.hpp")
#        set(JSON_INCLUDE nlohmann/json.hpp)
#    endif (NOT JSON_INCLUDE2)
#else (NOT JSON_INCLUDE)
#    message(STATUS "Found Niels Lohmann's JSON: json.hpp")
#    set(JSON_INCLUDE json.hpp)
#endif (NOT JSON_INCLUDE)

#find_package(FLEX REQUIRED)
#find_package(BISON REQUIRED)

# Check flex version to prevent yylex_destroy link errors.
#if(${FLEX_VERSION} VERSION_LESS "2.5.9")
#        message(FATAL_ERROR "Your flex version is ${FLEX_VERSION}, You must have flex version 2.5.9 or higher!")
#endif(${FLEX_VERSION} VERSION_LESS "2.5.9")

# Generate our flex and bison targets.
#FLEX_TARGET(LEXER ${CMAKE_SOURCE_DIR}/src/scanner.ll ${CMAKE_BINARY_DIR}/lexer.cpp)
#BISON_TARGET(PARSER ${CMAKE_SOURCE_DIR}/src/parser.yy ${CMAKE_BINARY_DIR}/parser.cpp)
#ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

# Make sure if the platform we're on requires libdl that we use it.
find_library(LIBDL dl)
find_library(LIBPTHREAD pthread)
find_library(CLANG_CXXABI c++abi)
#find_library(DOCOPT docopt)

# Add our include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${MYSQL_INCLUDE_DIR} ${FastCGI_INCLUDE_DIR})

# Configuration file and include the binary directory
configure_file(${CMAKE_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_BINARY_DIR}/sysconf.h)
include_directories(${CMAKE_BINARY_DIR})

# set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/LICENSE.h")
# find_program(XXD xxd REQUIRED)
#
# add_custom_command(
#     OUTPUT ${OUTPUT_FILE}
#     COMMAND ${XXD} -i LICENSE ${OUTPUT_FILE}
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/
#     COMMENT "Generating binary license header")
#
# add_custom_target(
#     license_headers
#     ALL DEPENDS
#     ${OUTPUT_FILE})

#find_program(GZIP gzip)

# Finally, tell CMake how to build the project
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${FLEX_LEXER_OUTPUTS} ${BISON_PARSER_OUTPUTS})
#set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CFLAGS} -DPROJ_CORE")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${LINKFLAGS}")
#set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
#set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if (LIBDL)
	target_link_libraries(${PROJECT_NAME} ${LIBDL})
endif (LIBDL)
if (LIBPTHREAD)
	target_link_libraries(${PROJECT_NAME} ${LIBPTHREAD})
endif (LIBPTHREAD)
#if(CLANG_CXXABI)
#    target_link_libraries(${PROJECT_NAME} ${CLANG_CXXABI})
#endif(CLANG_CXXABI)

#if (NOT DOCOPT)
#	message(FATAL_ERROR "docopt.cpp required to compile, you can find it at https://github.com/docopt/docopt.cpp")
#endif (NOT DOCOPT)

#target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES} ${FastCGI_LIBRARIES} ${DOCOPT})
# add_dependencies(${PROJECT_NAME} license_headers)

#add_subdirectory(passwd)
#add_subdirectory(modules)

# Do the make install
#install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
#install(FILES ${CMAKE_SOURCE_DIR}/doc/nbstftp.conf.example DESTINATION etc RENAME nbstftp.conf)
# install(FILES ${MAN_FILE} DESTINATION man)
