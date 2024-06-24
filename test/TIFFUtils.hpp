
#pragma once
#ifndef DM_TIFF_UTILS_HPP
#define DM_TIFF_UTILS_HPP

#include <cstddef> // for size_t
#include <string>  // for string

enum myErrors {
  RAW_TO_TIFF_OKAY = 0,
  LIBTIFF_NOT_FOUND,
  LIBRAW_NOT_FOUND,
  TIFF_TEST_FAIL,
};

auto tiff_dump(float **img[3], const std::size_t w, const std::size_t h,
               const std::string &file_name) -> int;

#endif // ! DM_TIFF_UTILS_HPP
