
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TofPetMultiColumn.h>

// -------------------------------------------------------------------------------------

TofPetMultiColumn::TofPetMultiColumn(const char *fname, char delimiter): mColumnCount(0)
{
  std::ifstream fin(fname);
  if (!fin.is_open()) return;

  std::vector<std::string> row; 
  std::string line, word;

  while (!fin.eof()) {
    row.clear(); 
  
    // Read an entire row and store it in a string variable 'line';
    getline(fin, line); 
    // Skip comments;
    if (line[0] == '#') continue;
    //std::cout << line;

    // Used for breaking words;
    std::stringstream s(line); 
    
    // Read every column data of a row and store it in a string variable 'word';
    while (getline(s, word, delimiter))
      // Add all the column data of a row to a vector;
      row.push_back(word); 
    //printf("%d\n", row.size());

    if (!row.size()) return;

    if (!mColumnCount) mColumnCount = row.size();
    if (row.size() != mColumnCount) return;
    {
      std::vector<double> data;

      for(unsigned iq=0; iq<mColumnCount; iq++)
	data.push_back(stof(row[iq]));

      mData.push_back(data);
    }
  } // while

  fin.close();
} // TofPetMultiColumn::TofPetMultiColumn()

// -------------------------------------------------------------------------------------

std::vector<std::pair<double, double> > TofPetMultiColumn::GetDataPoints(unsigned c1, unsigned c2)
{
  std::vector<std::pair<double, double> > data;

  if (c1 < mColumnCount && c2 < mColumnCount) {
    for(unsigned iq=0; iq<mData.size(); iq++) {
      const std::vector<double> &entry = mData[iq];

      data.push_back(std::pair<double, double>(entry[c1], entry[c2]));
    } //for iq
  } //if

  return data;
} // TofPetMultiColumn::GetDataPoints()

// -------------------------------------------------------------------------------------

std::vector<double> TofPetMultiColumn::GetDataPoints(unsigned c1)
{
  std::vector<double> data;

  if (c1 < mColumnCount) 
    for(unsigned iq=0; iq<mData.size(); iq++) 
      data.push_back(mData[iq][c1]);

  return data;
} // TofPetMultiColumn::GetDataPoints()

// -------------------------------------------------------------------------------------

