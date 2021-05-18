#include <string>

auto SHA1(const std::string &val) -> std::string;

class SortedSHA1Search {
public:
  SortedSHA1Search() = default;

  auto Open(const std::string &fname) -> bool;

  auto ValCount(const std::string &val) -> int;

  auto HashCount(const std::string &val) -> int;

  ~SortedSHA1Search();

private:
  auto HashCount(const std::string &val, size_t low, size_t high) -> int;

  char *_addr{nullptr};
  int _fd{-1};
  size_t _length{0};
};
