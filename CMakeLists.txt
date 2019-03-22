cmake_minimum_required (VERSION 2.6)
project (CevalProfiler)
# The version number.
set (Ceval_prof_VERSION_MAJOR 1)
set (Ceval_prof_VERSION_MINOR 0)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/Include")

# add the executable
add_executable(CevalProfiler
        Python/ceval-profile.c
        pyconfig.h)

target_link_libraries(CevalProfiler -L${PROJECT_SOURCE_DIR}/Objects)
target_link_libraries(CevalProfiler -L${PROJECT_SOURCE_DIR}/Modules)
target_link_libraries(CevalProfiler -L${PROJECT_SOURCE_DIR}/Python)

target_link_libraries(CevalProfiler /usr/lib/libdtrace.dylib)
target_link_libraries(CevalProfiler ${PROJECT_SOURCE_DIR}/libpython3.8dm.a)

