#include <cstdint>

#include "platform/api/atomic_boolean.h"
#include "platform/api/atomic_reference_def.h"
#include "platform/api/ble.h"
#include "platform/api/ble_v2.h"
#include "platform/api/bluetooth_adapter.h"
#include "platform/api/bluetooth_classic.h"
#include "platform/api/condition_variable.h"
#include "platform/api/count_down_latch.h"
#include "platform/api/hash_utils.h"
#include "platform/api/lock.h"
#include "platform/api/platform.h"
#include "platform/api/server_sync.h"
#include "platform/api/settable_future_def.h"
#include "platform/api/submittable_executor_def.h"
#include "platform/api/system_clock.h"
#include "platform/api/thread_utils.h"
#include "platform/api/wifi.h"
#include "platform/cancelable.h"
#include "platform/impl/sample/atomic_reference_impl.h"
#include "platform/impl/sample/settable_future_impl.h"
#include "platform/impl/shared/sample/sample_wifi_medium.h"
#include "platform/port/string.h"
#include "platform/ptr.h"
#include "platform/runnable.h"
#include "absl/types/any.h"

namespace location {
namespace nearby {
namespace platform {

Ptr<ScheduledExecutor> ImplementationPlatform::createScheduledExecutor() {
  return Ptr<ScheduledExecutor>{};
}

Ptr<SubmittableExecutor> ImplementationPlatform::createSingleThreadExecutor() {
  return Ptr<SubmittableExecutor>{};
}

Ptr<SubmittableExecutor> ImplementationPlatform::createMultiThreadExecutor(
    int max_concurrency) {
  return Ptr<SubmittableExecutor>{};
}

Ptr<AtomicReference<absl::any>>
ImplementationPlatform::createAtomicReferenceAny(absl::any initial_value) {
  return Ptr<AtomicReference<absl::any>>(
      new AtomicReferenceImpl(initial_value));
}

Ptr<SettableFuture<absl::any>>
ImplementationPlatform::createSettableFutureAny() {
  return Ptr<SettableFuture<absl::any>>(new SettableFutureImpl{});
}

Ptr<BluetoothAdapter> ImplementationPlatform::createBluetoothAdapter() {
  return Ptr<BluetoothAdapter>{};
}

Ptr<WifiMedium> ImplementationPlatform::createWifiMedium() {
  return Ptr<WifiMedium>();
}

Ptr<CountDownLatch> ImplementationPlatform::createCountDownLatch(
    std::int32_t count) {
  return Ptr<CountDownLatch>{};
}

Ptr<ThreadUtils> ImplementationPlatform::createThreadUtils() {
  return Ptr<ThreadUtils>{};
}

Ptr<SystemClock> ImplementationPlatform::createSystemClock() {
  return Ptr<SystemClock>{};
}

Ptr<AtomicBoolean> ImplementationPlatform::createAtomicBoolean(
    bool initial_value) {
  return Ptr<AtomicBoolean>{};
}

Ptr<BluetoothClassicMedium>
ImplementationPlatform::createBluetoothClassicMedium() {
  return Ptr<BluetoothClassicMedium>();
}

Ptr<BLEMedium> ImplementationPlatform::createBLEMedium() {
  return Ptr<BLEMedium>();
}

Ptr<BLEMediumV2> ImplementationPlatform::createBLEMediumV2() {
  return Ptr<BLEMediumV2>();
}

Ptr<ServerSyncMedium> ImplementationPlatform::createServerSyncMedium() {
  return Ptr<ServerSyncMedium>{};
}

Ptr<WebRtcSignalingMessenger>
ImplementationPlatform::createWebRtcSignalingMessenger(
    const std::string& self_id) {
  return Ptr<WebRtcSignalingMessenger>{};
}

Ptr<Lock> ImplementationPlatform::createLock() { return Ptr<Lock>{}; }

Ptr<ConditionVariable> ImplementationPlatform::createConditionVariable(
    Ptr<Lock> lock) {
  return Ptr<ConditionVariable>{};
}

Ptr<HashUtils> ImplementationPlatform::createHashUtils() {
  return Ptr<HashUtils>{};
}

std::string ImplementationPlatform::getDeviceId() { return "sample"; }

std::string ImplementationPlatform::getPayloadPath(int64_t payload_id) {
  return "/tmp/sample-" + std::to_string(payload_id);
}

}  // namespace platform
}  // namespace nearby
}  // namespace location
