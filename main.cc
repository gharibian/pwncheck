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
    int cnt = ssha1.ValCount(val);

    std::cout << val << " : " << hash << " : " << cnt << std::endl;

    if (cnt > 0) {
      found = true;
    }
  }

  return found ? 0 : 1;
}
