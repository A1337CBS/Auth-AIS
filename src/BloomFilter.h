#include <vector>
#include <bitset>
#include <cstdint> 
#include <array>
#include <string>

struct BloomFilter {
  BloomFilter(uint64_t size, uint8_t numHashes);
  ~BloomFilter();
  
  void add(const uint8_t *data, std::size_t len);
  bool possiblyContains(const uint8_t *data, std::size_t len) const;

  int Encode_BloomFilter(int);
  void Decode_BloomFilter();
  std::string get_string();
  void to_bits(std::string input);

private:
  uint8_t m_numHashes;
  unsigned char *m_compressedData;
  std::vector<bool> m_bits;
};