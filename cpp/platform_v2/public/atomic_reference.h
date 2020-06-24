#ifndef PLATFORM_V2_PUBLIC_ATOMIC_REFERENCE_H_
#define PLATFORM_V2_PUBLIC_ATOMIC_REFERENCE_H_

#include <memory>
#include <type_traits>

#include "platform_v2/api/atomic_reference.h"
#include "platform_v2/api/platform.h"
#include "platform_v2/public/mutex.h"
#include "platform_v2/public/mutex_lock.h"
#include "absl/types/any.h"

namespace location {
namespace nearby {

// An object reference that may be updated atomically.
template <typename, typename = void>
class AtomicReference;

// Platform-based atomic type, for something convertible to std::uint32_t.
template <typename T>
class AtomicReference<T, std::enable_if_t<sizeof(T) <= sizeof(std::uint32_t) &&
                                              std::is_trivially_copyable_v<T>,
                                          void>>
    final {
 public:
  using Platform = api::ImplementationPlatform;
  explicit AtomicReference(T value)
      : impl_(Platform::CreateAtomicUint32(static_cast<std::uint32_t>(value))) {
  }
  ~AtomicReference() = default;
  AtomicReference(AtomicReference&&) = default;
  AtomicReference& operator=(AtomicReference&&) = default;

  T Get() const { return static_cast<T>(impl_->Get()); }
  void Set(T value) { impl_->Set(static_cast<std::uint32_t>(value)); }

 private:
  std::unique_ptr<api::AtomicUint32> impl_;
};

// Atomic type that is using Platform mutex to provide atomicity.
// Supports any copyable type.
template <typename T>
class AtomicReference<T, std::enable_if_t<(sizeof(T) > sizeof(std::uint32_t) ||
                                           !std::is_trivially_copyable_v<T>),
                                          void>>
    final {
 public:
  explicit AtomicReference(T value) {
    MutexLock lock(&mutex_);
    value_ = std::move(value);
  }
  void Set(T value) {
    MutexLock lock(&mutex_);
    value_ = std::move(value);
  }
  T Get() const& {
    MutexLock lock(&mutex_);
    return value_;
  }
  T&& Get() const&& {
    MutexLock lock(&mutex_);
    return std::move(value_);
  }

 private:
  mutable Mutex mutex_;
  T value_;
};

}  // namespace nearby
}  // namespace location

#endif  // PLATFORM_V2_PUBLIC_ATOMIC_REFERENCE_H_
