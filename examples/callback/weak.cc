// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <functional>
#include "gtest/gtest.h"

struct Good : std::enable_shared_from_this<Good> {
  std::shared_ptr<Good> getptr() {
    return shared_from_this();
  }
};

struct Bad {
  std::shared_ptr<Bad> getptr() {
    return std::shared_ptr<Bad>(this);
  }
  ~Bad() {
    std::cout << "Bad::~Bad() called\n";
  }
};

TEST(WeakCallback, Example) {
  // Good: the two shared_ptr's shared the same object
  std::shared_ptr<Good> gp1 = std::make_shared<Good>();
  std::shared_ptr<Good> gp2 = gp1->getptr();
  std::cout << "gp2.use_count = " << gp2.use_count() << '\n';


  // Bad: shared_from_this is called without having std::shared_ptr
  // owning the caller
  try {
    Good not_so_good;
    std::shared_ptr<Good> gp1 = not_so_good.getptr();
  } catch (std::bad_weak_ptr &e) {
    std::cout << e.what() << '\n';
  }

  // Bad, each shared_ptr links it's the only owner of the object
  std::shared_ptr<Bad> bp1 = std::make_shared<Bad>();
  //std::shared_ptr<Bad> bp2 = bp1->getptr();
  //std::cout << "bp2.use_count() = " << bp2.use_count() << '\n';
  // UB: double-delete of Bad
}

class Stock {
public:
  Stock(std::string k) : key_(std::move(k)) {}
  std::string key() { return key_; };
private:
  std::string key_;
};

class StockFactory : public std::enable_shared_from_this<StockFactory> {
public:
  std::shared_ptr<Stock> get(std::string key) {
    std::shared_ptr<Stock> ret;
    std::lock_guard<std::mutex> lock(mutex_);
    std::weak_ptr<Stock> &ptr = stocks_[key];
    ret = ptr.lock();
    if (!ret) {
      ret = std::shared_ptr<Stock>(new Stock(key),
                                   std::bind(
                                       &StockFactory::weakDeleteCallback,
                                       weak_from_this(),
                                       std::placeholders::_1));
      ptr = ret;
    }
    return ret;
  }

  size_t size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return stocks_.size();
  }
private:
  static void weakDeleteCallback(const std::weak_ptr<StockFactory> &wkFactory,
                                 Stock *stock) {
    auto factory(wkFactory.lock());
    if (factory) {
      factory->deleteSock(stock);
    }
    delete stock;
  }
  void deleteSock(Stock *sock) {
    if (sock) {
      std::lock_guard<std::mutex> lock(mutex_);
      stocks_.erase(sock->key());
    }
  }
  std::mutex mutex_;
  std::map<std::string, std::weak_ptr<Stock>> stocks_;
};

// ref: en.cppreference.com/w/cpp/memory/enable_shared_from_this
// It is permitted to call shared_from_this only on a previously shared object,
// i.e. on an object managed by std::shared_ptr<T>
TEST(WeakCallback, Exception) {
  auto *factory = new StockFactory;

  // why this will throw? not just return nullptr?
  // this the behavior of shared_ptr<T> constructor, throw std::bad_weak_ptr
  // from a default-constructed weak_this
  try {
    factory->shared_from_this();
  } catch (std::bad_weak_ptr &e) {

  } catch (...) {
    FAIL() << "Expected: std::bad_weak_ptr()\n";
  }
  delete factory;
}

// Stock destructs first
TEST(WeakCallback, Stock) {
  auto factory = std::make_shared<StockFactory>();
  {
    auto ibm = factory->get("NYSE:IBM");
    auto ibm2 = factory->get("NYSE:IBM");

    EXPECT_EQ(ibm, ibm2);
    // stock destructs here
  }
  // factory destructs here
  EXPECT_EQ((unsigned) 0, factory->size());
}

// StockFactory destructs first
TEST(WeakCallback, StockFactory) {
  std::shared_ptr<Stock> ibm;
  {
    auto factory = std::make_shared<StockFactory>();
    ibm = factory->get("NYSE:IBM");

    auto ibm2 = factory->get("NYSE:IBM");

    EXPECT_EQ(ibm, ibm2);
    // factory destructs here
  }
  // stock destructs here
}