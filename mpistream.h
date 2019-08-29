#pragma once

#include <iostream>
#include <sstream>
#include <string>

class MPIStream {
private:
  int rank;
  std::ostringstream oss;
  std::ostringstream os_backup;

public:
  MPIStream(void) {
    rank = 0;
  };
  void SetRank(int r) {
    rank = r;
  };
  template <class T>
  MPIStream &operator<<(const T &a) {
    oss << a;
    os_backup << a;
    return *this;
  }
  MPIStream &operator<<(std::ostream &(*pf)(std::ostream &)) {
    os_backup << pf;
    if (0 == rank) {
      std::cout << oss.str() << pf;
    }
    oss.str("");
    oss.clear();
    return *this;
  };
  void SaveToFile(std::string filename);
  void AppendToFile(std::string filename);
};
extern MPIStream mout;