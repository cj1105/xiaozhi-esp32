#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <vector>
#include "DeviceState.h"

class DeviceStateMachineA {
 public:
  DeviceStateMachineA();
  DeviceStateA GetState() const { return current_state_.load(); }
  bool TransitionTo(DeviceStateA new_state);
  bool CanTransitionTo(DeviceStateA target) const;
  using StateCallback = std::function<void(DeviceStateA, DeviceStateA)>;
  int AddStateChangeListener(StateCallback callback);
  void RemoveStateChangeListener(int listener_id);

 private:
  std::atomic<DeviceStateA> current_state_{kDeviceStateUnknownA};
  std::vector<std::pair<int, StateCallback>> listeners_;
  int next_listener_id_{0};
  std::mutex mutex_;
  bool IsValidTransition(DeviceStateA from, DeviceStateA to) const;
  void NotifyStateChange(DeviceStateA old_state, DeviceStateA new_state);
};

