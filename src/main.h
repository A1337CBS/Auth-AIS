/* 
  main.h 
  @authors Ahmed Aziz, Pietro Tedeschi, Savio Sciancalepore, Roberto Di Pietro
  @Description: A program for implementing AIS_CAESAR Protocol PoC
  @version 1.0 25/02/19
**/
#pragma once
#ifndef AIS_CAESAR_MAIN_H_
#define AIS_CAESAR_MAIN_H_
#include "core-master/cpp/randapi.h"
#include <random>
#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>
#include "core-master/cpp/ecdh_ED25519.h"
#include "BloomFilter.h"
#include "ais_receiver/ais_rx.h"
#include <unistd.h>
#include <ios>

using namespace std;
using namespace B256_56;
using namespace ED25519;

/***********************************************************
 *                                                         *
 *                  Macros as required                     *
 *                                                         *
 * *********************************************************/
#define field_size_EFS EFS_ED25519
#define field_size_EGS EGS_ED25519
#define AESKEY AESKEY_ED25519
#ifndef SECURITY_LEVEL
  #define SECURITY_LEVEL 1
#endif
#define MAX_SLOTS 3
#define MAX_SLOTS_DATA_SIZE 66 
#ifndef PORT_SEND
#define PORT_SEND 5200
#endif

#define WRITE_TESTS false

/***********************************************************
 *                                                         *
 *                  Common Functions                       *
 *                                                         *
 * *********************************************************/

/**	
 *  @brief Get memory used by process and save it in supplied vars
 *  @param double& vm_usage Virtual memory usage
 *  @param double& resident_set Physical memory usage
 *  @return void
 */
void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

//K_input = input Key, K_output=output key, n=number of times to hash
/**	
 *  @brief Generate keychain by hashing input consecutively n times
 *  @param octet *K_input Octet that points to input
 *  @param octet *K_output Octet that will store output
 *  @param int n number of times to hash
 *  @param int key_size truncates hash to supplied key_size bytes
 *  @return void
 */
int generateKeyChainCommit(octet *K_input, octet *K_output, int n, int key_size){ //octet *output

  //create a temp octet
  char tempOctet_K_size[2 * field_size_EGS + 1];
  octet tempOctet = {0, sizeof(tempOctet_K_size), tempOctet_K_size};
  //copy K_input into K_output
  OCT_copy(K_output, K_input);

  //Use hash function iteratively
  for (int j=0; j<n; j++){

    SPhash(MC_SHA2, SHA256, &tempOctet, K_output);
    //copy temp octet into K_output
    if (key_size>0){
      //truncate hash to keysize
      tempOctet.len = key_size;
    }
    OCT_copy(K_output, &tempOctet);
    
  }

  return 0;
}


#endif //AIS_CAESAR_MAIN_H_