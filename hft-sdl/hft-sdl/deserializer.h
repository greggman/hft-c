//
//  deserializer.h
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happfuntimes_deserializer__
#define __happfuntimes_deserializer__

#include <string>

namespace HappyFunTimes {

class JSONObject;

class Deserializer {
 public:
  Deserializer();
  template<typename T> std::unique_ptr<T> deserialize(const std::string& str) const;
  template<typename T> std::unique_ptr<T> deserialize(const JSONObject& data) const;
};

}  // namespace HappyFunTimes

#endif /* defined(__happfuntimes_deserializer__) */
