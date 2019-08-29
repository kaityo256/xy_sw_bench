#include <fstream>
#include "mpistream.h"

MPIStream mout;
//----------------------------------------------------------------------
void
MPIStream::SaveToFile(std::string filename) {
  if (0 != rank) {
    return;
  }
  std::ofstream ofs(filename.c_str());
  ofs << os_backup.str() << std::endl;
}
//----------------------------------------------------------------------
void
MPIStream::AppendToFile(std::string filename) {
  if (0 != rank) {
    return;
  }
  std::ofstream ofs(filename.c_str(), std::ios_base::app);
  ofs << os_backup.str() << std::endl;
}
//----------------------------------------------------------------------


