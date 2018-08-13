cmake_minimum_required (VERSION 2.8.11)
project (CoreBuffer)

enable_testing()

include_directories(src/ ./)

add_library(CoreBuffer STATIC src/parser.cpp src/parser.h src/cppoutput.cpp src/cppoutput.h src/package.cpp src/package.h)

add_executable (CoreBufferC src/corebuffer.cpp)

add_executable (CoreBufferTests catch/catch.hpp test/parser_tests.cpp test/parser_error_tests.cpp
  test/corebuffer_tests.cpp)

add_executable (CoreBufferOutputTests catch/catch.hpp test/basetypes.h test/enumtypes.h test/tabletypes.h test/shop.h
  test/shop_tests.cpp test/tabletypes_tests.cpp test/basetype_tests.cpp test/enumtypes_tests.cpp
  test/corebufferoutput_tests.cpp)

target_link_libraries(CoreBufferC CoreBuffer)
target_link_libraries(CoreBufferTests CoreBuffer)

add_test(NAME BaseTypeBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/basetypes.cor ${PROJECT_SOURCE_DIR}/test/basetypes.h)
add_test(NAME EnumTypesBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/enumtypes.cor ${PROJECT_SOURCE_DIR}/test/enumtypes.h)
add_test(NAME TableTypesBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/tabletypes.cor ${PROJECT_SOURCE_DIR}/test/tabletypes.h)
add_test(NAME ShopExampleBuild COMMAND $<TARGET_FILE:CoreBufferC> ${PROJECT_SOURCE_DIR}/cor/shop.cor ${PROJECT_SOURCE_DIR}/test/shop.h)

add_test(CoreBufferTest CoreBufferTests)
add_test(CoreBufferOutputTest CoreBufferOutputTests)
