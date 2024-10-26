/**
 * @file color.h
 * @author Wouter (wjtje)
 * @brief Multiple wrappers around color data. With easy function to set, get,
 * manipluate and convert colors.
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2024 wjtje. MIT License
 */
#pragma once
#include <stdint.h>

#include <algorithm>

// Forward declare
class ColorRgb;
class ColorHsv;
class ColorTemp;

//-----------------------------------------------------------------------------

/**
 * @brief Store a color internally as RGB, each an 8 bit value.
 */
class ColorRgb {
 public:
  static const ColorRgb kBlack;

  ColorRgb() = default;
  ColorRgb(uint8_t r, uint8_t g, uint8_t b);
  ColorRgb(const ColorHsv& hsv);
  ColorRgb(const ColorTemp& ct);

  inline bool operator==(const ColorRgb& rhs) const {
    return (red == rhs.red && green == rhs.green && blue == rhs.blue);
  }

  inline constexpr void SetRed(uint8_t r) { red = r; }
  inline constexpr uint8_t Red() const { return red; }

  inline constexpr void SetGreen(uint8_t g) { green = g; }
  inline constexpr uint8_t Green() const { return green; }

  inline constexpr void SetBlue(uint8_t b) { blue = b; }
  inline constexpr uint8_t Blue() const { return blue; }

  ColorHsv ToHsv() const;
  ColorTemp ToCt() const;

  constexpr uint8_t GetLuminance() const {
    // https://en.wikipedia.org/wiki/Relative_luminance
    const uint16_t luminance =
        (0.2126f * red + 0.7152f * green + 0.0722f * blue);
    return std::clamp<uint8_t>(luminance, 0u, UINT8_MAX);
  }

 private:
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
};

//-----------------------------------------------------------------------------

/**
 * @brief Store a color internally as HSV. Due to the way this is stored
 * internally there are only 360 * 100 * 100 possible colors.
 */
class ColorHsv {
 public:
  ColorHsv() = default;
  ColorHsv(uint16_t hue, uint8_t saturation, uint8_t value);
  ColorHsv(const ColorRgb& rgb);

  inline bool operator==(const ColorHsv& other) const {
    return (hue_ == other.hue_ && saturation_ == other.saturation_ &&
            value_ == other.value_);
  }

  inline constexpr void SetHue(uint16_t hue) {
    hue_ = std::clamp(hue, uint16_t(0), uint16_t(360));
  }
  inline constexpr uint16_t GetHue() const { return hue_; }

  inline constexpr void SetSaturation(uint8_t saturation) {
    saturation_ = std::clamp(saturation, uint8_t(0), uint8_t(100));
  }
  inline constexpr uint8_t GetSaturation() const { return saturation_; }

  inline constexpr void SetValue(uint8_t value) {
    value_ = std::clamp(value, uint8_t(0), uint8_t(100));
  }
  inline constexpr uint8_t GetValue() const { return value_; }

  ColorRgb ToRgb() const;

 private:
  /// @brief [0-360]
  uint16_t hue_ = 0;
  /// @brief [0-100]
  uint8_t saturation_ = 0;
  /// @brief [0-100]
  uint8_t value_ = 0;
};

//-----------------------------------------------------------------------------

/**
 * @brief Stores a color as a temperature in kelvin.
 */
class ColorTemp {
 public:
  static const ColorTemp kCandle;
  static const ColorTemp kIncandescent;
  static const ColorTemp kFluorescent;
  static const ColorTemp kDaylight;
  static const ColorTemp kWhite;
  static const ColorTemp kCoolWhite;

  ColorTemp() = default;
  ColorTemp(uint16_t kelvin);
  ColorTemp(const ColorRgb& rgb);

  inline bool operator==(const ColorTemp& rhs) {
    return (kelvin_ == rhs.kelvin_);
  }

  inline constexpr void SetKelvin(uint16_t kelvin) {
    kelvin_ = std::clamp(kelvin, uint16_t(1500), uint16_t(15000));
  }
  inline constexpr uint16_t GetKelvin() const { return kelvin_; }

  ColorRgb ToRgb() const;

 private:
  uint16_t kelvin_ = 2700;
};