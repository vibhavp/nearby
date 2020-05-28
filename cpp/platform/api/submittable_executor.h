#ifndef PLATFORM_API_SUBMITTABLE_EXECUTOR_H_
#define PLATFORM_API_SUBMITTABLE_EXECUTOR_H_

#include <functional>

#include "platform/api/executor.h"
#include "platform/api/future.h"
#include "platform/api/platform.h"
#include "platform/api/settable_future.h"
#include "platform/api/submittable_executor_def.h"
#include "platform/exception.h"

namespace location {
namespace nearby {

// "Common" part of implementation.
// Placed here for textual compatibility to minimize scope of changes.
// Can be (and should be) moved to a separate file outside "api" folder.
// TODO(apolyudov): for API v2.0
template <typename T>
Ptr<Future<T>> SubmittableExecutor::submit(Ptr<Callable<T>> callable) {
  using Platform = platform::ImplementationPlatform;
  Ptr<SettableFuture<T>> future{Platform::createSettableFuture<T>()};
  bool submitted = DoSubmit([callable, future]() {
    ExceptionOr<T> result = callable->call();
    if (result.ok()) {
      future->set(std::move(result.result()));
    } else {
      future->setException({result.exception()});
    }
  });
  if (!submitted) {
    // Raise Exception::kExecution if we are shutting down.
    future->setException({Exception::kExecution});
  }
  return future;
}

}  // namespace nearby
}  // namespace location

#endif  // PLATFORM_API_SUBMITTABLE_EXECUTOR_H_
