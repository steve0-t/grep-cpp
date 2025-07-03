#include "../include/main.h"
#include <cstring>

std::atomic_size_t alloc_counter{0};
std::atomic_size_t total_alloc_size{0};
void *operator new(size_t size) {
  alloc_counter += 1;
  total_alloc_size += size;
  return malloc(size);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <file/folder> <search string>"
              << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  std::string needle(argv[2]);
#ifdef _WIN32
  if (path.back() != '\\') {
    path.push_back('\\');
  }
#endif
#ifdef __unix__
  if (path.back() != '/') {
    path.push_back('/');
  }
#endif

  std::cout << "this is path: " << path << '\n';

  size_t nthreads = std::thread::hardware_concurrency();
  if (nthreads == 0)
    nthreads = 2;

  Engine engine(nthreads, needle);
  engine.parse_dirs(std::move(path));

  engine.wait();

  std::cout << "Mem allocs: " << alloc_counter << '\n';
  std::cout << "Total alloc size: " << total_alloc_size << '\n';

  return 0;
}
