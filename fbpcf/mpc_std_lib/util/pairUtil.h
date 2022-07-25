/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace fbpcf::mpc_std_lib::util {

/*
 * The adapters here are partial template specializations for function
 * templates listed in mpc_std_lib/util/util.h file to support a pair template
 * type std::pair<T, U>.
 */

template <typename T, typename U, int schedulerId>
struct SecBatchType<std::pair<T, U>, schedulerId> {
  using type = std::pair<
      typename SecBatchType<T, schedulerId>::type,
      typename SecBatchType<U, schedulerId>::type>;
};

template <typename T, typename U, int schedulerId>
class MpcAdapters<std::pair<T, U>, schedulerId> {
 public:
  using SecBatchTypeT = typename SecBatchType<T, schedulerId>::type;
  using SecBatchTypeU = typename SecBatchType<U, schedulerId>::type;
  using SecBatchType =
      typename SecBatchType<std::pair<T, U>, schedulerId>::type;
  static SecBatchType processSecretInputs(
      const std::vector<std::pair<T, U>>& secrets,
      int secretOwnerPartyId) {
    auto size = secrets.size();
    std::vector<T> secrets1(size);
    std::vector<U> secrets2(size);
    for (size_t i = 0; i < size; i++) {
      secrets1[i] = secrets.at(i).first;
      secrets2[i] = secrets.at(i).second;
    }
    auto rst1 = MpcAdapters<T, schedulerId>::processSecretInputs(
        secrets1, secretOwnerPartyId);
    auto rst2 = MpcAdapters<U, schedulerId>::processSecretInputs(
        secrets2, secretOwnerPartyId);
    return {rst1, rst2};
  }

  static std::pair<SecBatchType, SecBatchType> obliviousSwap(
      const SecBatchType& src1,
      const SecBatchType& src2,
      frontend::Bit<true, schedulerId, true> indicator) {
    auto [rst11, rst12] = MpcAdapters<T, schedulerId>::obliviousSwap(
        src1.first, src2.first, indicator);
    auto [rst21, rst22] = MpcAdapters<U, schedulerId>::obliviousSwap(
        src1.second, src2.second, indicator);
    return {{rst11, rst21}, {rst12, rst22}};
  }

  static std::vector<std::pair<T, U>> openToParty(
      const SecBatchType& src,
      int partyId) {
    auto rst1 = MpcAdapters<T, schedulerId>::openToParty(src.first, partyId);
    auto rst2 = MpcAdapters<U, schedulerId>::openToParty(src.second, partyId);
    if (rst1.size() != rst2.size()) {
      throw std::runtime_error("data size does not match");
    }

    auto size = rst1.size();
    std::vector<std::pair<T, U>> rst(size);
    for (size_t i = 0; i < size; i++) {
      rst[i] = {rst1.at(i), rst2.at(i)};
    }
    return rst;
  }

  static SecBatchType batchingWith(
      const SecBatchType& src,
      const std::vector<SecBatchType>& others) {
    std::vector<SecBatchTypeT> others1(others.size());
    std::vector<SecBatchTypeU> others2(others.size());
    for (size_t i = 0; i < others.size(); i++) {
      others1[i] = others.at(i).first;
      others2[i] = others.at(i).second;
    }
    auto rst1 = MpcAdapters<T, schedulerId>::batchingWith(src.first, others1);
    auto rst2 = MpcAdapters<U, schedulerId>::batchingWith(src.second, others2);
    return {rst1, rst2};
  }

  static std::vector<SecBatchType> unbatching(
      const SecBatchType& src,
      std::shared_ptr<std::vector<uint32_t>> unbatchingStrategy) {
    auto rst1 =
        MpcAdapters<T, schedulerId>::unbatching(src.first, unbatchingStrategy);
    auto rst2 =
        MpcAdapters<U, schedulerId>::unbatching(src.second, unbatchingStrategy);
    std::vector<SecBatchType> rst(rst1.size());
    for (size_t i = 0; i < rst.size(); i++) {
      rst[i] = {rst1.at(i), rst2.at(i)};
    }
    return rst;
  }
};

} // namespace fbpcf::mpc_std_lib::util

//#include "fbpcf/mpc_std_lib/util/pair_impl.h"
