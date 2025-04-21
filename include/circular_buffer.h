/**
 * @file circular_buffer.h
 * @author Wouter (wjtje)
 * @brief This is a simple cirular buffer that uses a static internal buffer
 * @version 0.1
 * @date 2023-11-26
 *
 * @copyright Copyright (c) 2024 wjtje. MIT License
 */
#pragma once
#include <array>
#include <cstddef>

/**
 * @brief A basic circular buffer using a static buffer
 *
 * @tparam T The type of the static buffer
 * @tparam SIZE The length of the buffer
 */
template <typename T, size_t SIZE>
class CircularBuffer {
 public:
  using Buffer = std::array<T, SIZE>;

  /**
   * @brief Return true when the buffer is full.
   *
   * @return true
   * @return false
   */
  inline bool Full() const { return this->full_; }
  /**
   * @brief Return true when the buffer is empty
   *
   * @return true
   * @return false
   */
  constexpr bool Empty() const {
    return (!this->full_ && (this->tail_ == this->head_));
  }
  void Clear() {
    this->full_ = false;
    this->tail_ = 0;
    this->head_ = 0;
  }
  /**
   * @brief Return the size (capacity) of the buffer.
   *
   * @return size_t
   */
  inline constexpr size_t MaxSize() const { return SIZE; }
  /**
   * @brief Return the amount of elements in the buffer, this is between 0 and
   * size.
   *
   * @return size_t
   */
  size_t Size() const {
    if (this->full_) return SIZE;
    if (this->tail_ >= this->head_) return this->tail_ - this->head_;
    return SIZE + this->tail_ - this->head_;
  }
  /**
   * @brief Push data to the end of the buffer.
   *
   * @param data[in]
   * @return int Return 0 on success, -1 when out of space.
   */
  int Push(const T& data) {
    if (this->full_) return -1;
    this->buffer_[this->tail_] = data;
    this->advance_pointer_();
    return 0;
  }
  /**
   * @brief Push data to the end of the buffer, even if the buffer is full.
   *
   * @param data[in]
   */
  void PushForce(const T& data) {
    this->buffer_[this->tail_] = data;
    this->advance_pointer_();
  }
  /**
   * @brief Get the data that is at the front of the buffer
   *
   * @param data[out]
   * @return int Returns 0 on success, -1 when there is no data
   */
  int Pop(T* data) {
    if (this->Empty()) return -1;
    *data = this->buffer_[this->head_];
    this->retreat_pointer_();
    return 0;
  }
  /**
   * @brief Remove the data this is at the front of the buffer
   *
   * @return int Returns 0 on success, -1 when there is no data.
   */
  int Pop() {
    if (this->Empty()) return -1;
    this->retreat_pointer_();
    return 0;
  }
  /**
   * @brief Direct pop.
   * Get the data that is at the front of the buffer. Even if that data is
   * invalid.
   *
   * @return const T& A reference to that value
   */
  const T& DirectPop() {
    T& d = this->buffer_[this->head_];
    this->retreat_pointer_();
    return d;
  }
  /**
   * @brief Get the data that in the front of the buffer, without removing it.
   *
   * @param data
   * @return int Returns 0 on success, -1 when there is no data
   */
  int Peek(T** data) {
    if (this->Empty()) return -1;
    *data = &this->buffer_[this->head_];
    return 0;
  }
  /**
   * @brief Get access to the first item in the queue.
   * @warning This item is invalid when the queue is empty.
   *
   * @return T&
   */
  T& Front() { return this->buffer_[this->head_]; }

  struct Iterator {
    Iterator(size_t position, Buffer& buffer, bool is_tail)
        : position_(position), buffer_(buffer), is_head_(is_tail) {}

    T& operator*() const { return buffer_[position_]; }
    T* operator->() { return &buffer_[position_]; }
    T& operator=(const T& p) { return buffer_[position_] = p; }
    /**
     * @brief Get access to the pointer of the current item of the Iterator.
     * This can be used to update an item that's already in the queue.
     *
     * @return T&
     */
    T& Get() { return buffer_[position_]; }
    const T& Get() const { return buffer_[position_]; }

    Iterator& operator++() {
      if (++position_ == SIZE) position_ = 0;
      is_head_ = false;
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) {
      return a.position_ == b.position_ && a.is_head_ == b.is_head_;
    }
    friend bool operator!=(const Iterator& a, const Iterator& b) {
      return a.position_ != b.position_ || a.is_head_ != b.is_head_;
    }

    size_t position_;
    Buffer& buffer_;
    bool is_head_;  // Indicated the tail (begin) of the iterator
  };

  Iterator begin() { return Iterator(this->head_, this->buffer_, true); }
  Iterator end() { return Iterator(this->tail_, this->buffer_, this->Empty()); }

  Iterator begin() const { return Iterator(this->head_, this->buffer_, true); }
  Iterator end() const {
    return Iterator(this->tail_, this->buffer_, this->Empty());
  }

 protected:
  Buffer buffer_;
  size_t tail_{0}, head_{0};
  bool full_{false};

  void advance_pointer_() {
    if (this->full_)
      if (++(this->head_) == SIZE) this->head_ = 0;
    if (++(this->tail_) == SIZE) this->tail_ = 0;
    this->full_ = (this->tail_ == this->head_);
  }
  void retreat_pointer_() {
    this->full_ = false;
    if (++(this->head_) == SIZE) this->head_ = 0;
  }
};
