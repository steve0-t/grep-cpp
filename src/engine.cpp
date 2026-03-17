#include "../include/engine.h"

Engine::Engine(const std::string &path, const std::string &search)
    : path(path), needle(search), stop(false), found(false), m_active(0) {
  m_nthreads = std::thread::hardware_concurrency();
  m_workers.reserve(m_nthreads);
  for (size_t i = 0; i < m_nthreads; i++)
    m_workers.emplace_back(&Engine::thread_func, this);

  std::cout << "Workers " << std::to_string(m_nthreads) << "\n";
  parse_dirs(path);
  wait();
};

Engine::~Engine() {
  stop = true;
  m_cv.notify_all();
  for (std::thread &worker : m_workers)
    worker.join();
}

void Engine::parse_dirs(const std::string &path) {
  struct dirent *entity;
  DIR *dir = opendir(path.c_str());
  if (dir == nullptr)
    return;

  while ((entity = readdir(dir)) != nullptr) {
    if (!strcmp(entity->d_name, ".") || !strcmp(entity->d_name, ".."))
      continue;

    std::string full_path = path + "/" + entity->d_name;

    if (entity->d_type == DT_REG) {
      int fd = open(full_path.c_str(), O_RDONLY);
      add_file(fd, full_path);
    } else if (entity->d_type == DT_DIR) {
      // std::cout << "new dir found: " << entity->d_name << "\n";
      parse_dirs(full_path);
    }
  }
  closedir(dir);
  return;
}

void Engine::add_file(const int &fd, const std::string &pathname) {
  std::lock_guard<std::mutex> lg(m_mutex);
  fds.emplace(std::make_pair(fd, pathname));
  m_cv.notify_one();
}

void Engine::thread_func() {
  while (true) {
    std::unique_lock<std::mutex> ul(m_mutex);
    m_cv.wait(ul, [this] { return !fds.empty() || stop || found; });
    if (stop || found)
      return;

    // Try switching and see what ONE KEY PRESS does
    // THIS IS INSANITY !!!
    // const std::pair<int, std::string> &file_ref = fds.front();
    const std::pair<int, std::string> file_ref = fds.front();
    // std::cout << "Parsing " << file_ref.second << "\n";
    m_active++;
    fds.pop();

    // std::cout << "Is fds empty: " << fds.empty() << "\n";
    if (fds.empty())
      m_cv.notify_all();

    ul.unlock();

    parse_file(file_ref);
    m_active--;
    m_cv.notify_all();
  }
}

void Engine::parse_file(const std::pair<int, std::string> &pair) {
  char line[256] = {0};
  int bytes_read = 0;
  while ((bytes_read = read(pair.first, line, sizeof(line))) > 0) {
    line[bytes_read] = 0;
    if (strstr(line, needle.c_str())) {
      std::cout << "Found " << needle << " in " << pair.second << "\n";
      found = true;
      m_cv.notify_all();
      break;
    }
  }
  close(pair.first);
}

void Engine::wait() {
  std::unique_lock<std::mutex> ul(m_mutex);
  m_cv.wait(ul,
            [this] { return (fds.empty() && m_active <= 0) || found || stop; });
}
