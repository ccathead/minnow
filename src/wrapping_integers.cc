#include "wrapping_integers.hh"
#include <iostream>

using namespace std;

/*
 * 将 64 位的无符号整数 n 封装成 32 位的 Wrap32 序列号。
 * zero_point 是一个参考起点，用于计算封装的结果。
 */
Wrap32 Wrap32::wrap(uint64_t n, Wrap32 zero_point) {
  // 将 64 位整数 n 截断为低 32 位
  uint32_t num = static_cast<uint32_t>(n);
  // 将截断后的值加到 zero_point 上，得到封装的 Wrap32 值
  return zero_point + num;
}

/*
 * 将 32 位的封装序列号解包成 64 位的绝对序列号。
 * zero_point 是参考的起点，checkpoint 是最近接收到的绝对序列号，
 * 用于将当前封装的序列号解包到最接近 checkpoint 的位置。
 */
uint64_t Wrap32::unwrap(Wrap32 zero_point, uint64_t checkpoint) const {
  // 获取 checkpoint 的高 32 位，表示大致的绝对序列号范围
  uint64_t top_32 = checkpoint >> 32;
  // 获取 checkpoint 的低 32 位，表示 checkpoint 的相对位置
  uint64_t low_32 = checkpoint & 0xFFFFFFFF;
  
  // 计算当前 Wrap32 值相对 zero_point 的偏移量，并转换为 64 位
  uint64_t num = static_cast<uint64_t>(this->raw_value_ - zero_point.get_raw_value());

  // 确定偏移的方向，bias_r 和 bias_l 用于调整偏移到最接近的绝对序列号
  int bias_r = num >= low_32 ? 0 : 1;  // 偏移到更高的 32 位块
  int bias_l = num <= low_32 ? 0 : 1;  // 偏移到更低的 32 位块

  // 计算偏移到更高或更低 32 位块的绝对序列号
  uint64_t larger = num + ((top_32 + bias_r) << 32);
  uint64_t less = num + ((top_32 - bias_l) << 32);

  // 比较 larger 和 less，返回与 checkpoint 最接近的绝对序列号
  if (top_32 != 0)  // 如果 top_32 不为 0，选择离 checkpoint 更近的值
    return (larger - checkpoint) > (checkpoint - less) ? less : larger;

  // 如果 num 小于 checkpoint，选择离 checkpoint 更近的值
  if (num <= checkpoint)
    return (larger - checkpoint) > (checkpoint - num) ? num : larger;

  // 默认返回 larger
  return larger;
}

