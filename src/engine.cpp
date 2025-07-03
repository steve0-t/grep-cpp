#include "../include/engine.h"
#include <cstring>
#include <iomanip>

Engine::Engine(size_t nthreads, const std::string &search)
    : m_nthreads(nthreads), needle(search), stop(false), found(false) {
  dir_names.reserve((64 * sizeof(char)) * 1024);
  dir_name_map.reserve((64 * sizeof(char)) * 1024);

  m_workers.reserve(m_nthreads);
  for (size_t i = 0; i < m_nthreads; i++)
    m_workers.emplace_back(&Engine::thread_func, this);
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
  if (dir == nullptr) {
    return;
  }

  while ((entity = readdir(dir)) != nullptr) {
    if (strcmp(entity->d_name, ".") == 0 || strcmp(entity->d_name, "..") == 0)
      continue;

    if (entity->d_type == DT_REG) {
      add_file(path, std::move(entity->d_name));
    } else if (entity->d_type == DT_DIR) {
      size_t d_name_len = strlen(entity->d_name);
      std::string pathname;

      std::cout << "for " << std::quoted(entity->d_name)
                << " testing: " << entity->d_name[d_name_len - 1] << '\n';
      if (entity->d_name[d_name_len - 1] != '/') {
        pathname.reserve(path.length() + d_name_len + 1);
        pathname = path + entity->d_name + "/";
      } else {
        pathname.reserve(path.length() + d_name_len);
        pathname = path + entity->d_name;
      }

      std::size_t index = add_dir(std::move(pathname));
      std::cout << "new state of pathname: " << pathname << '\n';
      parse_dirs(dir_names[index]);
    }
  }
  closedir(dir);
  return;
}

void Engine::add_file(const std::string &path, const std::string &filename) {
  std::lock_guard<std::mutex> lg(m_mutex);
  auto index = dir_name_map.find(path);
  std::cout << "adding file: " << filename << '\n';
  if (index != dir_name_map.end()) {
    file_names.push({filename, index->second});
  } else {
    file_names.push({filename, SIZE_MAX});
  }
  m_cv.notify_one();
}

std::size_t Engine::add_dir(const std::string &pathname) {
  std::lock_guard<std::mutex> lg(m_mutex);
  auto does_exist = dir_name_map.find(pathname);
  std::cout << "inspecting: " << pathname << '\n';
  std::size_t index = 0;
  if (does_exist == dir_name_map.end()) {
    std::cout << "adding dir: " << pathname << '\n';
    index = dir_names.size();
    dir_names.emplace_back(pathname);
    dir_name_map[pathname] = index;
    return index;
  } else {
    index = does_exist->second;
  }
  return index;
}

void Engine::thread_func() {
  while (true) {
    std::unique_lock<std::mutex> ul(m_mutex);
    m_cv.wait(ul, [this] { return !file_names.empty() || stop || found; });
    if (stop || found)
      return;

    const file_ref_t &file_ref = file_names.front();
    std::string full_path;
    full_path.reserve(128);
    if (file_ref.dir_index != SIZE_MAX) {
      std::cout << dir_names[file_ref.dir_index] << file_ref.filename << '\n';
      std::string full_path =
          (dir_names[file_ref.dir_index] + file_ref.filename);
    } else {
      std::string full_path = std::move(file_ref.filename);
    }
    file_names.pop();

    if (file_names.empty())
      m_cv.notify_all();

    ul.unlock();

    std::cout << "parsing: " << full_path << '\n';
    parse_file(full_path);
  }
}

void Engine::parse_file(const std::string &pathname) {
  FILE *fp = fopen(pathname.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "Error! Could not open %s.\n", pathname.c_str());
    return;
  }

  char line[256] = {0};
  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, needle.c_str())) {
      std::unique_lock<std::mutex> ul(m_mutex);
      std::cout << "Found " << needle << " in " << pathname << '\n';
      found = true;
      ul.unlock();
      m_cv.notify_all();
      break;
    }
  }
  fclose(fp);
}

void Engine::wait() {
  std::unique_lock<std::mutex> ul(m_mutex);
  m_cv.wait(ul, [this] { return file_names.empty() || found || stop; });
}
