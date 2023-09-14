#pragma once

namespace variant19 {

/// Type expresses that nothing is stored
/// used in Variant to mark an invalid state
/// note: mainly exists to be compaible with ostream/serialize etc.
struct None {
    bool operator==(const None&) const = default;
};

} // namespace variant19
