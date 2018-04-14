//
// Created by xing on 4/14/18.
//

#ifndef NETWORK_POLLY_UTIL_COPYABLE_H_
#define NETWORK_POLLY_UTIL_COPYABLE_H_

namespace polly {

/// A tag class emphasises the objects are copyable.
/// The empty base class optimization applies.
/// Any derived class of copyable should be a value type.
class copyable {
};

} // namespace polly
#endif //NETWORK_POLLY_UTIL_COPYABLE_H_
