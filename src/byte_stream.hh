// byte_stream.hh
#pragma once

#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>

class Reader;
class Writer;

class ByteStream
{
protected:
  enum State { CLOSED, ERROR };
  uint64_t capacity_;
  uint64_t bytes_pushed_ {}; // 已写入的字节数
  uint64_t bytes_popped_ {}; // 已弹出的字节数
  bool error_{}; // 错误状态标志，初始为 false

  unsigned char flag {};	// 0: normal, 1: closed, 2: error
  std::queue<std::string> buffer_data {};
  std::string_view buffer_view {};

public:
  explicit ByteStream( uint64_t capacity );

  // 其他成员函数和构造函数
  void set_error() { error_ = true; }       // 设置流的错误状态
  bool has_error() const { return error_; } // 检查流是否有错误

  // 提供ByteStream的 reader 和 writer 接口的辅助函数
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;
};

class Writer : public ByteStream
{
public:
  void push( std::string data ) noexcept; // 在可用容量允许的范围内向流中写入数据

  void close() noexcept; // 关闭流，不允许再向流中写入数据
  void set_error() noexcept; // 流中出现错误，置位错误标志

  bool is_closed() const noexcept;      // 判断流是否已关闭
  uint64_t available_capacity() const noexcept; // 计算流中剩余可用容量
  uint64_t bytes_pushed() const noexcept;       // 计算流中已写入的字节数
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const noexcept; // 返回流中下一个数据块的只读视图
  void pop( uint64_t len ) noexcept;      // 从流中弹出指定长度的数据块

  bool is_finished() const noexcept; // 判断流是否已关闭且所有数据块都已弹出
  bool has_error() const noexcept;   // 判断流是否出现错误

  uint64_t bytes_buffered() const noexcept; // 计算当前流中剩余的字节数
  uint64_t bytes_popped() const noexcept;   // 计算流中已弹出的字节数
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );
