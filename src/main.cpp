#include "../include/main.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

#define VERBOSE 1

Queue my_queue;

bool found = false;

void parse_file(const char *path, const char *search);
void parse_dirs(const char *path);
void print_message(const char *path, const char *search);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <file/folder> <search string>" << '\n';
    return 1;
  }

  const char *path = argv[1];
  const char *search = argv[2];

  parse_dirs(path);
  my_queue.print_queue_size();

  while (!found) {
    parse_file(my_queue.get_filename(), search);
    my_queue.dequeue();
  }

  return 0;
}

void parse_dirs(const char *path) {
  struct dirent *entity;
  DIR *dir = opendir(path);
  if (dir == nullptr) {
    return;
  }

  size_t path_len = strlen(path);
  // cout << path[path_len - 1] << endl;
  bool include_slash = (path[path_len - 1] == '/') ? false : true;

  while ((entity = readdir(dir)) != nullptr) {
    if (strcmp(entity->d_name, ".") == 0 || strcmp(entity->d_name, "..") == 0)
      continue;

    size_t d_name_len = strlen(entity->d_name);
    size_t pathname_len = (!include_slash) ? path_len + d_name_len + 1
                                           : path_len + d_name_len + 2;
    char *pathname = (char *)malloc(pathname_len);

    memcpy(pathname, path, path_len);
    if (include_slash)
      memcpy(pathname + path_len, "/", 1);
    memcpy(
        ((include_slash) ? (pathname + path_len + 1) : (pathname + path_len)),
        entity->d_name, d_name_len + 1);

    if (entity->d_type == DT_REG)
      my_queue.enqueue(pathname);

    if (entity->d_type == DT_DIR)
      parse_dirs(pathname);
    free(pathname);
  }
  closedir(dir);
  return;
}

void parse_file(const char *path, const char *search) {
  if (!path || !search) {
    fprintf(stderr, "Path or search not specified\n");
    return;
  }
  FILE *fp = fopen(path, "r");
  if (!fp) {
    fprintf(stderr, "Error! Could not open %s.\n", path);
    return;
  }

  char line[256] = {0};
  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, search)) {
      found = true;
      cout << "Found " << search << " in " << path << endl;
      break;
    }
  }
  fclose(fp);
}

/*
void print_message(const char *path, const char *search) {
  cout << "  ";
  for (size_t i = 0; i < message_len; i++)
    cout << '-';
  cout << '\n';

  cout << "| searching for " << search << " in " << path << " |\n";

  cout << "  ";
  for (size_t i = 0; i < message_len; i++)
    cout << '-';
  cout << '\n';
}
*/
