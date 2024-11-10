#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  // cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  // cerr << "Warning: get_URL() has not been implemented yet.\n";
  //创建一个TCP套接字
  TCPSocket sock{};
  //连接到指定主机的HTTP服务
  sock.connect(Address(host,"http"));
  sock.write("GET "+path+" HTTP/1.1\r\n");
  sock.write("Host: "+host+"\r\n");
  sock.write("Connection: close\r\n\r\n");
  //通知服务器请求已经发送完毕
  sock.shutdown(SHUT_WR);
  //读取服务器的回应并输出
  while(!sock.eof()){
    string tmp;
    sock.read(tmp);
    cout<<tmp;

  }
  //关闭套接字
  sock.close();
  return;
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
