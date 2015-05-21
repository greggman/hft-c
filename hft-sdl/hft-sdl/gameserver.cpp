//
//  gameserver.cpp
//  hft-sdl
//
//  Created by GREGG TAVARES on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#include "gameserver.h"

#include "easywsclient.hpp"
#include "gamesystem.h"
#include "realnetplayer.h"

using namespace easywsclient;

namespace HappyFunTimes {
    
GameServer::MsgHandler::MsgHandler(GameServer* server) :
  server_(server) {
}

void GameServer::MsgHandler::operator()(const std::string& msg) const {
  server_->handleMessage(msg);
}

//public delegate void UntypedCmdEventHandler(Dictionary<string, object> data, string id);
//public delegate void TypedCmdEventHandler<T>(T eventArgs, string id) where T : MessageCmdData;
//
//private class CmdConverter<T> where T : MessageCmdData
//{
//    public CmdConverter(TypedCmdEventHandler<T> handler) {
//        m_handler = handler;
//    }
//
//    public void Callback(GameServer server, MessageCmdData data, Dictionary<string, object> dict, string id) {
//        server.QueueEvent(delegate() {
//            m_handler((T)data, id);
//        });
//    }
//
//    TypedCmdEventHandler<T> m_handler;
//}
//
//private class UntypedCmdConverter {
//    public UntypedCmdConverter(UntypedCmdEventHandler handler) {
//        m_handler = handler;
//    }
//
//    public void Callback(GameServer server, MessageCmdData data, Dictionary<string, object> dict, string id) {
//        server.QueueEvent(delegate() {
//            object mcd = null;
//            // dict is the MessageCmd. We want dict for the MessageCmdData inside the MessageCmd
//            // It might not exist
//            dict.TryGetValue("data", out mcd);
//            m_handler((Dictionary<string, object>)mcd, id);
//        });
//    }
//
//    UntypedCmdEventHandler m_handler;
//}

GameServer::Options::Options() {
//    cwd = Application.dataPath;
//    disconnectPlayersIfGameDisconnects = true;
//    url = "ws://localhost:18679";
//
//    // Prefix all HFT arguments with "hft-" so user can filter them out
//    ArgParser p = new ArgParser();
//    p.TryGet<string>("hft-id", ref id);
//    p.TryGet<string>("hft-url", ref url);
//    master = p.Contains("hft-master");
}

GameServer::GameServer(const Options& options) :
  options_(options),
  msg_handler_(this) {
//    m_gameSystem = new GameSystem(this);

    // TODO: Run HFT
}

GameServer::~GameServer() {
  if (socket_) {
    socket_->close();
    delete socket_;
  }
}

void GameServer::init() {
    connect();
}

void GameServer::connect() {
  if (socket_ == NULL) {
    got_messages_ = false;
    socket_ = WebSocket::from_url(options_.url);
  }
}

void GameServer::close() {
  if (socket_ != NULL) {
    cleanup();
    socket_->close();
    delete socket_;
    socket_ = NULL;
  }
}

template<> void GameServer::sendSysCmd(const char* cmd, const char* id, const GameServer::Options& o) {
};

void GameServer::process() {
  auto state = socket_->getReadyState();
  bool check = false;
  typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;
  switch (state) {
    case WebSocket::CONNECTING:
      check = true;
      break;
    case WebSocket::OPEN:
      if (!connected_) {
        connected_ = true;
        // Send all cube
        sendSysCmd<GameServer::Options>("server", "-1", options_);
      }
      check = true;
      break;
    case WebSocket::CLOSING:
      check = true;
      break;
    case WebSocket::CLOSED:
      break;
  }

  if (check) {
    socket_->poll();
    socket_->dispatch(msg_handler_);
  }
}

class JSONObject {
 public:
  JSONObject();
};

struct RelayServerCmd {
  std::string cmd;
  std::string id;
  JSONObject data;
};

struct MessageToClient {
  std::string cmd;
  std::string id;
  JSONObject data;
};

struct MessageGameStart {
  std::string id;
};

struct MessageCmd {
  std::string cmd;           // command to emit
  JSONObject data;//??
  //?  public MessageCmdData data;  // data for command
};

//// HFT system data that comes when the player starts
struct HFTPlayerStartData {
    std::string __hft_session_id__;
};

void GameServer::handleMessage(const std::string& str) {
  got_messages_ = true;

  if (str.compare("P") == 0) {
    send("P");
  }

  if (options_.show_messages) {
    printf("r[%d] %s\n", recv_count_++, str.c_str());
  }

  std::unique_ptr<MessageToClient> m = deserializer_.deserialize<MessageToClient>(str);
  if (!m) {
    fprintf(stderr, "bad message: %s\n", str.c_str());
    return;
  }

  const std::string& cmd = m->cmd;
  const std::string& id  = m->id;

  // TODO: change to hash_map<string, handler>
  if (cmd.compare("update") == 0) {
     updatePlayer(id, m->data);
  } else if (cmd.compare("upgame") == 0) {
     updateGame(id, m->data);
  } else if (cmd.compare("start") == 0) {
     startPlayer(id, "", m->data);
  } else if (cmd.compare("gamestart") == 0) {
     startGame(id, m->data);
  } else if (cmd.compare("remove") == 0) {
     removePlayer(m->id);
  } else if (cmd.compare("system") == 0) {
     doSysCommand(m->data);
  } else {
     fprintf(stderr, "unknown client message: %s\n", cmd.c_str());
  }
}

//void GameServer::RegisterCmdHandler(const string& name, UntypedCmdEventHandler callback) {
//    UntypedCmdConverter converter = new UntypedCmdConverter(callback);
//    m_handlers[name] = converter.Callback;
//    m_mcdc.RegisterCreator(name, typeof(Dictionary<string, object>));
//}
//
///// <param name="server">This needs the server because messages need to be queued as they need to be delivered on anther thread</param>.
//private delegate void CmdEventHandler(GameServer server, MessageCmdData cmdData, Dictionary<string, object> dict, string id);
//
//
//public class MessageToClient {
//    public string cmd;  // command 'server', 'update'
//    public string id;      // id of client
//    public Dictionary<string, object> data;
//};
//
//private class MessageGameStart {
//    public string id = "";
//};
//
//private class RelayServerCmd {
//    public RelayServerCmd(string _cmd, string _id, object _data) {
//        cmd = _cmd;
//        id = _id;
//        data = _data;
//    }
//
//    public string cmd;
//    public string id;
//    public object data;
//}
//
//private void SocketOpened(object sender, System.EventArgs e) {
//    //invoke when socket opened
//    Debug.Log("Connnected to HappyFunTimes");
//    m_connected = true;
//
//    List<String>.Enumerator i = m_sendQueue.GetEnumerator();
//    while (i.MoveNext()) {
//        m_socket.Send(i.Current);
//    }
//    m_sendQueue.Clear();
//
//    // Inform the relayserver we're a server
//    try {
//        SendSysCmd("server", "-1", m_options);
//    } catch (Exception ex) {
//        Debug.LogException(ex);
//    }
//}
//
//private void SocketClosed(object sender, CloseEventArgs e) {
//    //invoke when socket closed
//    if (m_connected) {
//        Debug.Log("Disconnected from HappyFunTimes");
//    }
//    Cleanup();
//}
//
//private void SocketMessage(object sender, MessageEventArgs e) {
//    m_gotMessages = true;
//    //invoke when socket message
//    if ( e!= null && e.Type == Opcode.Text) {
//        try {
//            // Handle ping.
//            if (e.Data == "P") {
//                Send("P");
//                return;
//            }
//            if (m_options.showMessages) {
//                Debug.Log("r[" + (m_recvCount++) + "] " + e.Data);
//            }
//            MessageToClient m = m_deserializer.Deserialize<MessageToClient>(e.Data);
//            // TODO: make this a dict to callback
//            if (m.cmd.Equals("update")) {
//                UpdatePlayer(m.id, m.data);
//            } else if (m.cmd.Equals("upgame")) {
//                UpdateGame(m.id, m.data);
//            } else if (m.cmd.Equals("start")) {
//                StartPlayer(m.id, "", m.data);
//            } else if (m.cmd.Equals("gamestart")) {
//                StartGame(m.id, m.data);
//            } else if (m.cmd.Equals("remove")) {
//                RemovePlayer(m.id);
//            } else if (m.cmd.Equals("system")) {
//                DoSysCommand(m.data);
//            } else {
//                Debug.LogError("unknown client message: " + m.cmd);
//            }
//        } catch (Exception ex) {
//            Debug.LogException(ex);  // TODO: Add object if possible
//            return;
//        }
//    }
//}
//
//private void SocketError(object sender, ErrorEventArgs e) {
//    if (!m_gotMessages) {
//        Debug.Log("Could not connect to HappyFunTimes. Is it running?");
//        Close();
//        QueueEvent(delegate() {
//            if (OnConnectFailure != null) {
//                OnConnectFailure.Emit(this, new EventArgs());
//            }
//        });
//    } else {
//        //invoke when socket error
//        Debug.Log("socket error: " + e.Message);
//        Cleanup();
//    }
//}
//
void GameServer::cleanup() {
    bool was_connected = connected_;
    connected_ = false;

    if (was_connected) {
      printf("TODO: disconnect emit event\n");
      //  QueueEvent(delegate() {
      //      if (OnDisconnect != null) {
      //          OnDisconnect.Emit(this, new EventArgs());
      //      }
      //  });
    }

    while (!players_.empty()) {
      NetPlayerMap::iterator it = players_.begin();
      removePlayer(it->first);
    }
}

void GameServer::startPlayer(const std::string& id, const std::string& name, const JSONObject& data) {
  // Exit if this player already exists
  if (players_.find(id) != players_.end()) {
    return;
  }

  std::string local_name(name);
  if (local_name.empty()) {
    char buffer[100];
    sprintf(buffer, "Player%d", ++total_player_count_);
    local_name = std::string(buffer);
  }

  RealNetPlayer::Options options;
  std::unique_ptr<HFTPlayerStartData> start_data = deserializer_.deserialize<HFTPlayerStartData>(data);
  if (start_data) {
    options.session_id = start_data->__hft_session_id__;
  }

  std::unique_ptr<NetPlayer> player(new RealNetPlayer(this, id, name, options));

  printf("TODO: Emit player connect message\n");
  //  QueueEvent(delegate() {
  //      // UGH! This is not thread safe because someone might add handler to OnPlayerConnect
  //      // Odds are low though?
  //      if (OnPlayerConnect != null) {
  //          OnPlayerConnect.Emit(this, new PlayerConnectMessageArgs(player, name, data));
  //      }
  //  });
  players_.insert(std::make_pair(id, std::move(player)));
}

void GameServer::doSysCommand(const JSONObject& cmd) {
  gamesystem_->handleUnparsedCommand(cmd);
}

void GameServer::updatePlayer(const std::string& id, const JSONObject& cmd) {
  NetPlayerMap::iterator it = players_.find(id);
  if (it == players_.end()) {
    return;
  }

  it->second->sendUnparsedEvent(cmd);
}


void GameServer::startGame(const std::string& id, const JSONObject& cmd) {
  std::unique_ptr<MessageGameStart> data = deserializer_.deserialize<MessageGameStart>(cmd);
  if (data) {
    id_ = data->id;
  }
printf("TODO: !OnCoonect!");
//
//    QueueEvent(delegate() {
//        if (OnConnect != null) {
//            OnConnect.Emit(this, new EventArgs());
//        }
//    });
}


void GameServer::updateGame(const std::string& id, const JSONObject& data) {
  std::unique_ptr<MessageCmd> cmd = deserializer_.deserialize<MessageCmd>(data);
  if (!cmd) {
    fprintf(stderr, "error in updateGame\n");
    return;
  }

  CmdEventHandlerMap::iterator it = handlers_.find(cmd->cmd);
  if (it == handlers_.end()) {
    fprintf(stderr, "unhandled GameServer cmd: %s\n", cmd->cmd.c_str());
    return;
  }
  const std::unique_ptr<CmdEventHandler>& handler = it->second;
  (*handler)(deserializer_, cmd->data, data, id);
}

void GameServer::removePlayer(const std::string& id) {
  NetPlayerMap::iterator it = players_.find(id);
  if (it == players_.end()){
    return;
  }
  it->second->disconnect();
  players_.erase(it);
}

void GameServer::send(const std::string& msg) {
  if (connected_) {
    if (options_.show_messages) {
      printf("q[%d] %s\n", send_count_++, msg.c_str());
    }
    if (socket_) {
      socket_->send(msg);
    }
  }
}
//
//public void SendSysCmd(string cmd, string id, object data) {
//    var msg = new RelayServerCmd(cmd, id, data);
//    string json = Serializer.Serialize(msg);
//    Send(json);
//}
//
//// Only NetPlayer should call this.
//public void SendCmd(string cmd, string name, MessageCmdData data, string id = "-1") {
//    MessageCmd msgCmd = new MessageCmd(name, data);
//    SendSysCmd(cmd, id, msgCmd);
//}
//
//// Only NetPlayer should call this.
//public void SendCmd(string cmd, MessageCmdData data, string id = "-1") {
//    string name = MessageCmdDataNameDB.GetCmdName(data.GetType());
//    SendCmd(cmd, name, data, id);
//}
//
//public void BroadcastCmd(string cmd, MessageCmdData data) {
//    SendCmd("broadcast", cmd, data);
//}
//
//public void BroadcastCmd(MessageCmdData data) {
//    SendCmd("broadcast", data);
//}
//
//public void SendCmdToGame(string id, string cmd, MessageCmdData data) {
//    SendCmd("peer", cmd, data, id);
//}
//
//public void SendCmdToGame(string id, MessageCmdData data) {
//    SendCmd("peer", data, id);
//}
//
//public void BroadcastCmdToGames(string cmd, MessageCmdData data) {
//    SendCmd("bcastToGames", cmd, data);
//}
//
//public void BroadcastCmdToGames(MessageCmdData data) {
//    SendCmd("bcastToGames", data);
//}
//
//
//
    
}  // namespace HappyFunTimes

