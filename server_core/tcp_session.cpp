#include "server_core/tcp_session.h"

#include "base/logger.h"
#include "server_core/server.h"
#include "server_plugin/server_plugin.h"
#include "socket/tcp_listener.h"
#include "plugin_api/protocol_handler.h"

bool ConnectionSession::CanRead() {
  return !m_protoHandler->HasDataToSend();
}

bool ConnectionSession::HasDataToSend() {
  return m_protoHandler->HasDataToSend();
}

int ConnectionSession::OnWrite(int* err) {
  int sent = reinterpret_cast<TcpSocket*>(m_sock.get())->Send(
        m_protoHandler->data_to_send(),
        m_protoHandler->data_to_send_size(), err);

  if (sent > 0) {
    m_protoHandler->DidSend(sent);
  }

  return sent;
}

namespace {
const int kRequestBufSize = 512;
}  // namespace

int ConnectionSession::OnRead(int* err) {
  // TODO(Olster): Avoid copying.
  char buf[kRequestBufSize + 1];

  // TODO(Olster): Provide abstraction without casting to specific type.
  int bytesRead = reinterpret_cast<TcpSocket*>(m_sock.get())->Receive(
        buf, kRequestBufSize, err);
  if (bytesRead < 1) {
    return bytesRead;
  }

  buf[bytesRead] = '\0';

  m_protoHandler->DidReceive(buf, bytesRead);
  return bytesRead;
}

int AcceptorSession::OnRead(int* err) {
  Socket::SOCK_TYPE sock = reinterpret_cast<TcpListener*>(
       m_sock.get())->Accept(err);

  ConnectionSession* acceptedSession = new ConnectionSession(
        std::shared_ptr<Socket>(new TcpSocket(sock)),
        m_plugin->NewProtocolHandler());

  Log(INFO) << "Socket connected: " << sock << " Handled by plugin: \"" <<
    m_plugin->name() << '\"';

  m_server->RegisterSession(acceptedSession);

  return 1;
}