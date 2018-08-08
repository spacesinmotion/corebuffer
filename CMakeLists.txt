cmake_minimum_required (VERSION 2.8.11)
project (CoreData)

enable_testing()

include_directories(src/ ./)

add_library(coredata STATIC src/parser.cpp src/cppoutput.cpp src/package.cpp)

add_executable (CoreData src/coredata.cpp)
add_executable (CoreDataTests test/parser_tests.cpp test/coredata_tests.cpp)
add_executable (CoreDataOutputTests test/tabletypes_tests.cpp test/basetype_tests.cpp test/enumtypes_tests.cpp test/coredataoutput_tests.cpp)

target_link_libraries(CoreData coredata)
target_link_libraries(CoreDataTests coredata)

add_test(NAME BaseTypeBuild COMMAND $<TARGET_FILE:CoreData> ${PROJECT_SOURCE_DIR}/cor/basetypes.cor ${PROJECT_SOURCE_DIR}/test/basetypes.h)
add_test(NAME EnumTypesBuild COMMAND $<TARGET_FILE:CoreData> ${PROJECT_SOURCE_DIR}/cor/enumtypes.cor ${PROJECT_SOURCE_DIR}/test/enumtypes.h)
add_test(NAME TableTypesBuild COMMAND $<TARGET_FILE:CoreData> ${PROJECT_SOURCE_DIR}/cor/tabletypes.cor ${PROJECT_SOURCE_DIR}/test/tabletypes.h)

add_test(CoreDataTest CoreDataTests)
add_test(CoreDataOutputTest CoreDataOutputTests)
