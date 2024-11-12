#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

class TCPReceiver
{
public:
  // Construct with given Reassembler
  //构造函数
  explicit TCPReceiver( Reassembler&& reassembler ) : reassembler_( std::move( reassembler )) ,isn(0) {}

  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  void receive( TCPSenderMessage message );

  // The TCPReceiver sends TCPReceiverMessages to the peer's TCPSender.
  TCPReceiverMessage send() const;

  // Access the output (only Reader is accessible non-const)
  const Reassembler& reassembler() const { return reassembler_; }
  Reader& reader() { return reassembler_.reader(); }
  const Reader& reader() const { return reassembler_.reader(); }
  const Writer& writer() const { return reassembler_.writer(); }

private:
  Reassembler reassembler_;
  Wrap32 isn;//初始序列号
  uint64_t next_abs=0;//下一个期望收到的绝对序号
  uint64_t checkpoint =0;//上一个接收到的最大绝对序列号
  bool RST=false;//重置标志，指示是否接收到RST（重置标志）
  bool if_set_isn=false;//是否已经设置了ISN

};
