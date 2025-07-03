#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#define _POSIX_C_SOURCE 200809L
#include <condition_variable>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <string.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

typedef struct file_ref {
  std::string filename;
  size_t dir_index;
} file_ref_t;

class Engine {
public:
  Engine(size_t nthreads, const std::string &search);
  ~Engine();
  void parse_dirs(const std::string &path);
  void add_file(const std::string &path, const std::string &filename);
  [[nodiscard]] std::size_t add_dir(const std::string &pathname);
  void thread_func();
  void parse_file(const std::string &pathname);
  void wait();

private:
  std::size_t m_nthreads;
  const std::string needle;
  std::atomic_bool stop;
  std::atomic_bool found;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::vector<std::thread> m_workers;

  std::queue<file_ref_t> file_names;
  std::unordered_map<std::string, std::size_t> dir_name_map;
  std::vector<std::string> dir_names;
};

#endif
