/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H
#define GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include <grpc++/support/config.h>

#include "src/cpp/server/thread_pool_interface.h"

namespace grpc {

class DynamicThreadPool final : public ThreadPoolInterface {
 public:
  explicit DynamicThreadPool(int reserve_threads);
  ~DynamicThreadPool();

  void Add(const std::function<void()>& callback) override;

 private:
  class DynamicThread {
   public:
    DynamicThread(DynamicThreadPool* pool);
    ~DynamicThread();

   private:
    DynamicThreadPool* pool_;
    std::unique_ptr<std::thread> thd_;
    void ThreadFunc();
  };
  std::mutex mu_;
  std::condition_variable cv_;
  std::condition_variable shutdown_cv_;
  bool shutdown_;
  std::queue<std::function<void()>> callbacks_;
  int reserve_threads_;
  int nthreads_;
  int threads_waiting_;
  std::list<DynamicThread*> dead_threads_;

  void ThreadFunc();
  static void ReapThreads(std::list<DynamicThread*>* tlist);
};

}  // namespace grpc

#endif  // GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H
