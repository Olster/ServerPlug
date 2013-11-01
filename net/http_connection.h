#ifndef NET_SOCKET_HTTP_CONNECTION_H_
#define NET_SOCKET_HTTP_CONNECTION_H_

#include "base/build_required.h"

#include <string>
#include <assert.h>

namespace net {
class TcpSocket;

class HttpConnection {
 public:
  DISALLOW_COPY_AND_ASSIGN(HttpConnection);
  DISALLOW_MOVE(HttpConnection);

  HttpConnection(TcpSocket* clientSock, const std::string& filesPath)
   : m_clientSock(clientSock),
     m_files(filesPath) {
    assert(m_clientSock);
  }

  ~HttpConnection();

  TcpSocket* clientSocket() const { return m_clientSock; }

  void SetReadyClose() { m_bReadyClose = true; }
  bool ReadyClose() const { return m_bReadyClose; }

  int ReadRequest();
  void ProcessRequest();
  int SendResponse();

  // Returns true if server has any data for the client.
  bool DataAvailable() const { return m_response.length() > 0; }
 private:
  // TODO(Olster): Add more method support.
  enum Method {
    GET = 0,
    METHOD_MAX // Notifies about an error.
  };

  Method MethodFromString(const std::string& method);

  // TODO(Olster): Eventually add support for HTTP/2.0.
  enum HttpVersion {
    HTTP_1_1 = 0,
    HTTP_ERROR
  };

  // Formats "400 Bad Request" message to send to client.
  void FormatInvalidRequestResponse();

  TcpSocket* m_clientSock = nullptr;
  std::string m_files;
  bool m_bReadyClose = false;

  std::string m_request;
  std::string m_response;
};

} // namespace net

#endif // NET_SOCKET_HTTP_CONNECTION_H_
