//
// Created by yahuichen on 2019/1/25.
//

#ifndef LET_CALLBACK_H
#define LET_CALLBACK_H

#include <functional>
#include <memory>

struct event;

namespace let
{
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void(TcpConnectionPtr)>;
using DisconnectionCallback = std::function<void(TcpConnectionPtr)>;
using MessageCallback = std::function<void(TcpConnectionPtr)>;

// conn, errorCode
using ErrorCallback = std::function<void(TcpConnectionPtr, int)>;

using TimerId = struct event *;

using TimerCallback = std::function<void()>;

using SingalCallback = std::function<void()>;


} // namespace let
#endif //LET_CALLBACK_H
