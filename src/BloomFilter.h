#include <vector>
#include <bitset>
#include <cstdint> 
#include <array>
#include <string>
#include <chrono>
#include <stdlib.h>
#include "FastAC_fix-nh/FastAC/arithmetic_codec.h"
#include <iostream>
#include <fstream>
#include "smhasher-master/src/MurmurHash3.h"
#ifndef SECURITY_LEVEL
  #define SECURITY_LEVEL 1
#endif
#define WRITE_TESTS false

struct BloomFilter {
  BloomFilter(uint64_t size, uint8_t numHashes);
  ~BloomFilter();
  
  void add(const uint8_t *data, std::size_t len);
  bool possiblyContains(const uint8_t *data, std::size_t len) const;

  std::string get_string();
  void to_bits(std::string input);
  
  /* FUTURE WORK
  int Encode_BloomFilter(int);
  void Decode_BloomFilter();
  */

private:
  uint8_t m_numHashes;
  unsigned char *m_compressedData;
  std::vector<bool> m_bits;
};