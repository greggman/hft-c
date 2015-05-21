//
//  eventprocessor.h
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#ifndef __happfuntimes_eventprocessor__
#define __happfuntimes_eventprocessor__

#include <list>
#include <mutex>

namespace HappyFunTimes {

class EventProcessor {
 public:
  void queueEvent(std::function<void()> func);
  void process();

 private:
  void moveQueuedEventsToExecuting();

  std::list<std::function<void()> > queued_events_;
  std::list<std::function<void()> > executing_events_;
  std::mutex queue_mutex_;
};

}  // namespace HappyFunTimes

#endif /* defined(__happfuntimes_eventprocessor__) */
