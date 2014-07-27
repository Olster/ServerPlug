#include "net/socket/socket.h"

#if defined(UNIX)
#include <unistd.h>
#endif

const Socket::SOCK_TYPE Socket::kInvalidSocket;

#if defined(UNIX)
  typedef int SOCK_TYPE;
  static const SOCK_TYPE kInvalidSocket = -1;
#elif defined(WIN32)
  typedef SOCKET SOCK_TYPE;
  static const SOCK_TYPE kInvalidSocket = INVALID_SOCKET;
#endif

Socket::~Socket() {
  ShutDown();
  Close();
}

bool Socket::Close(int* err) {
#if defined(UNIX)
  int rc = close(m_socket);
#else
  int rc = closesocket(m_socket);
#endif

  if (err) {
    *err = SocketError();
  }

  m_socket = kInvalidSocket;
  return rc > -1;
}

bool Socket::ShutDown(ShutDownCode code, int* err) {
  int rc = shutdown(m_socket, code);
  if (err) {
    *err = SocketError();
  }

  return rc != 0;
}

bool Socket::OpenHelper(int domain, int type, int protocol, int* err) {
  m_socket = socket(domain, type, protocol);
  if (err) {
    *err = SocketError();
  }

  return m_socket != kInvalidSocket;
}