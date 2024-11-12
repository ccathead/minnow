#pragma once

#include <cstdint>

class Wrap32
{
public:
  // 构造函数：以给定的 32 位原始值初始化 Wrap32 对象
  explicit Wrap32(uint32_t raw_value) : raw_value_(raw_value) {}

  /* 
   wrap 静态方法：根据绝对序列号 n 和指定的零点（起始序列号），
   将一个 64 位的绝对序列号转换为 Wrap32 表示（32 位序列号）。
   参数：n：绝对序列号（64 位）、zero_point：32 位的零点，用于确定封装的起始位置。
   返回：封装后的 Wrap32 序列号。
   */
  static Wrap32 wrap(uint64_t n, Wrap32 zero_point);

  /*
   * unwrap 方法：将当前 Wrap32 对象表示的 32 位序列号解包为接近给定 checkpoint 的
   * 64 位绝对序列号。
    参数：zero_point：32 位的零点，用于解包过程中对序列号的偏移基准。
        -checkpoint：最近的绝对序列号，作为解包的参考点，用于找到最接近 checkpoint 的绝对序列号。
    返回：与当前 Wrap32 序列号对应的、最接近 checkpoint 的 64 位绝对序列号。
   */
  uint64_t unwrap(Wrap32 zero_point, uint64_t checkpoint) const;

  // 重载加法运算符，将当前 Wrap32 值与 32 位无符号整数相加，返回新的 Wrap32 对象
  Wrap32 operator+(uint32_t n) const { return Wrap32{raw_value_ + n}; }

  // 重载等于运算符，判断两个 Wrap32 对象是否相等
  bool operator==(const Wrap32& other) const { return raw_value_ == other.raw_value_; }

  // 重载小于运算符，判断当前 Wrap32 的原始值是否小于一个无符号整数
  bool operator<(uint32_t other) const { return raw_value_ < other; }

  // 重载加等于运算符，将一个无符号整数加到当前 Wrap32 对象上
  void operator+=(uint32_t other) { raw_value_ += other; }

  // 获取当前 Wrap32 对象的原始 32 位值
  uint32_t get_raw_value() const { return raw_value_; }

protected:
  uint32_t raw_value_{};  // 存储 32 位的原始序列号值
};