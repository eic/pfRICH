
#include <vector>

#ifndef _TOF_PET_MULTI_COLUMN_
#define _TOF_PET_MULTI_COLUMN_

class TofPetMultiColumn {
 public:
  TofPetMultiColumn(const char *fname, char delimiter = ' ');
  ~TofPetMultiColumn();
  
  unsigned GetDataPointCount( void ) const { return mData.size(); };
  std::vector<std::pair<double, double> > GetDataPoints(unsigned c1, unsigned c2);
  std::vector<double> GetDataPoints(unsigned c1);

 private:
  unsigned mColumnCount;
  // FIXME: this storage is redundant; but ok;
  std::vector<std::vector<double> > mData;
};

#endif
