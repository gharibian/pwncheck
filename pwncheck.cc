#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#include <sha1.h>

#include <algorithm>
#include <string>

#include "pwncheck.hh"

auto SHA1(const std::string &val) -> std::string {
  if (val.length() == 0u) {
    return "";
  }

  std::string hash(SHA1_DIGEST_STRING_LENGTH - 1, '\0');
  SHA1Data(reinterpret_cast<const uint8_t *>(val.c_str()), val.length(),
           const_cast<char *>(hash.data()));
  std::transform(hash.begin(), hash.end(), hash.begin(), ::toupper);

  return hash;
}

auto SortedSHA1Search::Open(const std::string &fname) -> bool {
  _fd = open(fname.c_str(), O_RDONLY | O_CLOEXEC);

  if (_fd == -1) {
    return false;
  }

  struct stat sb {};
  if (fstat(_fd, &sb) == -1) {
    return false;
  }
  _length = sb.st_size;

  _addr = static_cast<char *>(
      ::mmap(nullptr, _length, PROT_READ, MAP_PRIVATE, _fd, 0));
  return _addr != MAP_FAILED;
}

auto SortedSHA1Search::ValCount(const std::string &val) -> int {
  return HashCount(SHA1(val));
}

auto SortedSHA1Search::HashCount(const std::string &val) -> int {
  return HashCount(val, 0, _length - 1);
}

SortedSHA1Search::~SortedSHA1Search() {
  ::munmap(_addr, _length);
  ::close(_fd);

  _fd = -1;
  _length = 0;
  _addr = nullptr;
}

auto SortedSHA1Search::HashCount(const std::string &val, size_t low, size_t high)
    -> int {
  if (high < low || (high - low) < SHA1_DIGEST_STRING_LENGTH) {
    return 0;
  }

  size_t mid = (high - low) / 2 + low;
  while (mid != 0 && *(_addr + mid - 1) != '\n') {
    mid--;
  }

  std::string needle(_addr + mid, SHA1_DIGEST_STRING_LENGTH - 1);
  int c = needle.compare(val);
  if (c == 0) {
    char *cbegin =
        _addr + std::min(_length - 1, mid + SHA1_DIGEST_STRING_LENGTH);
    char *cend = cbegin;
    while (cend < (_addr + _length) && *cend != '\n') {
      cend++;
    }

    std::string count(cbegin, cend);
    return std::stoi(count);
  } else if (c > 0) {
    return HashCount(val, low, mid == 0 ? 0 : mid - 1);
  } else {
    while (mid < high && *(_addr + mid) != '\n') {
      mid++;
    }
    mid++;
    return HashCount(val, mid, high);
  }
}
