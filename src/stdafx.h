#pragma once
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
#define field_size_EFS EFS_ED25519
#define field_size_EGS EGS_ED25519
#define AESKEY AESKEY_ED25519
#define MAX_SLOTS 3
#define MAX_SLOTS_DATA_SIZE 66 
#ifndef PORT_SEND
#define PORT_SEND 5200
#define SECURITY_LEVEL 6
#endif
