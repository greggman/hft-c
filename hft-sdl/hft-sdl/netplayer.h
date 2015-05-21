//
//  netplayer.h
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happyfuntimes_netplayer__
#define __happyfuntimes_netplayer__

#include <map>
#include <string>
#include "deserializer.h"

namespace HappyFunTimes {

class GameServer;
class JSONObject;

class NetPlayer {
 public:
  class CmdEventHandler {
   public:
    CmdEventHandler();
    virtual ~CmdEventHandler();
    virtual void operator()(const Deserializer& deserializer, const JSONObject& data, const JSONObject& parent_data, const std::string& id);
  };

  template<typename ClassType, typename MsgType>
  class TypedEventHandler : public CmdEventHandler {
   public:
    typedef void (ClassType::*Handler)(const MsgType& data);
    TypedEventHandler(Handler handler) :
      handler_(handler) {
    }
    virtual void operator()(const Deserializer& deserializer, const JSONObject& data, const JSONObject& parent_data, const std::string& id) {
      std::unique_ptr<MsgType> msg = deserializer.deserialize<MsgType>(data);
      if (msg) {
        handler_(*msg);
      }
    }
   private:
    Handler handler_;
  };

  virtual ~NetPlayer();

  void disconnect();
  void sendUnparsedEvent(const JSONObject& cmd);

 protected:
  NetPlayer(GameServer* server, const std::string& name);

 private:
  std::string id_;
  bool connected_ = false;
  bool have_handlers_ = false;
  GameServer* server_;
  typedef std::map<std::string, std::unique_ptr<CmdEventHandler> > CmdEventHandlerMap;
  CmdEventHandlerMap handlers_;  // handlers by command name
  CmdEventHandlerMap internal_handlers_;  // handlers by command name
};

}  // namespace HappyFunTimes

#endif /* defined(__happyfuntimes_netplayer__) */
