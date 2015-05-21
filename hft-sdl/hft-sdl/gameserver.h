//
//  gameserver.h
//  hft-sdl
//
//  Created by GREGG TAVARES on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happyfuntimes_gameserver__
#define __happyfuntimes_gameserver__

#include <functional>
#include <list>
#include <map>
#include <string>

#include "deserializer.h"

namespace easywsclient {

class WebSocket;

}

namespace HappyFunTimes {

class NetPlayer;
class GameServer;
class GameSystem;
class JSONObject;

//GameServer::RegisterCmdHandler("foo", function MessageThatTakesAMessageFoo)


class GameServer {
 public:
  class Options {
   public:
     Options();

     ///<summary>
     /// there's generally no need to set this.
     ///</summary>
     std::string game_id;

     ///<summary>
     /// id used for multi-player games. Can be set from command line with --hft=id=someid
     ///</summary>
     std::string id;

     ///<summary>
     ///Deprecated and not used.
     ///</summary>
     std::string controller_url;

     ///<summary>
     ///true allows multiple games to run as the same id. Default: false
     ///
     ///normally when a second game connects the first game will be disconnected
     ///as it's assumed the first game probably crashed or for whatever reason did
     ///not disconnect and this game is taking over. Setting this to true doesn't
     ///disconnect the old game. This is needed for multi-machine games.
     ///</summary>
     bool allow_multiple_games;   // allow multiple games

     ///<summary>
     ///For a multiple machine game designates this game as the game where players start.
     ///Default: false
     ///Can be set from command line with --hft-master
     ///</summary>
     bool master;

     ///<summary>
     ///The URL of HappyFunTimes
     ///
     ///Normally you don't need to set this as HappyFunTimes is running on the same machine
     ///as the game. But, for multi-machine games you'd need to tell each machine the address
     ///of the machine running HappyFunTimes. Example: "ws://192.168.2.9:18679".
     ///
     ///Can be set from the command line with --hft-url=someurl
     ///</summary>
     std::string url;

     ///<summary>
     ///Normally if a game disconnets all players are also disconnected. This means
     ///they'll auto join the next game running.
     ///Default: true
     ///</summary>
     bool disconnect_players_if_game_disconnects;

     ///<summary>
     ///Prints all the messages in and out to the console.
     ///</summary>
     bool show_messages;

     ///<summary>
     ///path to exe. Note: this is used to tell HFT where to find your game's HTML assets
     ///If you don't set it it will be set automatically to the cwd.
     ///</summary>
     std::string cwd;
 };

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

  /// <summary>
  /// Constructor for GameServer
  /// </summary>
  /// <param name="options">The objects</param>
  GameServer(const Options& options);
  ~GameServer();

  /// <summary>
  /// Starts the connection to HappyFunTimes.
  /// </summary>
  void init();

  void close();
  void process();

  void queueEvent(std::function<void ()> func);

//  void RegisterCmdHandler(string name, UntypedCmdEventHandler callback);
//      UntypedCmdConverter converter = new UntypedCmdConverter(callback);
//      m_handlers[name] = converter.Callback;
//      m_mcdc.RegisterCreator(name, typeof(Dictionary<string, object>));
//  }

  /// <summary>
  /// Id of the machine assigned by HappyFunTimes.
  ///
  /// If you're running a multi-machine you can pass an id in the GameServer construtor options.
  /// If you don't pass an id you'll be assigned one. This is the assigned one.
  ///
  /// Note: It is invalid to read this property before the game has connected.
  /// </summary>
  /// <example>
  /// <code>
  /// ...
  /// gameServer.OnConnect += OnConnect;
  /// ...
  /// void OnConnect(EventArgs e) {
  ///   Debug.Log(gameServer.Id);
  /// }
  /// </code>
  /// </example>
  /// <returns>id of machine assigned by HappyFunTimes</returns>
  const std::string& id() {
    return id_;
  }

  private:
    class MsgHandler {
     public:
      MsgHandler(GameServer* server);

      void operator()(const std::string& data) const;

     private:
      GameServer* server_;
    };

    void handleMessage(const std::string& str);
    void connect();
    void cleanup();
    void send(const std::string& msg);

    void updatePlayer(const std::string& id, const JSONObject& data);
    void updateGame(const std::string& id, const JSONObject& data);
    void startGame(const std::string& id, const JSONObject& data);
    void startPlayer(const std::string& id, const std::string& name, const JSONObject& data);
    void removePlayer(const std::string& id);
    void doSysCommand(const JSONObject& data);

//    void setDisconnectHandler(Callable)

//    template<typename T> void sendSysCmd(const std::string& cmd, const std::string& id, const T& data);
    template<typename T> void sendSysCmd(const char* cmd, const char* id, const T& data) {
    }

    Options options_;
    bool connected_ = false;
    int total_player_count_ = 0;
    int recv_count_ = 0;
    int send_count_ = 0;
    int queue_count_ = 0;
    bool got_messages_ = false;
    easywsclient::WebSocket* socket_;
    Deserializer deserializer_;
    MsgHandler msg_handler_;
    std::unique_ptr<GameSystem> gamesystem_;

    typedef std::map<std::string, std::unique_ptr<NetPlayer> > NetPlayerMap;
    NetPlayerMap players_;
    typedef std::map<std::string, std::unique_ptr<CmdEventHandler> > CmdEventHandlerMap;
    CmdEventHandlerMap handlers_;  // handlers by command name
    std::string id_;  // id assigned to this game.

   // private void SocketOpened(object sender, System.EventArgs e) {
   //     //invoke when socket opened
   //     Debug.Log("Connnected to HappyFunTimes");
   //     m_connected = true;
   //
   //     List<String>.Enumerator i = m_sendQueue.GetEnumerator();
   //     while (i.MoveNext()) {
   //         m_socket.Send(i.Current);
   //     }
   //     m_sendQueue.Clear();
   //
   //     // Inform the relayserver we're a server
   //     try {
   //         SendSysCmd("server", "-1", m_options);
   //     } catch (Exception ex) {
   //         Debug.LogException(ex);
   //     }
   // }
   //
   // private void SocketClosed(object sender, CloseEventArgs e) {
   //     //invoke when socket closed
   //     if (m_connected) {
   //         Debug.Log("Disconnected from HappyFunTimes");
   //     }
   //     Cleanup();
   // }
   //
   // private void SocketMessage(object sender, MessageEventArgs e) {
   //     m_gotMessages = true;
   //     //invoke when socket message
   //     if ( e!= null && e.Type == Opcode.Text) {
   //         try {
   //             // Handle ping.
   //             if (e.Data == "P") {
   //                 Send("P");
   //                 return;
   //             }
   //             if (m_options.showMessages) {
   //                 Debug.Log("r[" + (m_recvCount++) + "] " + e.Data);
   //             }
   //             MessageToClient m = m_deserializer.Deserialize<MessageToClient>(e.Data);
   //             // TODO: make this a dict to callback
   //             if (m.cmd.Equals("update")) {
   //                 UpdatePlayer(m.id, m.data);
   //             } else if (m.cmd.Equals("upgame")) {
   //                 UpdateGame(m.id, m.data);
   //             } else if (m.cmd.Equals("start")) {
   //                 StartPlayer(m.id, "", m.data);
   //             } else if (m.cmd.Equals("gamestart")) {
   //                 StartGame(m.id, m.data);
   //             } else if (m.cmd.Equals("remove")) {
   //                 RemovePlayer(m.id);
   //             } else if (m.cmd.Equals("system")) {
   //                 DoSysCommand(m.data);
   //             } else {
   //                 Debug.LogError("unknown client message: " + m.cmd);
   //             }
   //         } catch (Exception ex) {
   //             Debug.LogException(ex);  // TODO: Add object if possible
   //             return;
   //         }
   //     }
   // }
   //
   // private void SocketError(object sender, ErrorEventArgs e) {
   //     if (!m_gotMessages) {
   //         Debug.Log("Could not connect to HappyFunTimes. Is it running?");
   //         Close();
   //         QueueEvent(delegate() {
   //             if (OnConnectFailure != null) {
   //                 OnConnectFailure.Emit(this, new EventArgs());
   //             }
   //         });
   //     } else {
   //         //invoke when socket error
   //         Debug.Log("socket error: " + e.Message);
   //         Cleanup();
   //     }
   // }
   //
   // private void Cleanup()
   // {
   //     bool wasConnected = m_connected;
   //     m_connected = false;
   //
   //     if (wasConnected) {
   //         QueueEvent(delegate() {
   //             if (OnDisconnect != null) {
   //                 OnDisconnect.Emit(this, new EventArgs());
   //             }
   //         });
   //     }
   //
   //     while (m_players.Count > 0) {
   //         Dictionary<string, NetPlayer>.Enumerator i = m_players.GetEnumerator();
   //         i.MoveNext();
   //         RemovePlayer(i.Current.Key);
   //     }
   // }
   //
   // private void StartPlayer(string id, string name, Dictionary<string, object> data) {
   //     if (m_players.ContainsKey(id)) {
   //         return;
   //     }
   //
   //     if (string.IsNullOrEmpty(name)) {
   //         name = "Player" + (++m_totalPlayerCount);
   //     }
   //
   //     RealNetPlayer.Options options = new RealNetPlayer.Options();
   //     if (data != null) {
   //         DeJson.Deserializer deserializer = new DeJson.Deserializer();
   //         HFTPlayerStartData startData = deserializer.Deserialize<HFTPlayerStartData>(data);
   //         if (startData != null) {
   //             options.sessionId = startData.__hft_session_id__;
   //         }
   //     }
   //
   //     NetPlayer player = new RealNetPlayer(this, id, name, options);
   //     m_players[id] = player;
   //     QueueEvent(delegate() {
   //         // UGH! This is not thread safe because someone might add handler to OnPlayerConnect
   //         // Odds are low though?
   //         if (OnPlayerConnect != null) {
   //             OnPlayerConnect.Emit(this, new PlayerConnectMessageArgs(player, name, data));
   //         }
   //     });
   // }
   //
   // private void DoSysCommand(Dictionary<string, object> cmd) {
   //     m_gameSystem.HandleUnparsedCommand(cmd);
   // }
   //
   // private void UpdatePlayer(string id, Dictionary<string, object> cmd) {
   //     NetPlayer player;
   //     if (!m_players.TryGetValue(id, out player)) {
   //         return;
   //     }
   //     player.SendUnparsedEvent(cmd);
   // }
   //
   // private void StartGame(string id, Dictionary<string, object> cmd) {
   //     MessageGameStart data = m_deserializer.Deserialize<MessageGameStart>(cmd);
   //     m_id = data.id;
   //
   //     QueueEvent(delegate() {
   //         if (OnConnect != null) {
   //             OnConnect.Emit(this, new EventArgs());
   //         }
   //     });
   // }
   //
   // private void UpdateGame(string id, Dictionary<string, object> data) {
   //     try {
   //         MessageCmd cmd = m_deserializer.Deserialize<MessageCmd>(data);
   //         CmdEventHandler handler;
   //         if (!m_handlers.TryGetValue(cmd.cmd, out handler)) {
   //             Debug.LogError("unhandled GameServer cmd: " + cmd.cmd);
   //             return;
   //         }
   //         handler(this, cmd.data, data, id);
   //     } catch (Exception ex) {
   //         Debug.LogException(ex);
   //     }
   // }
   //
   // private void RemovePlayer(string id) {
   //     NetPlayer player;
   //     if (!m_players.TryGetValue(id, out player)) {
   //         return;
   //     }
   //     QueueEvent(delegate() {
   //         player.Disconnect();
   //     });
   //     m_players.Remove(id);
   // }
   //
   // private void Send(string msg) {
   //     if (m_connected) {
   //         if (m_options.showMessages) {
   //             Debug.Log("q[" + (m_queueCount++) + "] " + msg);
   //         }
   //         m_socket.Send(msg);
   //     } else {
   //         if (m_options.showMessages) {
   //             Debug.Log("s[" + (m_sendCount++) + "] " + msg);
   //         }
   //         m_sendQueue.Add(msg);
   //     }
   // }
   //
   // public void SendSysCmd(string cmd, string id, object data) {
   //     var msg = new RelayServerCmd(cmd, id, data);
   //     string json = Serializer.Serialize(msg);
   //     Send(json);
   // }
   //
   // // Only NetPlayer should call this.
   // public void SendCmd(string cmd, string name, MessageCmdData data, string id = "-1") {
   //     MessageCmd msgCmd = new MessageCmd(name, data);
   //     SendSysCmd(cmd, id, msgCmd);
   // }
   //
   // // Only NetPlayer should call this.
   // public void SendCmd(string cmd, MessageCmdData data, string id = "-1") {
   //     string name = MessageCmdDataNameDB.GetCmdName(data.GetType());
   //     SendCmd(cmd, name, data, id);
   // }
   //
   // public void BroadcastCmd(string cmd, MessageCmdData data) {
   //     SendCmd("broadcast", cmd, data);
   // }
   //
   // public void BroadcastCmd(MessageCmdData data) {
   //     SendCmd("broadcast", data);
   // }
   //
   // public void SendCmdToGame(string id, string cmd, MessageCmdData data) {
   //     SendCmd("peer", cmd, data, id);
   // }
   //
   // public void SendCmdToGame(string id, MessageCmdData data) {
   //     SendCmd("peer", data, id);
   // }
   //
   // public void BroadcastCmdToGames(string cmd, MessageCmdData data) {
   //     SendCmd("bcastToGames", cmd, data);
   // }
   //
   // public void BroadcastCmdToGames(MessageCmdData data) {
   //     SendCmd("bcastToGames", data);
   // }
   //
   // // HFT system data that comes when the player starts
   // class HFTPlayerStartData : MessageCmdData // don't think this needs to inherit from MessageCmdData
   // {
   //     public string __hft_session_id__ = "";
   // }
   //


};

}  // HappyFunTimes

#endif /* defined(__happyfuntimes_gameserver__) */
