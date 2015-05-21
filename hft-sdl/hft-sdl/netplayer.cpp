//
//  netplayer.cpp
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#include "netplayer.h"

#include "gameserver.h"
#include "jsonobject.h"

namespace HappyFunTimes {

struct MessageCmd {
  std::string cmd;           // command to emit
  JSONObject data;//??
  //?  public MessageCmdData data;  // data for command
};

void NetPlayer::sendUnparsedEvent(const JSONObject& cmd) {
  if (!connected_) {
    return;
  }

  // This is kind of round about. The issue is we queue message
  // if there are no handlers as that means no one has had time
  // to register any and those message will be lost.
  //
  // That's great but we can also call RemoveAllHanders. PlayerConnector
  // does this. Players that are waiting have all messages disconnected.
  // That means if they are waiting for 2-3 mins, with a poorly designed
  // controller there could be tons of messages queued up.
  //
  // So, only allow queuing messages once. After that they're never
  // queued.
  if (!handlers_.empty()) {
      have_handlers_ = true;
  }

  // If there are no handlers registered then the object using this NetPlayer
  // has not been instantiated yet. The issue is the GameSever makes a NetPlayer.
  // It then has to queue an event to start that player so that it can be started
  // on another thread. But, before that event has triggered other messages might
  // come through. So, if there are no handlers then we add an event to run the
  // command later. It's the same queue that will birth the object that needs the
  // message.
  if (!have_handlers_) {
      server_->queueEvent([=]() {
          sendUnparsedEvent(cmd);
      });
      return;
  }

  //try {
  //    MessageCmd cmd = m_deserializer.Deserialize<MessageCmd>(data);
  //    CmdEventHandler handler;
  //    if (!m_handlers.TryGetValue(cmd.cmd, out handler)) {
  //        if (!m_internalHandlers.TryGetValue(cmd.cmd, out handler)) {
  //            Debug.LogError("unhandled NetPlayer cmd: " + cmd.cmd);
  //            return;
  //        }
  //    }
  //    handler(m_server, cmd.data, data);
  //} catch (Exception ex) {
  //    Debug.LogException(ex);
  //}

}

}  // namespace HappyFunTimes

