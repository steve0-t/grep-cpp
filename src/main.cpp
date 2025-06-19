#include "../include/main.h"
#include <atomic>
#include <cstddef>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

std::atomic_size_t alloc_counter{0};
void *operator new(size_t size) {
  alloc_counter += 1;
  return malloc(size);
}

ThreadPool::ThreadPool(size_t nthreads, const std::string &search)
    : m_nthreads(nthreads), needle(std::move(search)), stop(false),
      found(false) {
  m_workers.reserve(m_nthreads);
  for (size_t i = 0; i < m_nthreads; i++)
    m_workers.emplace_back(&ThreadPool::thread_func, this);
};

void ThreadPool::parse_dirs(const std::string &path) {
  struct dirent *entity;
  DIR *dir = opendir(path.c_str());
  if (dir == nullptr) {
    return;
  }

  bool include_slash = (path.back() == '/') ? false : true;

  while ((entity = readdir(dir)) != nullptr) {
    if (strcmp(entity->d_name, ".") == 0 || strcmp(entity->d_name, "..") == 0)
      continue;

    std::string pathname = (include_slash) ? (path + "/" + entity->d_name)
                                           : (path + entity->d_name);

    if (entity->d_type == DT_REG) {
      m_queue.enqueue(std::move(pathname));
      m_cv.notify_one();
    }

    if (entity->d_type == DT_DIR)
      parse_dirs(pathname);
  }
  closedir(dir);
  return;
}

ThreadPool::~ThreadPool() {
  stop = true;
  m_cv.notify_all();
  for (std::thread &worker : m_workers)
    worker.join();
}

void ThreadPool::thread_func() {
  while (true) {
    std::unique_lock<std::mutex> ul(m_mutex);
    m_cv.wait(ul, [this] { return !m_queue.is_empty() || stop || found; });
    if (stop || found)
      return;
    std::string pathname = m_queue.get_filename();
    m_queue.dequeue();
    if (m_queue.is_empty())
      m_cv.notify_all();
    ul.unlock();
    parse_file(pathname);
  }
}

void ThreadPool::parse_file(const std::string &pathname) {
  FILE *fp = fopen(pathname.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "Error! Could not open %s.\n", pathname.c_str());
    return;
  }

  char line[256] = {0};
  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, needle.c_str())) {
      std::unique_lock<std::mutex> ul(m_mutex);
      cout << "Found " << ThreadPool::needle << " in " << pathname << '\n';
      found = true;
      ul.unlock();
      m_cv.notify_all();
      break;
    }
  }
  fclose(fp);
}

void ThreadPool::wait() {
  std::unique_lock<std::mutex> ul(m_mutex);
  m_cv.wait(ul, [this] { return m_queue.is_empty() || found || stop; });
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <file/folder> <search string>" << endl;
    return 1;
  }

  const std::string path = argv[1];
  const std::string search = argv[2];

  size_t nthreads = std::thread::hardware_concurrency();
  if (nthreads == 0)
    nthreads = 2;

  ThreadPool pool(nthreads, search);
  pool.parse_dirs(path);

  pool.wait();

  cout << "Mem allocs: " << alloc_counter << '\n';

  return 0;
}
