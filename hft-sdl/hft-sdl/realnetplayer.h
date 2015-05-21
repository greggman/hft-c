//
//  realnetplayer.h
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happfuntimes_realnetplayer__
#define __happfuntimes_realnetplayer__

#include "netplayer.h"

namespace HappyFunTimes {

class RealNetPlayer : public NetPlayer {
 public:
  class Options {
   public:
    Options();
    std::string session_id;
  };

  RealNetPlayer(GameServer* game_server, const std::string& id, const std::string& name, const RealNetPlayer::Options& options);
  virtual ~RealNetPlayer();

};

}  // namespace HappyFunTimes

#endif /* defined(__happfuntimes_realnetplayer__) */
