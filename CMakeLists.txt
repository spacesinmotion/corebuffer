cmake_minimum_required (VERSION 3.1)

project (CoreBuffer LANGUAGES CXX VERSION 0.1)

enable_testing()
set (CMAKE_CXX_STANDARD 11)

include_directories(src/ ./)

add_library(CoreBuffer STATIC src/parser.cpp src/parser.h src/structurecheck.h src/structurecheck.cpp src/cppoutput.cpp
  src/cppoutput.h src/package.cpp src/package.h src/fileposition.h src/fileerror.h)

add_executable (CoreBufferC src/corebuffer.cpp)

add_executable (CoreBufferTests catch/catch.hpp test/parser_tests.cpp test/parser_error_tests.cpp
  test/corebuffer_tests.cpp test/structurecheck_tests.cpp)

add_executable (CoreBufferOutputTests catch/catch.hpp test/basetypes.h test/enumtypes.h test/tabletypes.h test/shop.h
  test/shop_tests.cpp test/tabletypes_tests.cpp test/basetype_tests.cpp test/enumtypes_tests.cpp
  test/corebufferoutput_tests.cpp)

target_link_libraries(CoreBufferC CoreBuffer)
target_link_libraries(CoreBufferTests CoreBuffer)

add_test(NAME BaseTypeBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/basetypes.cor ${PROJECT_SOURCE_DIR}/test/basetypes.h)
add_test(NAME EnumTypesBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/enumtypes.cor ${PROJECT_SOURCE_DIR}/test/enumtypes.h)
add_test(NAME TableTypesBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/tabletypes.cor ${PROJECT_SOURCE_DIR}/test/tabletypes.h)
add_test(NAME ShopExampleBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/shop.cor ${PROJECT_SOURCE_DIR}/test/shop.h)
add_test(NAME SchemaBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/schema.cor ${PROJECT_SOURCE_DIR}/test/schema.h)

add_test (NAME CheckUsage1 COMMAND $<TARGET_FILE:CoreBufferC> )
add_test (NAME CheckUsage2 COMMAND $<TARGET_FILE:CoreBufferC> "not_existing.cor" "not_existing.h")
add_test (NAME CheckUsage3 COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/schema.cor "not_/_working.h")
add_test (NAME CheckHelp COMMAND $<TARGET_FILE:CoreBufferC> --help)
add_test (NAME ArgsError COMMAND $<TARGET_FILE:CoreBufferC> --fail)
set_tests_properties (CheckUsage1 CheckUsage2 CheckUsage3 CheckHelp ArgsError PROPERTIES
  PASS_REGULAR_EXPRESSION "\[<input.cor>\] \[<output.h>\]")

add_test (NAME CheckVersion COMMAND $<TARGET_FILE:CoreBufferC> --version)
set_tests_properties (CheckVersion PROPERTIES PASS_REGULAR_EXPRESSION ${PROJECT_VERSION})

add_test (NAME CheckStructureError COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/some_errors.cor schema.h)
add_test (NAME CheckError COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/parser_errors.cor schema.h)
set_tests_properties (CheckStructureError CheckError PROPERTIES PASS_REGULAR_EXPRESSION "error:")

add_test(CoreBufferTest CoreBufferTests -r compact)
add_test(CoreBufferOutputTest CoreBufferOutputTests)

install (TARGETS CoreBufferC DESTINATION bin)
