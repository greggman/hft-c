//
//  eventprocessor.cpp
//  hft-sdl
//
//  Created by Gregg Tavares on 5/21/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#include "eventprocessor.h"

#include <mutex>

namespace HappyFunTimes {

void EventProcessor::queueEvent(std::function<void()> func) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  queued_events_.push_back(func);
}

void EventProcessor::process() {
  moveQueuedEventsToExecuting();

  while (!executing_events_.empty()) {
    executing_events_.front()();
    executing_events_.pop_front();
  }
}

void EventProcessor::moveQueuedEventsToExecuting() {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  executing_events_.splice(executing_events_.end(), queued_events_);
}


}  // namespace HappyFunTimes


