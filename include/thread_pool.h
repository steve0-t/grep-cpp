#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#define _POSIX_C_SOURCE 200809L

#include "queue.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <dirent.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool(size_t nthreads, const std::string &search);
  void parse_dirs(const std::string &path);
  ~ThreadPool();
  void thread_func();
  void parse_file(const std::string &pathname);
  void wait();
  Queue m_queue;

private:
  size_t m_nthreads;
  const std::string needle;
  std::atomic_bool stop;
  std::atomic_bool found;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::vector<std::thread> m_workers;
};

#endif
