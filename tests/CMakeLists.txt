set(CATCH_ROOT ${CMAKE_BINARY_DIR}/external/catch)
set(CATCH_INSTALL_DIR "${CATCH_ROOT}")
set(CATCH_CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CATCH_INSTALL_DIR})
set(CATCH_CXX_ARGS "-std=c++1z")
if (NOT NO_CLANG)
    set (CATCH_CXX_ARGS "${CATCH_CXX_ARGS} -stdlib=libc++")
endif (NOT NO_CLANG)
set(CATCH_CMAKE_ARGS ${CATCH_CMAKE_ARGS} -DCMAKE_CXX_FLAGS=${CATCH_CXX_ARGS})

ExternalProject_Add(catch
    GIT_REPOSITORY https://github.com/philsquared/Catch
    PREFIX ${CATCH_ROOT}
    BINARY_DIR ${CATCH_ROOT}
    INSTALL_DIR ${CATCH_ROOT}
    CMAKE_ARGS ${CATCH_CMAKE_ARGS}
    #LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    #LOG_INSTALL ON
)

add_library(Catch INTERFACE)
add_dependencies(Catch catch)
target_include_directories(Catch INTERFACE ${CATCH_ROOT}/include/catch)

# Compile our test
add_executable(kstring_test ${CMAKE_SOURCE_DIR}/tests/kstring.cpp ${CMAKE_SOURCE_DIR}/src/kstring.cpp ${CMAKE_SOURCE_DIR}/src/ManagedBuffer.cpp)
target_link_libraries(kstring_test Catch)

# Enable CTest and do some unit tests.
add_test(NAME KStringTest COMMAND kstring_test)
