#include "DeviceStateMachine.h"

DeviceStateMachineA::DeviceStateMachineA() {}

bool DeviceStateMachineA::TransitionTo(DeviceStateA new_state) {
  std::lock_guard<std::mutex> lock(mutex_);
  DeviceStateA old = current_state_.load();
  if (!IsValidTransition(old, new_state)) return false;
  current_state_.store(new_state);
  NotifyStateChange(old, new_state);
  return true;
}

bool DeviceStateMachineA::CanTransitionTo(DeviceStateA target) const {
  return IsValidTransition(current_state_.load(), target);
}

int DeviceStateMachineA::AddStateChangeListener(StateCallback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  int id = next_listener_id_++;
  listeners_.push_back({id, callback});
  return id;
}

void DeviceStateMachineA::RemoveStateChangeListener(int listener_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  listeners_.erase(std::remove_if(listeners_.begin(), listeners_.end(), [&](auto& p) { return p.first == listener_id; }), listeners_.end());
}

bool DeviceStateMachineA::IsValidTransition(DeviceStateA from, DeviceStateA to) const {
  if (from == to) return true;
  switch (from) {
    case kDeviceStateUnknownA: return to == kDeviceStateStartingA || to == kDeviceStateWifiConfiguringA;
    case kDeviceStateStartingA: return to == kDeviceStateIdleA || to == kDeviceStateWifiConfiguringA || to == kDeviceStateConnectingA;
    case kDeviceStateWifiConfiguringA: return to == kDeviceStateConnectingA || to == kDeviceStateIdleA;
    case kDeviceStateIdleA: return to == kDeviceStateConnectingA || to == kDeviceStateListeningA || to == kDeviceStateUpgradingA || to == kDeviceStateActivatingA;
    case kDeviceStateConnectingA: return to == kDeviceStateIdleA || to == kDeviceStateListeningA || to == kDeviceStateSpeakingA;
    case kDeviceStateListeningA: return to == kDeviceStateSpeakingA || to == kDeviceStateIdleA;
    case kDeviceStateSpeakingA: return to == kDeviceStateIdleA || to == kDeviceStateListeningA;
    case kDeviceStateUpgradingA: return to == kDeviceStateIdleA;
    case kDeviceStateActivatingA: return to == kDeviceStateIdleA;
    case kDeviceStateAudioTestingA: return to == kDeviceStateIdleA;
    case kDeviceStateFatalErrorA: return to == kDeviceStateStartingA;
    default: return false;
  }
}

void DeviceStateMachineA::NotifyStateChange(DeviceStateA old_state, DeviceStateA new_state) {
  for (auto& it : listeners_) {
    it.second(old_state, new_state);
  }
}

