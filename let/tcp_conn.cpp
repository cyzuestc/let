//
// Created by yahuichen on 2019/1/23.
//

#include <event2/event.h>
#include "tcp_conn.h"
#include "event_loop.h"
#include "buffer.h"

namespace let
{
TcpConnection::TcpConnection(int fd,
                             const IpAddress &local_addr,
                             const IpAddress &remote_addr)
    : fd_(fd),
      remote_addr_(remote_addr),
      local_addr_(local_addr)
{
}

TcpConnection::~TcpConnection()
{
    bufferevent_free(buf_ev_);
    delete in_buf_;
    delete out_buf_;
}

evutil_socket_t TcpConnection::getFd()
{
    return bufferevent_getfd(buf_ev_);
}

void TcpConnection::send(const void *message, size_t len)
{
    out_buf_->add(message, len);

    bufferevent_setcb(buf_ev_,
                      nullptr,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ | EV_WRITE);
}

void TcpConnection::readCallback(struct bufferevent *bev, void *ctx)
{
    auto self = (TcpConnection *)ctx;

    self->message_cb_(self->shared_from_this());
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *ctx)
{
    auto conn = (TcpConnection *)ctx;
}

void TcpConnection::eventCallback(struct bufferevent *bev, short events, void *ctx)
{
    auto self = (TcpConnection *)ctx;

    bool finished = false;
    if (events & BEV_EVENT_EOF)
    {
        self->close_cb_(self->shared_from_this());
    }
    else if (events & BEV_EVENT_ERROR)
    {
        self->error_cb_(self->shared_from_this(), EVUTIL_SOCKET_ERROR());
    }
}

void TcpConnection::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpConnection::setCloseCallback(const CloseCallback &cb)
{
    close_cb_ = cb;
}

void TcpConnection::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

Buffer *TcpConnection::inputBuffer()
{
    return in_buf_;
}

Buffer *TcpConnection::outBuffer()
{
    return out_buf_;
}

void TcpConnection::setContext(std::any context)
{
    context_ = context;
}

std::any *TcpConnection::getContext()
{
    return &context_;
}

void TcpConnection::setBufferEvent(bufferevent *buf_ev)
{
    buf_ev_ = buf_ev;
}

const IpAddress &TcpConnection::getLocalAddr() const
{
    return local_addr_;
}

const IpAddress &TcpConnection::getRemoteAddr() const
{
    return remote_addr_;
}

void TcpConnection::bindEventLoop(EventLoop *event_loop)
{
    in_buf_ = new Buffer(bufferevent_get_input(buf_ev_));
    out_buf_ = new Buffer(bufferevent_get_output(buf_ev_));

    bufferevent_setcb(buf_ev_,
                      readCallback,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ);
    bufferevent_disable(buf_ev_, EV_WRITE);

    readCallback(buf_ev_, this);
}

} // namespace let