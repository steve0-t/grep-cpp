#include "../include/main.h"

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

  // #if defined(_WIN32)
  //   if (path.back() != '\\')
  //     path.append("\\");
  // #elif defined(__unix__)
  //   if (path.back() != '/')
  //     path.append("/");
  // #endif

  std::cout << "this is path: " << path << '\n';

  Engine engine(path, needle);

  return 0;
}
