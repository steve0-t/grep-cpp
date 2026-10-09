# file_parser_cpp

`file_parser_cpp` is a small POSIX command-line program that searches files under a directory for a case-sensitive text substring. It walks subdirectories recursively and uses a pool of worker threads to scan regular files.

## Build

Requirements: a C++ compiler, [Meson](https://mesonbuild.com/), and Ninja.

```sh
meson setup build
meson compile -C build
```

The executable is written to `build/file_parser_cpp`. If you use Nix, `nix develop` opens the development shell declared by `flake.nix`; build the project with the same Meson commands inside it.

## Usage

```sh
./build/file_parser_cpp <directory> <search-string>
```

For example, search the generated sample tree for a phrase:

```sh
./build/file_parser_cpp test_dirs "Lorem ipsum"
```

The program prints the worker count when it starts and prints `Found <search-string> in <path>` for files where it detects a match. It stops scheduling work after a match is found; workers already scanning files can report additional matches at about the same time. A search with no matches produces no result message, and the exit status does not distinguish that case from a match.

The input must be a directory. Although the usage text in the executable says `<file/folder>`, the current implementation opens the argument as a directory, so passing a file does not search that file. Matching is case-sensitive. Directory traversal uses POSIX directory-entry types and does not follow symlinks.

## Generated sample data

`generate_test_dirs.sh` creates nested directories containing text files with repeated sample text. Run it from the repository root as a non-root user:

```sh
./generate_test_dirs.sh 1
```

The supported levels generate the following directory and file counts:

| Level | Directories | Files |
| --- | ---: | ---: |
| `1` | 250 | 12,500 |
| `2` | 500 | 50,000 |
| `3` | 1,000 | 200,000 |

The script writes into `test_dirs/`; generated data is ignored by Git. It does not clear that directory before generating, so files left by an earlier, larger run can remain. There is no separate automated test suite configured in Meson.

## Implementation notes

The worker count comes from `std::thread::hardware_concurrency()`. Files are read in fixed 256-byte chunks. The current reader does not carry bytes between chunks, so a search string split across a chunk boundary can be missed; a full 256-byte read also places the string terminator beyond the buffer. This implementation is intended as a small text-search example rather than a robust search utility for arbitrary or binary files.

This project is licensed under the [MIT License](LICENSE).
