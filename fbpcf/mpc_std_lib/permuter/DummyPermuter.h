/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "fbpcf/mpc_std_lib/permuter/IPermuter.h"

#include "fbpcf/mpc_std_lib/util/util.h"

namespace fbpcf::mpc_std_lib::permuter::insecure {

/**
 * This permuter doesn't do anything but simply output the input. It is only
 * meant to be used as a placeholder in tests.
 **/
template <typename T, int schedulerId>
class DummyPermuter final
    : public IPermuter<typename util::SecBatchType<T, schedulerId>::type> {
 public:
  using SecBatchType = typename util::SecBatchType<T, schedulerId>::type;
  DummyPermuter(int myId, int partnerId) : myId_(myId), partnerId_(partnerId) {}

  SecBatchType permute(const SecBatchType& src, size_t /*size*/)
      const override {
    auto placeHolder =
        util::MpcAdapters<T, schedulerId>::openToParty(src, partnerId_);

    return util::MpcAdapters<T, schedulerId>::processSecretInputs(
        placeHolder, partnerId_);
  }

  SecBatchType permute(
      const SecBatchType& src,
      size_t /*size*/,
      const std::vector<uint32_t>& order) const override {
    auto plaintext = util::MpcAdapters<T, schedulerId>::openToParty(src, myId_);
    auto permuted = plaintext;
    for (size_t i = 0; i < order.size(); i++) {
      permuted[i] = plaintext.at(order.at(i));
    }
    return util::MpcAdapters<T, schedulerId>::processSecretInputs(
        permuted, myId_);
  }

 private:
  int myId_;
  int partnerId_;
};

} // namespace fbpcf::mpc_std_lib::permuter::insecure
