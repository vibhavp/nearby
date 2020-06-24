#ifndef CORE_V2_INTERNAL_INTERNAL_PAYLOAD_H_
#define CORE_V2_INTERNAL_INTERNAL_PAYLOAD_H_

#include <cstdint>

#include "core_v2/payload.h"
#include "proto/connections/offline_wire_formats.pb.h"
#include "platform_v2/base/byte_array.h"
#include "platform_v2/base/exception.h"

namespace location {
namespace nearby {
namespace connections {

// Defines the operations layered atop a Payload, for use inside the
// OfflineServiceController.
//
// <p>There will be an extension of this abstract base class per type of
// Payload.
class InternalPayload {
 public:
  explicit InternalPayload(Payload payload);
  virtual ~InternalPayload() = default;

  Payload ReleasePayload();

  Payload::Id GetId() const;

  // Returns the PayloadType of the Payload to which this object is bound.
  //
  // <p>Note that this is supposed to return the type from the OfflineFrame
  // proto rather than what is already available via
  // Payload::getType().
  //
  // @return The PayloadType.
  virtual PayloadTransferFrame::PayloadHeader::PayloadType GetType() const = 0;

  // Deduces the total size of the Payload to which this object is bound.
  //
  // @return The total size, or -1 if it cannot be deduced (for example, when
  // dealing with streaming data).
  virtual std::int64_t GetTotalSize() const = 0;

  // Breaks off the next chunk from the Payload to which this object is bound.
  //
  // <p>Used when we have a complete Payload that we want to break into smaller
  // byte blobs for sending across a hard boundary (like the other side of
  // a Binder, or another device altogether).
  //
  // @return The next chunk from the Payload, or null if we've reached the end.
  virtual ByteArray DetachNextChunk() = 0;

  // Adds the next chunk that comprises the Payload to which this object is
  // bound.
  //
  // <p>Used when we are trying to reconstruct a Payload that lives on the
  // other side of a hard boundary (like the other side of a Binder, or another
  // device altogether), one byte blob at a time.
  //
  // @param chunk The next chunk; this being null signals that this is the last
  // chunk, which will typically be used as a trigger to perform whatever state
  // cleanup may be required by the concrete implementation.
  virtual Exception AttachNextChunk(const ByteArray& chunk) = 0;

  // Cleans up any resources used by this Payload. Called when we're stopping
  // early, e.g. after being cancelled or having no more recipients left.
  virtual void Close() {}

 protected:
  Payload payload_;
  // We're caching the payload ID here because the backing payload will be
  // released to another owner during the lifetime of an incoming
  // InternalPayload.
  Payload::Id payload_id_;
};

}  // namespace connections
}  // namespace nearby
}  // namespace location

#endif  // CORE_V2_INTERNAL_INTERNAL_PAYLOAD_H_
