#include "color.h"

#include <math.h>

// MARK: ColorRgb

const ColorRgb ColorRgb::kBlack = {0, 0, 0};

ColorRgb::ColorRgb(uint8_t r, uint8_t g, uint8_t b)
    : red(r), green(g), blue(b) {}
ColorRgb::ColorRgb(const ColorHsv& hsv) { *this = hsv.ToRgb(); }
ColorRgb::ColorRgb(const ColorTemp& ct) { *this = ct.ToRgb(); }

ColorHsv ColorRgb::ToHsv() const {
  // Temporary storage
  union {
    int32_t sl;
    uint32_t ul;
  };

  // Source: https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB

  uint16_t hue;
  uint8_t saturation, value;

  const uint8_t max = std::max({red, green, blue});  // Maximum component
  const uint8_t min = std::min({red, green, blue});  // Minimun component
  const uint8_t c = max - min;                       // Range (i.e. chrome)

  // Calulate value (Brightness)
  ul = max << 24;
  ul /= 255;
  ul *= 100;
  value = uint8_t(ul >> 24);
  if ((ul & 0xFFFFFF) >= 0x800000) value += 1;

  if (c == 0 || max == 0) {
    // Grayscale
    hue = 0;
    saturation = 0;
  } else {
    // Calculate hue
    if (red == max) {
      // Red channel dominant
      sl = (green - blue) << 16;
      sl /= c;
      if (sl < 0) {
        // Due to the way this is calculated using fixed-point arithmatic this
        // weird syntax is required
        sl = -sl;
        sl *= 60;

        hue = 360 - uint16_t(sl >> 16);
        if ((sl & 0xFFFF) >= 0x8000) hue -= 1;
        goto calc_saturation;
      }
    } else if (green == max) {
      // Green channel dominant
      sl = (blue - red) << 16;
      sl /= c;
      sl += 2 << 16;
    } else {
      // Blue channel dominant
      sl = (red - green) << 16;
      sl /= c;
      sl += 4 << 16;
    }

    sl *= 60;
    hue = uint16_t(sl >> 16);
    if ((sl & 0xFFFF) >= 0x8000) hue += 1;

  calc_saturation:
    // Calculate saturation
    ul = c << 24;
    ul /= max;
    ul *= 100;
    saturation = uint8_t(ul >> 24);
    if ((ul & 0xFFFFFF) >= 0x800000) saturation += 1;
  }

  // Make sure the values are in range

  return ColorHsv(hue, saturation, value);
}

ColorTemp ColorRgb::ToCt() const {
  uint16_t kelvin = 1000;

  if (red == 255) {  // Kelvin <= 6600
    // 99.4708025861 * ln(t) - 161.1195681661 = green
    // 99.4708025861 * ln(t) = green + 161.1195681661
    // ln(t) = (green + 161.1195681661) / 99.4708025861
    // t = exp((green + 161.1195681661) / 99.4708025861)
    const double temp = exp((green + 161.1195681661) / 99.4708025861);

    // Round to every 25 degress
    kelvin = uint16_t(temp * 100);
    if (kelvin % 25 != 0) {
      kelvin += 25 / 2;
      kelvin -= (kelvin % 25);
    }
  } else {  // Kelvin >= 6000
    // 329.698727446 * pow((t - 60), -0.1332047592) = red
    // pow((t - 60), -0.1332047592) = red / 329.698727446
    // t - 60 = (red / 329.698727446)^(1/-0.1332047592)
    // t = (red / 329.698727446)^(1/-0.1332047592) + 60
    const double temp_red =
        (pow((red / 329.698727446), 1 / -0.1332047592)) + 60;

    const double temp_green =
        (pow((green / 288.1221695283), 1 / -0.0755148492)) + 60;

    const double temp =
        std::min(temp_red, temp_green) + (abs(temp_red - temp_green) / 2);

    // Round to every 50 degrees.
    kelvin = uint16_t(temp * 100);
    if (kelvin % 50 != 0) {
      kelvin += 50 / 2;
      kelvin -= (kelvin % 50);
    }
  }

  return ColorTemp(kelvin);
}

// MARK: ColorHsv

ColorHsv::ColorHsv(uint16_t hue, uint8_t saturation, uint8_t value) {
  SetHue(hue);
  SetSaturation(saturation);
  SetValue(value);
}

ColorHsv::ColorHsv(const ColorRgb& rgb) { *this = rgb.ToHsv(); }

ColorRgb ColorHsv::ToRgb() const {
  // Adapted from: https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
  const uint32_t chroma =
      uint32_t(value_ * saturation_) * 255 / 10000;  // [0, 255]
  const uint16_t m = (value_ * 255 / 100) - chroma;  // [0, 255]

  int32_t tmp = hue_ << 16;
  tmp /= 60;
  tmp %= 2 << 16;
  tmp -= 0xFFFF;
  tmp = abs(tmp);
  tmp = 0xFFFF - tmp;
  tmp *= chroma;
  const uint8_t x = uint8_t(tmp >> 16);  //[0, 255]

  const uint8_t h = uint8_t(hue_ / 60);  // [0, 5]
  if (h == 0 || h >= 6) {
    return ColorRgb(chroma + m, x + m, m);
  } else if (h == 1) {
    return ColorRgb(x + m, chroma + m, m);
  } else if (h == 2) {
    return ColorRgb(m, chroma + m, x + m);
  } else if (h == 3) {
    return ColorRgb(m, x + m, chroma + m);
  } else if (h == 4) {
    return ColorRgb(x + m, m, chroma + m);
  } else if (h == 5) {
    return ColorRgb(chroma + m, m, x + m);
  }

  return ColorRgb();
}

// MARK: ColorTemp

const ColorTemp ColorTemp::kCandle{1850};
const ColorTemp ColorTemp::kIncandescent{2400};
const ColorTemp ColorTemp::kFluorescent{3000};
const ColorTemp ColorTemp::kDaylight{5000};
const ColorTemp ColorTemp::kWhite{6500};
const ColorTemp ColorTemp::kCoolWhite{7000};

ColorTemp::ColorTemp(uint16_t kelvin) { SetKelvin(kelvin); }
ColorTemp::ColorTemp(const ColorRgb& rgb) { *this = rgb.ToCt(); }

ColorRgb ColorTemp::ToRgb() const {
  // Source:
  // https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html

  const double t = double(kelvin_) / 100;
  double r, g, b;

  if (t <= 66.0f) {
    r = 255;
    g = 99.4708025861 * log(t) - 161.1195681661;

    if (t <= 19.0f) {
      b = 0.0f;
    } else {
      b = 138.5177312231 * log(t - 10) - 305.0447927307;
    }
  } else {
    r = 329.698727446 * pow(t - 60, -0.1332047592);
    g = 288.1221695283 * pow(t - 60, -0.0755148492);
    b = 255;
  }

  return ColorRgb{uint8_t(std::clamp(r, 0.0, 255.0)),
                  uint8_t(std::clamp(g, 0.0, 255.0)),
                  uint8_t(std::clamp(b, 0.0, 255.0))};
}
