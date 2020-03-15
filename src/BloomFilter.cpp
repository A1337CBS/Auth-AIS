#include "BloomFilter.h"
#include <iostream>
#include <fstream>
#include "smhasher-master/src/MurmurHash3.h"
#define SECURITY_LEVEL 6

BloomFilter::BloomFilter(uint64_t size, uint8_t numHashes)
      : m_bits(size),
        m_numHashes(numHashes) { m_compressedData = new unsigned char[size]; }
       
BloomFilter::~BloomFilter(){// and do not forget to delete it later
delete[] m_compressedData;}

std::array<uint64_t, 2> hash(const uint8_t *data, std::size_t len) {
  std::array<uint64_t, 2> hashValue;
  MurmurHash3_x64_128(data, len, 0, hashValue.data());

  return hashValue;
}

inline uint64_t nthHash(uint8_t n,
                        uint64_t hashA,
                        uint64_t hashB,
                        uint64_t filterSize) {
    return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const uint8_t *data, std::size_t len, bool write_test=false) {
  
 // auto start = std::chrono::high_resolution_clock::now();
  auto hashValues = hash(data, len);
 // auto elapsed = std::chrono::high_resolution_clock::now() - start;
 // long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
 // printf("\n Time taken to hash element for B.F. : %lld nanoseconds\n\n",  nanoseconds);
  
  //tests
  
  if(write_test){
    std::ofstream outfile;
    outfile.open("test_1_sec_lvl_"+std::to_string(SECURITY_LEVEL)+".csv", std::ios::out | std::ios::app );
    outfile << "\n Time taken to hash element for B.F. in nanoseconds \n";
    for (int i=0; i<505; i++)
    {
          auto start = std::chrono::high_resolution_clock::now();
          auto hashValues = hash(data, len);
          auto elapsed = std::chrono::high_resolution_clock::now() - start;
          long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
        //  printf("\n Time taken to add/map element in B.F. : %lld nanoseconds\n\n",  nanoseconds);
          
          // write inputted data into the file.
          outfile << nanoseconds << ", ";
                  
    }
    // close the opened file.
    outfile.close();
  }
  

  for (int n = 0; n < m_numHashes; n++) {
      m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
  }
}

bool BloomFilter::possiblyContains(const uint8_t *data, std::size_t len) const {
  auto hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) {
          return false;
      }
  }

  return true;
}


// - - Inclusion - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <stdlib.h>

#include "FastAC_fix-nh/FastAC/arithmetic_codec.h"


// - - Constants - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const unsigned BufferSize = 65536;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Implementations - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


int BloomFilter::Encode_BloomFilter(int buffer_size=50)
{

    // Convert to a bitset:
 // std::string os;
 // copy(m_bits.begin(), m_bits.end(), ostream_iterator<bool>(os, ""));
 // bitset<10> bs(os.str());
 // cout << "Bitset:\n" << bs << endl;

  // buffer for data file data
  unsigned char *data = new unsigned char[BufferSize];
  // set data model
  Adaptive_Bit_Model dm;
  //for (unsigned m = 0; m < NumModels; m++) dm[m].set_alphabet(256);

  Arithmetic_Codec encoder(BufferSize, m_compressedData);                  // set encoder buffer

  encoder.start_encoder();

 

  for (unsigned p = 0; p < m_bits.size(); p++) {                       // compress data
    unsigned int vOut = m_bits[p]?1:0;
    encoder.encode( vOut, dm);
  }

  //  printf(" UnCompressed file size = %ld bytes\n", m_bits.size());
    //encoder.write_to_file(code_file);  // stop encoder & write compressed data
   return encoder.stop_encoder();

                                                          // done: close files
/*
  unsigned data_bytes = ftell(data_file), code_bytes = ftell(code_file);
  printf(" Compressed file size = %d bytes (%6.2f:1 compression)\n",
    code_bytes, double(data_bytes) / double(code_bytes));


  delete [] data;*/
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

std::string ToBits(unsigned char byte)
{
    return std::bitset<8>(byte).to_string();
}

std::string BloomFilter::get_string() {
    std::string ret = "";
    for ( bool bit : m_bits ) {
        unsigned int vOut = bit?1:0;
        ret += std::to_string(vOut);
    }
    return ret;
}

void BloomFilter::to_bits(std::string input) {

  std::string temp = input;

  if (temp.length()> m_bits.size()){
    printf("Error: input length greater than B.F. size");
    return;
  }
	for (int i = 0; i < temp.length(); i++) {
        bool vOut = (temp[i]=='1')?true:false;
        m_bits[i] = vOut;
	}
  return;  
}

void BloomFilter::Decode_BloomFilter()
{

  // buffer for data file data
  unsigned char * data = new unsigned char[BufferSize];
                                                            // set data models
  Adaptive_Bit_Model dm;

  Arithmetic_Codec decoder(BufferSize, m_compressedData);                  // set encoder buffer

 // for(int i=0; i<10; i++)
 //   printf("\n data %d", static_cast<int>(m_compressedData[i]));


  // decompress data
    decoder.start_decoder();
    for (unsigned p = 0; p < m_bits.size(); p++) {
      bool vOut = decoder.decode(dm)?true:false;
      
      bool check = m_bits[p] == vOut?true:false;
      //printf(" %d", check);
    }
    
    decoder.stop_decoder();

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */