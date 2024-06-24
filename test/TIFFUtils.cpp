#include "TIFFUtils.hpp"
#include <algorithm>
#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t, uint32_t, UINT8_MAX, UINT32_MAX
#include <iostream> // for operator<<, basic_ostream, cout
#include <limits>
#include <memory>   // for shared_ptr, __shared_ptr_access
#include <string>   // for char_traits, string
#include <tiff.h>   // for ORIENTATION_TOPLEFT, PHOTOMETRIC_RGB, PLA...
#include <tiffio.h> // for TIFFSetField, TIFFOpen, _TIFFmalloc, TIFF...
#include <vector>

void tiff_set_fields(const std::shared_ptr<TIFF> &out, const int h, const int w,
                     const int s, const int bps = 8) {
  TIFFSetField(out.get(), TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(out.get(), TIFFTAG_IMAGELENGTH, h);
  TIFFSetField(out.get(), TIFFTAG_SAMPLESPERPIXEL, s);
  TIFFSetField(out.get(), TIFFTAG_BITSPERSAMPLE, bps);
  TIFFSetField(out.get(), TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(out.get(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(out.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

  // We set the strip size of the file to be size of one row of pixels
  TIFFSetField(out.get(), TIFFTAG_ROWSPERSTRIP,
               TIFFDefaultStripSize(out.get(), w * s * (bps / 8)));
}

auto tiff_dump(uint8_t *buf, const std::size_t w, const std::size_t h,
               const std::string &file_name) -> int {

  std::cerr << "tiff_dump: " << file_name << "\n";
  auto status = RAW_TO_TIFF_OKAY;

  auto out = std::shared_ptr<TIFF>(TIFFOpen(file_name.c_str(), "w"), TIFFClose);

  tiff_set_fields(out, h, w, 4);

  for (std::size_t row = 0; status == RAW_TO_TIFF_OKAY && row < h; ++row) {
    auto scanline = buf + row * w * 4;
    if (TIFFWriteScanline(out.get(), scanline, row, 0) < 0) {
      status = TIFF_TEST_FAIL;
    }
  }

  return status;
}

auto tiff_dump(float **img[3], const std::size_t w, const std::size_t h,
               const std::string &file_name) -> int {
  auto buf = std::vector<std::uint8_t>{static_cast<unsigned char>(h * w)};
  buf.resize(h * w * 4);

  auto norm = static_cast<float>(UINT8_MAX) / 65535.f;
  for (std::size_t row = 0; row < h; ++row) {
    for (std::size_t col = 0; col < w; ++col) {
      auto idx = ((row * w) + col) * 4;
      buf[idx + 0] = img[0][row][col] * norm;
      buf[idx + 1] = img[1][row][col] * norm;
      buf[idx + 2] = img[2][row][col] * norm;
      buf[idx + 3] = UINT8_MAX;
    }
  }

  return tiff_dump(buf.data(), w, h, file_name);
}
