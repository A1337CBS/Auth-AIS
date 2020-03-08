/* 
  main.cpp 
  @authors Ahmed Aziz
  @Description: Transmitter program for implementing the AIS_Tesla Protocol PoC
  @version 1.0 25/01/19

  Compile command, add flag -DPORT_SEND or -DPORT_RECEIVE to set another port, 
  -DGEN_KEYS= true or false to set whether to generate keys or not
  g++ -O2 main.cpp core-master/cpp/core.a -o main
  g++ -O2 main.cpp core-master/cpp/core.a ./ais_receiver/*.c -o main
  g++ -O2 main.cpp BloomFilter.cpp smhasher-master/src/MurmurHash3.cpp core-master/cpp/core.a ./ais_receiver/*.c -o main
  g++ -O2 main.cpp BloomFilter.cpp smhasher-master/src/MurmurHash3.cpp core-master/cpp/core.a ./ais_receiver/*.c FastAC_fix-nh/FastAC/arithmetic_codec.cpp -o main
**/
/*Todo
timeslot
compression
change name from main to transmitter
fix message 4 encoding
*/
//Generate key chain commit
#include "core-master/cpp/randapi.h"
#include <random>
#include <bitset>
#include <iostream>
#include <chrono>
#include "core-master/cpp/ecdh_ED25519.h"
#include "BloomFilter.h"
#include "ais_receiver/ais_rx.h"
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
#endif

/**	
 *  @brief Convert hex string to binary string
 *  @param string &s 
 *  @return binary string
 */
string hextobin(const string &s){
    string out;
    for(auto i: s){
        uint8_t n;
        if(i <= '9' and i >= '0')
            n = i - '0';
        else
            n = 10 + i - 'A';
        for(int8_t j = 3; j >= 0; --j)
            out.push_back((n & (1<<j))? '1':'0');
    }

    return out;
}

/**	
 *  @brief Convert binary string to hex string
 *  @param string &s 
 *  @return hex string
 */
string bintohex(const string &s){
    string out;
    for(uint i = 0; i < s.size(); i += 4){
        int8_t n = 0;
        for(uint j = i; j < i + 4; ++j){
            n <<= 1;
            if(s[j] == '1')
                n |= 1;
        }

        if(n<=9)
            out.push_back('0' + n);
        else
            out.push_back('a' + n - 10);
    }

    return out;
}

/*  Functions used for AIS process itself   */

/**	@brief Create an AIS Message of type 8
 *
 *  @param int src_MMSI
 *  @param string payload
 *  @return binary string of message 6
 */
string encode_ais_message_8(string payload, int src_mmsi=247320162){ 
    string type = std::bitset<6>(8).to_string(); 
    string repeat = "00";
    string mmsi = std::bitset<30>(src_mmsi).to_string();
    string spare = "00";
    //application bits, 1 byte metadata are custom bits for self use
    string application_meta_bits = std::bitset<8>(0).to_string(); 
    string appid_dac = std::bitset<10>(0).to_string(); 
    string appid_fi = std::bitset<6>(50).to_string(); 
  
    return type+repeat+mmsi+spare+appid_dac+appid_fi+payload;
}

string encode_ais_message_4(int src_mmsi=247320162, float speed=0.1, float __long=9.72357833333333, float __lat=45.6910166666667, float __course=83.4, int __ts=38){
  string _type = std::bitset<6>(4).to_string(); 
	string _repeat = "00";										// repeat (directive to an AIS transceiver that this message should be rebroadcast.)
	string _mmsi = std::bitset<30>(src_mmsi).to_string();	// 30 bits (247320162)

  string _hour = std::bitset<5>(24).to_string();
	string _min = std::bitset<6>(60).to_string();
  string _sec = std::bitset<6>(60).to_string();


	string _accurancy = "1";									// <= 10m

  std::bitset<28> long_complement = std::bitset<28>( std::string("1111111111111111111111111111") );
  string _long = (std::bitset<28>(round(__long*600000)) & long_complement).to_string();

  std::bitset<27> lat_complement = std::bitset<27>( std::string("111111111111111111111111111") );
	string _lat =  (std::bitset<27>(round(__lat*600000)) & lat_complement).to_string();

	string _device = "0001";	// GPS
	string _flags = std::bitset<12>(0).to_string(); 
	// '0': transmission control for packet 24
	// '000000000':  spare
	// '0': Raim flag

	string _rstatus = std::bitset<19>(0).to_string();  // ??
	// '11100000000000000110' : Radio status

	return _type+_repeat+_mmsi+std::bitset<23>(0).to_string()+_hour+_min+_sec+_accurancy+_long+_lat+_device+std::bitset<11>(0).to_string()+_rstatus;
}


//K = input Key, K0=output key, n=number of times to hash
int generateKeyChainCommit(octet *K, octet *K0, int n, int key_size){ //octet *output

  //create a temp octet
  char tempOctet_K_size[2 * 16 + 1];
  octet tempOctet = {0, sizeof(tempOctet_K_size), tempOctet_K_size};
  //copy K into K0
  OCT_copy(K0, K);

  //Use hash function
  for (int i=0; i<n; i++){
    OCT_clear(K0);
    SPhash(MC_SHA2, SHA256, &tempOctet, K0);
    //copy temp octet into K0
    tempOctet.len = key_size;
    OCT_copy(K0, &tempOctet);
  }

  return 0;
}


/**	
 *  @brief send an ais message from socket to GNURadio
 *  @param message to be sent
 *  @return success/fail
 */
int send_message_2_sock(string message){   
    int sock = socket_init(PORT_SEND); 
    message = message + '\0';
    send(sock , message.c_str(), message.length(), 0 ); 
    printf("Message sent\n"); 
    close(sock);
    return 0; 
}

/**
 * 	@brief Send AIS Tag function
 *  @param Ship Ship 1 data
 *  @param Ship_2 Ship 2 data
 *  @param type describe whether Ship 1 is transmitter = 1 or receiver = 2
 *  @param fd1 file descriptor of read socket
 */
int send_ais_message(BloomFilter &bf,string payload, int ais_message_type=4, octet *auth_tag_message=NULL){
    
    printf("\n Sending AIS message: ");

    std::cout<<"\n Message Payload = "<<payload<<endl;

    //check size of payload, fit in 3 slots, max size allowed in 3 slots = 66 bytes according to AIS standard
    int message_count=0;
    int max_payload_size_bytes = MAX_SLOTS_DATA_SIZE;
    int payload_size_bytes = payload.length()/8;
    //DEBUG printf("\nSize of payload = %d\n", payload_size_bytes);

    int counter = ceil(payload_size_bytes / (float) max_payload_size_bytes);
    //DEBUG printf("\nNumber of messages to be send = %d\n", counter);
    if (ais_message_type==4){
      counter = 1;
    }

    int start_index = 0, end_index = 0;
    //std::cout<<"Payload length ="<< payload.length()<<endl<<"Payload=\n"<<payload<<endl;
    
    for (int i=0; i<counter; i++){
        string message;
        if (ais_message_type==8){
          //Send multiple messages, Divide payload into different slot messages if size > 3 continous slots
          end_index = max_payload_size_bytes*(i+1)*8;
          if(end_index > payload.length() ){
              end_index = payload.length();
          }
        //DEBUG   std::cout<<"Payload length ="<< payload.length()<<endl<<"Payload=\n"<<payload<<endl;
        //DEBUG std::cout<<"\nMessage #"<<i<<"Startindex "<< start_index <<" End index"<<end_index <<endl;
          string payload_2_send = payload.substr(start_index, end_index - start_index );

          message = encode_ais_message_8(payload_2_send);
         // std::cout<<"\nMessage: "<<payload_2_send;

                //DEBUG   std::cout<<"\nMessage #"<<i<<"Startindex "<< start_index <<" End index"<<end_index<<endl<<message<<"\npayload:"<<payload_2_send<<endl;
          start_index = end_index; 
        }
        else
        {
           message = encode_ais_message_4();
           if (auth_tag_message!=NULL){
         //    printf("\n message1:  ");//, message.data());
            OCT_jstring(auth_tag_message, (char *)message.data() );
           //  OCT_output(auth_tag_message);
           }
           bf.add((const unsigned char *)message.c_str(), message.length());
        }   

        int res = send_message_2_sock(message);
        if (res != 0)
        {
            printf("Message not sent over socket....Exiting!\n");
            return res;
        }
        
        message_count++;
    }

    return 0;
}


int AIS_Tesla(){
  /*
  We will have 4 security levels:
- Level 1: TESLA-only,  TESLA Digest Size: 512 bits, Overhead: 300%. Usage Scenario: off-shore, no congestion.
- Level 2: TESLA-only,  TESLA Digest Size: 160 bits, Overhead: 200%. Usage Scenario: port, minimum time to authenticate a message.
- Level 3: TESLA + BF in separate messages. TESLA Digest Size: 512 bits, Overhead: ~110%. Usage Scenario: port, medium-level congestion, vehicles moving.
- Level 4: TESLA + BF in separate messages. TESLA Digest Size: 512 bits, Overhead: ~80%. Usage Scenario:  port, high-level congestion, vehicles moving  
- Level 5: TESLA + BF in same message. TESLA Digest Size: 160 bits, Overhead: ~60%. Usage Scenario: port, heavy congestion, vehicles moving.
  */

   /*
 NEW SECURITY LEVELS
 Each security level will have 1 byte meta data, 16 byte key
- Level 1: TESLA-only,  TESLA Digest Size: 512 bits, Overhead: 300%. Usage Scenario: off-shore, no congestion.
- Level 2: TESLA-only,  TESLA Digest Size: 160 bits, Overhead: 200%. Usage Scenario: port, minimum time to authenticate a message.
- Level 3: TESLA + BF in separate messages. TESLA Digest Size: 512 bits, Overhead: ~110%. Usage Scenario: port, medium-level congestion, vehicles moving.
- Level 4: TESLA + BF in separate messages. TESLA Digest Size: 512 bits, Overhead: ~80%. Usage Scenario:  port, high-level congestion, vehicles moving  
- Level 5: TESLA + BF in same message. TESLA Digest Size: 160 bits, Overhead: ~60%. Usage Scenario: port, heavy congestion, vehicles moving.
  */

    int security_level = 5;
    int key_size=16;
    int application_meta_size=1;
    //input_digest_size, can only be 32, 48 or 64
    int input_digest_size, output_digest_size; 
    //number of AIS type 4 messages to send
    int number_of_messages = 1;
    bool sendBF = false;

    //when 512 digest size concatente to 384 or 392
    switch(security_level){
      case 1://Tesla only, 512 digest size
        //generate 512 bits Auth tag using HMAC
        input_digest_size = SHA512;
        output_digest_size = 49;
        number_of_messages = 1;
        break;
      case 2:
        //Tesla only, 160 bits digest size
        input_digest_size = SHA256;
        output_digest_size = 20;
        number_of_messages = 1;
        break;
      case 3:
         //Tesla +BF in same message, 160 digest size
        input_digest_size = SHA256;
        output_digest_size = 20;
        number_of_messages = 4;
        sendBF = true;
        break;
      case 4:
        //Tesla +BF in same message, 256 digest size
        input_digest_size = SHA256;
        output_digest_size = SHA256;
        number_of_messages = 2;
        sendBF = true;
        break;
      case 5:
        //Tesla +BF(2 slots) in sep. message, 512 digest size
        input_digest_size = SHA256;
        output_digest_size = input_digest_size;
        number_of_messages = 9;
        sendBF = true;
        break;
      case 6:
        //Tesla +BF(3 slots) in sep. message, 160 digest size
        input_digest_size = SHA512;
        output_digest_size = 20;
        number_of_messages = 9;
        sendBF = true;
        break;
      case 7:
        //Tesla +BF(3 slots) in sep. message, 512 digest size
        input_digest_size = SHA512;
        output_digest_size = 49;
        number_of_messages = 9;
        sendBF = true;
        break;

      default:
        break;
    }

    //SETTING UP B.F.
    int z = MAX_SLOTS_DATA_SIZE - (output_digest_size+key_size+application_meta_size);
    if(security_level==5 || security_level==6|| security_level==7){
      z = MAX_SLOTS_DATA_SIZE - application_meta_size;
    }
    int k = log(2) * (z / number_of_messages);
    BloomFilter bloomf(z*8, k);

    //TTP, Generates Random Km and very high number n, Sends it to a ship
    int res;
    char *pp = (char *)"M0ng00se";
    // These octets are automatically protected against buffer overflow attacks
    // Note salt must be big enough to include an appended word
    // Note ECIES ciphertext C must be big enough to include at least 1 appended block
    // Recall field_size_EFS is field size in bytes. So EFS_ED25519=32 for 256-bit curve
    char s0[2 * field_size_EGS], s1[2 * field_size_EGS], w0[2 * field_size_EFS + 1], w1[2 * field_size_EFS + 1], z0[output_digest_size], z1[field_size_EFS], key[AESKEY], salt[40], pw[40];
    octet Km = {0, sizeof(s0), s0};
    octet K0 = {0, sizeof(s1), s1};
    octet Ki = {0, sizeof(w0), w0};
    octet Ki1 = {0, sizeof(w1), w1};

    octet outputMAC = {0, static_cast<int> (sizeof(z0)), z0};
    octet Z1 = {0, sizeof(z1), z1};
    octet KEY = {0, sizeof(key), key};
    octet SALT = {0, sizeof(salt), salt};
    octet PW = {0, sizeof(pw), pw};

    //Complete Auth tag will be stored
    char auth_tag_message_size[number_of_messages * field_size_EFS + 1];
    octet auth_tag_message = {0, static_cast<int> (sizeof(auth_tag_message_size)), auth_tag_message_size};

        //Creating B.F
    if(security_level>2){


      // what to do here//increment i/n everytime ais message is sent
      /*
      for(int i=0; i<9; i++){
        application_meta_bits = std::bitset<3>(i+11).to_string(); 
        application_msgs = std::bitset<5>(i+1).to_string(); 
        payload = hextobin(string(char_payload));
        payload = application_meta_bits + application_msgs + payload;
        bloomf.add((const unsigned char *)payload.c_str(), payload.length());
      }
      */
    // cout<<"Contains"<<bloomf.possiblyContains((const unsigned char *)payload.c_str(), payload.length());
    //  cout<<"Compressed size"<< bloomf.Encode_BloomFilter(10);
    //  bloomf.Decode_BloomFilter();
      //cout<<"\nContains"<<bloomf.possiblyContains((const unsigned char *)payload.c_str(), payload.length());
    }

    for(int j=0; j<number_of_messages; j++){
      send_ais_message(bloomf, "",4, &auth_tag_message);
      printf("\n message: %d", j);
      //OCT_output(&auth_tag_message);
      //bloomf.add((const unsigned char *)payload.c_str(), payload.length());
    }

    //Starting TESLA protocol
    //Generation of random number for nonce
    std::random_device seed_gen{};
    std::mt19937_64 mt_rand(seed_gen());
    std::uniform_int_distribution<unsigned long long> dis;
    
    int n =  10 + (rand() % 4500);//mt_rand();
    cout<<"\n Value of n = "<<n<<endl;
    //n=10;
   
   
    //Generating Master key
    SALT.len = 8;
    for (int i = 0; i < 8; i++) SALT.val[i] = i + 1; // set Salt

    printf("Alice's Passphrase= %s\n", pp);

    OCT_empty(&PW);
    OCT_jstring(&PW, pp);  // set Password from string
    
    //Generate Master key Km, of size EGS_ED25519 bytes derived from Password and Salt
    PBKDF2(MC_SHA2, HASH_TYPE_ED25519, &Km, field_size_EGS, &PW, &SALT, 1000);

    printf("\n Km:\n");
    OCT_output(&Km);
    
    //generate K0
    generateKeyChainCommit(&Km, &K0, n, key_size);

    

    //Using pre-generated values
  //  OCT_fromHex(&Km, (char *) "f468065c522a3edcb7d17a063c8baa497d5222ef20aac565d25fa9e79ee6f0f6" );
  //  OCT_fromHex(&K0, (char *) "e3b0c44298fc1c149afbf4c8996fb924" );
    printf("\n K0:\n");
    OCT_output(&K0);

    //Messages sent
    //OnlinePhase
    //i = timeslot
    //Value of i should be less than or equal to 'n'
    int i = 1;//(std::uniform_int_distribution<int>(1,n), mt_rand()); 
    //i =  1 + (rand() % n);
    cout<<"\n Value of i = "<<i<<endl;
    //generate Ki
    generateKeyChainCommit(&Km, &Ki, n-i, key_size);

    printf("\n Ki:\n");
    OCT_output(&Ki);


   // std::string message = std::bitset<256>(n).to_string();
   // OCT_jstring(&auth_tag_message, (char *)message.data() );       

    HMAC(MC_SHA2, input_digest_size, &outputMAC, output_digest_size, &Ki, &auth_tag_message);
    printf("\n HMAC length:\n %d", outputMAC.len);
    
    //Change Octet to char for transmission
    char char_payload[outputMAC.len*2 + 1];
    OCT_toHex(&outputMAC, char_payload);

    char char_key[Ki.len*2 + 1];
    OCT_toHex(&Ki, char_key);
    
    printf("\n outputMAC:\n ");
    OCT_output(&outputMAC);

    
    if(security_level == 1 || security_level == 2 ){

      string security_level_bits = std::bitset<3>(security_level).to_string(); 
      string app_meta_bits = std::bitset<5>(0).to_string(); 
      string MAC = hextobin(string(char_payload));
      string Ki_key = hextobin(string(char_key));
      string payload = security_level_bits + app_meta_bits + Ki_key + MAC;
      
      res = send_ais_message(bloomf, payload, 8, NULL);
    }
    else if(security_level == 3 || security_level == 4 ){
      //same message
      string security_level_bits = std::bitset<3>(security_level).to_string(); 
      string app_meta_bits = std::bitset<5>(0).to_string(); 
      string MAC = hextobin(string(char_payload));
      string Ki_key = hextobin(string(char_key));
      string bf = bloomf.get_string();
      std::cout<<"\n bf: \n"<<bf;

      string payload = security_level_bits + app_meta_bits + Ki_key + MAC + bf;
      send_ais_message(bloomf, payload, 8, NULL);

    }
    else if(security_level == 5 || security_level == 6  || security_level == 7 ){
      //separate message for TESLA and B.F
      string security_level_bits = std::bitset<3>(security_level).to_string(); 
      string app_meta_bits = std::bitset<5>(0).to_string(); 
      string MAC = hextobin(string(char_payload));
      string Ki_key = hextobin(string(char_key));
      
      string payload = security_level_bits + app_meta_bits + Ki_key + MAC;
      //std:cout<<"\n length: "<<payload.length();
      send_ais_message(bloomf, payload, 8, NULL);

      string bf = bloomf.get_string();
      std:cout<<"\n length: "<<bf;
      app_meta_bits = std::bitset<5>(1).to_string(); 
      payload = security_level_bits + app_meta_bits + bf;
      
      send_ais_message(bloomf, payload, 8, NULL);

    }
    else
    {
      /* code */
    }

    //Key verification
    generateKeyChainCommit(&Ki, &Ki1, i, key_size);

    printf("\n Ki1( Hi(Ki) ):\n");
    OCT_output(&Ki1);

  return 0;
}

int main()
{
    int i,fd1;
    unsigned long ran;
   
    time((time_t *)&ran);
    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNG;                // Crypto Strong RNG

    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (i = 4; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("\nStarting SecureAIS protocol \n");

    /*
    do {
        sleep(1);
        fd1 = socket_init(PORT_RECEIVE);
    }while(fd1 == -1);
    printf("Connected to receive socket!\n");
    */

        auto start = std::chrono::high_resolution_clock::now();
        AIS_Tesla();
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("\nTime taken for exchange: %lld microseconds\n\n",  microseconds);


    KILL_CSPRNG(&RNG);
}


