
#include "TIFFUtils.hpp"
#include "librtprocess.h"
#include <cstddef>
#include <iostream>
#include <libraw/libraw.h>
#include <vector>

int main(int argc, char **argv) {
  std::cout << "Hello, World\n";

  auto lr = LibRaw{};
  std::string file_name = "mtl.dng";

  auto ret = lr.open_file(file_name.c_str());
  if (ret != LIBRAW_SUCCESS) {
    auto libraw_msg = libraw_strerror(ret);
    throw std::runtime_error(libraw_msg);
  }

  ret = lr.unpack();
  if (ret != LIBRAW_SUCCESS) {
    auto libraw_msg = libraw_strerror(ret);
    throw std::runtime_error(libraw_msg);
  }

  auto h = lr.imgdata.sizes.raw_height;
  auto w = lr.imgdata.sizes.raw_width;

  // auto num_px = static_cast<std::size_t>(h * w)
  auto rawdata = std::vector<float *>(h, nullptr);
  auto raw_buf = std::vector<float>(h * w, 0.0);
  rawdata[0] = raw_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    rawdata[i] = rawdata[i - 1] + w;

  auto red = std::vector<float *>(h, nullptr);
  auto red_buf = std::vector<float>(h * w, 0.0f);
  red[0] = red_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    red[i] = red[i - 1] + w;

  auto green = std::vector<float *>(h, nullptr);
  auto green_buf = std::vector<float>(h * w, 0.0f);
  green[0] = green_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    green[i] = green[i - 1] + w;

  auto blue = std::vector<float *>(h, nullptr);
  auto blue_buf = std::vector<float>(h * w, 0.0f);
  blue[0] = blue_buf.data();
  for (std::size_t i = 1; i < h; ++i)
    blue[i] = blue[i - 1] + w;

  unsigned int cfarray[2][2];
  int max_filter = 0;
  for (int row = 0; row < 2; ++row) {
    for (int col = 0; col < 2; ++col) {
      const int filter = lr.COLOR(row, col);
      if (filter > max_filter)
        max_filter = filter;
      cfarray[row][col] = filter < 3 ? filter : 1;
    }
  }

  for (std::size_t i = 0; i < h * w; ++i) {
    raw_buf[i] = lr.imgdata.rawdata.raw_image[i];
  }

  // rcd_demosaic(w, h, rawdata.data(), red.data(), green.data(), blue.data(),
  //              cfarray, [](double progress) {
  //                std::cout << "Progress " << progress << "\n";
  //                return false;
  //              });

  auto num_iters = (argc >= 2 ? atoi(argv[1]) : 1);
  std::cout << "Running " << num_iters << " Iterations... \n";
  for (int i = 0; i < num_iters; ++i) {
    auto err = amaze_demosaic(
        w, h, lr.imgdata.sizes.left_margin, lr.imgdata.sizes.top_margin, w, h,
        rawdata.data(), red.data(), green.data(), blue.data(), cfarray,
        [](double progress) -> bool {
          //       std::cerr << "amaze progress: " << progress << "\n";
          return false;
        },
        1, 0, 1, 1, 2, true);
  }

  float **img[3] = {red.data(), green.data(), blue.data()};

  tiff_dump(img, w, h, "out.tiff");
}
// RTPROCESS_API rpError amaze_demosaic(int raw_width, int raw_height, int winx,
// int winy, int winw, int winh, const float * const *rawData, float **red,
// float **green, float **blue, const unsigned cfarray[2][2], const
// std::function<bool(double)> &setProgCancel, double initGain, int border,
// float inputScale, float outputScale, std::size_t chunkSize = 2, bool measure
// = false);
