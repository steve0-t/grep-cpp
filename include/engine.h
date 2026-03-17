#pragma once
#include <utility>
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#define _POSIX_C_SOURCE 200809L
#include <condition_variable>

#include <atomic>
#include <cstddef>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

typedef struct file_ref {
  std::string filename;
  size_t dir_index;
} file_ref_t;

class Engine {
public:
  Engine(const std::string &path, const std::string &search);
  ~Engine();

private:
  void parse_dirs(const std::string &path);

  void add_file(const int &fd, const std::string &pathname);

  void thread_func();

  void parse_file(const std::pair<int, std::string> &pair);

  void wait();

private:
  const std::string path;
  const std::string needle;
  std::size_t m_nthreads;

  std::atomic_bool stop;
  std::atomic_bool found;
  std::atomic_int m_active;

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::vector<std::thread> m_workers;

  std::queue<std::pair<int, std::string>> fds;
};

#endif
