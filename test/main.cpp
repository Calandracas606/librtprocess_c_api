
#include "TIFFUtils.hpp"
#include "librtprocess.h"
#include <cstddef>
#include <iostream>
#include <libraw/libraw.h>
#include <vector>

int main(void) {
  std::cout << "Hello, World\n";

  auto lr = LibRaw{};
  std::string file_name = "mtl.dng";

  std::cout << "test 1\n";
  auto ret = lr.open_file(file_name.c_str());
  if (ret != LIBRAW_SUCCESS) {
    auto libraw_msg = libraw_strerror(ret);
    throw std::runtime_error(libraw_msg);
  }

  std::cout << "test 2\n";

  ret = lr.unpack();
  std::cout << "test 3\n";
  if (ret != LIBRAW_SUCCESS) {
    auto libraw_msg = libraw_strerror(ret);
    throw std::runtime_error(libraw_msg);
  }

  std::cout << "test 4\n";
  auto h = lr.imgdata.sizes.raw_height;
  auto w = lr.imgdata.sizes.raw_width;

  // auto num_px = static_cast<std::size_t>(h * w)
  std::cout << "test 5\n";
  auto rawdata = std::vector<float *>(h, nullptr);
  auto raw_buf = std::vector<float>(h * w, 0.0);
  std::cout << "test 6\n";
  rawdata[0] = raw_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    rawdata[i] = rawdata[i - 1] + w;
  std::cout << "test 7\n";

  auto red = std::vector<float *>(h, nullptr);
  auto red_buf = std::vector<float>(h * w, 0.0f);
  std::cout << "test 8\n";
  red[0] = red_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    red[i] = red[i - 1] + w;
  std::cout << "test 9\n";

  auto green = std::vector<float *>(h, nullptr);
  auto green_buf = std::vector<float>(h * w, 0.0f);
  std::cout << "test 10\n";
  green[0] = green_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    green[i] = green[i - 1] + w;
  std::cout << "test 11\n";

  auto blue = std::vector<float *>(h, nullptr);
  auto blue_buf = std::vector<float>(h * w, 0.0f);
  std::cout << "test 11\n";
  blue[0] = blue_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    blue[i] = blue[i - 1] + w;
  std::cout << "test 12\n";

  unsigned int cfarray[2][2];
  int max_filter = 0;
  std::cout << "test 13\n";
  for (int row = 0; row < 2; ++row) {
    for (int col = 0; col < 2; ++col) {
      const int filter = lr.COLOR(row, col);
      if (filter > max_filter)
        max_filter = filter;
      cfarray[row][col] = filter < 3 ? filter : 1;
    }
  }
  std::cout << "test 14\n";

  for (std::size_t i = 0; i < h * w; ++i) {
    raw_buf[i] = lr.imgdata.rawdata.raw_image[i];
  }
  std::cout << "test 14\n";

  rcd_demosaic(w, h, rawdata.data(), red.data(), green.data(), blue.data(),
               cfarray, [](double progress) {
                 std::cout << "Progress " << progress << "\n";
                 return false;
               });
  std::cout << "test 15\n";

  float **img[3] = {red.data(), green.data(), blue.data()};
  std::cout << "test 16\n";

  tiff_dump(img, w, h, "out.tiff");
  std::cout << "test 17\n";
}
