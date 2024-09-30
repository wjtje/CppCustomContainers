/**
 * @file set.h
 * @author Wouter (wjtje)
 * @brief A Set class template representing a set of elements where each bit
 * corresponds to an element in the set
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024 wjtje. MIT License
 */
#pragma once
#include <stdint.h>

/**
 * @brief A Set class template representing a set of elements.
 *
 * This class provides functionality to add, remove and check for the presence
 * of elements in the set.
 *
 * The set is represented as a single integer where each bit corresponds to an
 * element in the range [minEL, maxEL). Bit positions are calculated using the
 * formula U(value) - U(minEL).
 *
 * The maximum difference between minEL and maxEL depends on the size of
 * uintptr_t. This is 64 on 64 bit systems, and 32 on 32 bit systems.
 *
 * @tparam T Type of elements in the set (must be comparable with minEL and
 * maxEL)
 * @tparam minEL Minimum element value in the range [minEL, maxEL) (inclusive)
 * @tparam maxEL Maximum element value in the range [minEL, maxEL) (exclusive)
 */
template <typename T, T minEL, T maxEL>
class Set {
 public:
  Set() = default;
  Set(const Set &set) = default;
  Set(Set &&set) = default;

  /**
   * @brief Adds another Set object to this one.
   *
   * This operator adds another Set object to the current set by setting each
   * bit in the integer representation of both sets that corresponds to an
   * element present in both sets.
   *
   * @param value The Set object to add.
   */
  void operator+=(const Set &value) { data_ |= value.data_; }

  /**
   * @brief Removes another Set object from this one.
   *
   * This operator removes another Set object from the current set by setting
   * each bit in the integer representation of both sets that corresponds to an
   * element present in both sets.
   *
   * @param value The Set object to remove.
   */
  void operator-=(const Set &value) { data_ &= ~value.data_; }

  /**
   * @brief Exclusive OR another Set object with this one.
   *
   * This operator sets each bit in the integer representation of both sets
   * that corresponds to an element present in both sets.
   *
   * @param value The Set object to cross with.
   */
  void operator*=(const Set &value) { data_ &= value.data_; }

  /**
   * @brief Inserts an element into the set.
   *
   * If the element is within the valid range [minEL, maxEL), it is added to
   * the set by setting the corresponding bit in the integer representation.
   *
   * @param value The element to add.
   * @return A reference to this Set instance.
   */
  Set &operator<<(T value) {
    if (value < minEL || maxEL <= value) return *this;
    data_ |= (1 << (uint8_t(value) - uint8_t(minEL)));
    return *this;
  }
  Set &Insert(T value) { return operator<<(value); }

  /**
   * @brief Removes an element from the set.
   *
   * If the element is within the valid range [minEL, maxEL), it is removed
   * from the set by clearing the corresponding bit in the integer
   * representation.
   *
   * Note: This method does not handle cases where the value to remove is not
   * present in the set. It simply clears the bit without checking its previous
   * state.
   *
   * @param value The element to remove.
   * @return A reference to this Set instance.
   */
  Set &operator>>(T value) {
    if (value < minEL || maxEL <= value) return *this;
    data_ &= ~(1 << (uint8_t(value) - uint8_t(minEL)));
    return *this;
  }
  Set &Erase(T value) { return operator>>(value); }

  /**
   * @brief Checks if an element is present in the set.
   *
   * This method returns true if the element is within the valid range
   * [minEL, maxEL) and its corresponding bit in the integer representation
   * is set. Otherwise, it returns false.
   *
   * @param value The element to check for presence.
   * @return True if the element is present in the set, false otherwise.
   */
  bool operator[](T value) const {
    if (value < minEL || maxEL <= value) return false;
    return (data_ & (1 << (uint8_t(value) - uint8_t(minEL)))) != 0;
  }

  /**
   * @brief Checks if an element is present in the set, using a dedicated method
   * name.
   *
   * This method provides the same functionality as the overloaded operator[],
   * but allows for more clarity and readability in usage contexts.
   *
   * @param value The element to check for presence.
   * @return True if the element is present in the set, false otherwise.
   */
  bool Contains(T value) const { return (*this)[value]; }

  /**
   * @brief Equality comparison between two Set instances.
   *
   * This method checks whether both sets contain exactly the same elements,
   * without regard to their order or multiplicity. Two sets are considered
   * equal if they have the same size and all corresponding bit positions in
   * their integer representations are set to the same value (0 or 1).
   *
   * @param other The Set instance to compare with.
   * @return True if both sets are equal, false otherwise.
   */
  bool operator==(const Set &other) const { return data_ == other.data_; }

 private:
  /**
   * @brief This integer represents the set's data, with each bit corresponding
   * to an element in the range [minEL, maxEL). The variable is initialized to
   * zero, meaning no elements are initially present in the set.
   */
  uintptr_t data_ = 0;
};
