#include <chrono>
#include <iostream>

#include "pwncheck.hh"

auto main(int argc, char *argv[]) -> int {
  if (argc < 3) {
    ::fprintf(stderr, "%s [FILE] [PASSWORD...]\n", argv[0]);
    return 2;
  }

  SortedSHA1Search ssha1;
  if (!ssha1.Open(argv[1])) {
    ::perror("Unable to open file");
    return 2;
  }

  bool found = false;
  for (int i = 2; i < argc; i++) {
    std::string val = argv[i];
    std::string hash = SHA1(val);
    auto start = std::chrono::high_resolution_clock::now();
    int cnt = ssha1.HashCount(hash);
    auto stop = std::chrono::high_resolution_clock::now();
    auto dur =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
            .count();

    std::cout << val << " : " << hash << " : " << cnt << " : " << dur << " us"
              << std::endl;

    if (cnt > 0) {
      found = true;
    }
  }

  return found ? 0 : 1;
}
