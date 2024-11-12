#include "tcp_receiver.hh"

using namespace std;

/*
：：类外定义成员函数 用来指明函数属于哪个类
 接收TCPSenderMessage并处理其内容，插入有效负载数据到流中。
 根据消息中的SYN、RST、FIN标志更新接收器的状态
*/
void TCPReceiver::receive( TCPSenderMessage message )
{
  //如果接受的消息包含了RST标志，则置为true，然后设置流的状态，然后返回
  if(message.RST)
  {
    RST=true;
    reader().set_error();//设置流的错误状态
    return;
  }
  //如果消息中包含SYN标志，则设置初始序列号
  if(message.SYN){
    isn=message.seqno;//设置初始序列号
    if_set_isn=true;//标记一下
    //更新下一个接受序号
    next_abs=max(next_abs,message.seqno.unwrap(isn,checkpoint));
  }
  if(!if_set_isn) return;//如果isn还没有设置，那么返回
  //解包并获得消息的绝对序列号
  uint64_t abs_seq=message.seqno.unwrap(isn,checkpoint);
  checkpoint =abs_seq;//更新检查点
  //计算流中插入数据的索引值
  uint64_t str_index=message.SYN ? 0:abs_seq-1;
  //插入数据到重组器当中，如果包含了FIN，则在末尾插入结束符
  reassembler_.insert(str_index,message.payload,message.FIN);
  //检查流是否发生错误，错误则设置RST标志
  if (writer().has_error()) RST=true;
  //更新next_abs为已推入的字节数+1
  next_abs=static_cast<uint64_t>(writer().bytes_pushed())+1;
  //如果流已经关闭，next_abs++
  if(writer().is_closed()) next_abs+=1;
}
/*
发送一个TCPReceiverMessage给对端发送者，用于确认接收的状态。
返回的消息包含下一个期望的序列号和窗口大小。
 */
TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage msg;  // 创建TCPReceiverMessage

  // 如果ISN已设置，计算下一个期望的序列号（ackno）
  if(if_set_isn) {
    msg.ackno = Wrap32(0).wrap(next_abs, isn);
  }

  // 设置窗口大小，限制在16位无符号整数的最大值
  msg.window_size = writer().available_capacity() > UINT16_MAX ? UINT16_MAX : writer().available_capacity();

  // 设置RST标志
  msg.RST = RST;
  // 如果流有错误，设置消息的RST标志
  if(writer().has_error()) msg.RST = true;

  return msg;
}
