
find_library(LIB_RAW NAMES raw libraw)
find_library(LIB_TIFF NAMES tiff libtiff)
add_executable(rtprocess_test main.cpp TIFFUtils.cpp)
target_link_libraries(rtprocess_test rtprocess raw tiff)
