//
//  gamesystem.h
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happfuntimes_gamesystem__
#define __happfuntimes_gamesystem__

namespace HappyFunTimes {

class JSONObject;

class GameSystem {
 public:
  GameSystem();
  void handleUnparsedCommand(const JSONObject& cmd);
};

}  // namespace HappyFunTimes

#endif /* defined(__happfuntimes_gamesystem__) */
