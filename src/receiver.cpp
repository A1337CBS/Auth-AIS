//
//g++ -O2 receiver.cpp ais_receiver/*.c core-master/cpp/core.a BloomFilter.cpp smhasher-master/src/MurmurHash3.cpp FastAC_fix-nh/FastAC/arithmetic_codec.cpp -o recvr
//g++ -O2 receiver.cpp ais_receiver/*.c core-master/cpp/core.a BloomFilter.cpp smhasher-master/src/MurmurHash3.cpp -o recvr
#include "stdafx.h"


using namespace B256_56;
using namespace ED25519;
#define field_size_EFS EFS_ED25519
#define field_size_EGS EGS_ED25519
#define MAX_SLOTS_DATA_SIZE 66 


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


/**	
 *  @brief Convert binary string to hex string
 *  @param string &s 
 *  @return hex string
 */
std::string bintohex(const std::string &s){
    
    std::string out;
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

int main(void)
{
    AISConfiguration ais_config;
    std::vector<std::string> ais_messages_4;
    //Store K0 of transmitter
    //OCT_fromHex(&K0, (char *) "e3b0c44298fc1c149afbf4c8996fb924" ); 

    bool repeated_message = false;
    
    std::vector<ais_message_t> ais_vector;
    int fd1, message_count, last_count;
    int i=0;//ith timeslot for TESLA
    message_count = 0;
    do {
        sleep(1);
        fd1 = socket_init(51999);
    }while(fd1 == -1);
    printf("CAESAR Receiver Connected!\n");
    load_configuration(NULL, &ais_config);
   // printf("Total messages to be computed: %d\n");
   // for(message_count = 0; message_count < ais_config.total_messages; message_count++) {

    while(true){

        double vm, rss;
        process_mem_usage(vm, rss);
        std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;

        ais_message_t ais[1];
        ais[message_count].fd = fd1;
        ais[message_count].d.seqnr = 0;
        read_ais_message(&ais[message_count]);
        printf("!%s\r\n", ais[message_count].d.nmea);
        printf("Type: %d\r\n", ais[message_count].d.type);
        printf("security_level: %d\r\n", ais[message_count].d.security_level);
        bool nextBloomf=false;
       
        if(ais[message_count].d.type == 8 ){
           
            std::string temp = bintohex(std::string(ais[message_count].d.payload_buffer));
            std::cout<<"\n Temp: \n"<<temp;

           // std::cout<<"\n Temp: \n"<<ais[message_count].d.message;

           //CAESAR
            int security_level=ais[message_count].d.security_level;
            int application_meta_size=1;
            int key_size=16;
            //input_digest_size, can only be 32, 48 or 64
            int input_digest_size, output_digest_size; 
            //number of AIS type 4 messages to send
            int number_of_messages = 1;
            bool sendBF = false;

            //when 512 digest size concatente to 384 or 392(separate)
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
                    output_digest_size = 21;
                    number_of_messages = 1;
                    break;
                case 3:
                    //Tesla +BF in same message, 256 digest size
                    input_digest_size = SHA256;
                    output_digest_size = 32;
                    number_of_messages = 2;
                    sendBF = true;
                    break;
                case 4:
                    //Tesla +BF in same message, 256 digest size
                    input_digest_size = SHA256;
                    output_digest_size = 20;
                    number_of_messages = 4;
                    sendBF = true;
                    break;
                /*
                case 5:
                    //Tesla +BF(2 slots) in sep. message, 512 digest size
                    input_digest_size = SHA256;
                    output_digest_size = input_digest_size;
                    number_of_messages = 9;
                    sendBF = true;
                    break;*/
                case 5:
                    //Tesla +BF(2 slots) in sep. message, 160 digest size
                    input_digest_size = SHA512;
                    output_digest_size = 20;
                    number_of_messages = 9;
                    sendBF = true;
                    break;
                case 6:
                    //Tesla +BF(3 slots) in sep. message, 512 digest size
                    input_digest_size = SHA512;
                    output_digest_size = 49;
                    number_of_messages = 9;
                    sendBF = true;
                    break;

                default:
                    break;
            }

            //Complete Auth tag will be stored
            char auth_tag_message_size[number_of_messages * field_size_EFS + 1];
            octet auth_tag_message = {0, static_cast<int> (sizeof(auth_tag_message_size)), auth_tag_message_size};


        
            char s0[2 * field_size_EGS], s1[2 * field_size_EGS], z0[output_digest_size*2], z1[output_digest_size*2];
            octet K0_recvd = {0, sizeof(s0), s0};
            octet Ki = {0, sizeof(s1), s1};

            octet outputMAC = {0, static_cast<int> (sizeof(z0)), z0};
            octet outputMAC_recvd = {0, static_cast<int> (sizeof(z1)), z1};


            for(int j = ais_vector.size()-1; j >= 0; j--) {
                   
                    // make sure everything makes it to the output
                     fflush(stdout);   
                    if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==1 && ais_vector[j].d.security_level >= 5 ){
                        break;
                    }
                    else if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==0 && ais_vector[j].d.security_level >= 5 ){
                        continue;
                    }
                    else if ( security_level <5 && ais_vector[j].d.type == 8  ){
                        break;
                    }
                     std::cout<<"\n message: "<<j<< " \n"<<ais_vector[j].d.type;
                    std::string message = ais_vector[j].d.message;

                //  std::cout<<"\n message: "<<j<< " \n"<<message.data();
                //  printf("\n OCT output");
                    OCT_jstring(&auth_tag_message, (char *) message.data() );
                //  OCT_output(&auth_tag_message);
                    i++;
            }
            
        /*
        if(auth_tag_message.len <1){
            printf("\n OCT output");
            OCT_output(&auth_tag_message);
            //   continue;
        }*/

        if(security_level == 1 || security_level == 2 ){
            OCT_fromHex(&Ki, (char *) temp.substr(0, key_size*2).data()); 
            printf("\n Ki:\n ");
            OCT_output(&Ki);

            //Key verification
            generateKeyChainCommit(&Ki, &K0_recvd, i, key_size);
            //Truncate keysize to 16 bytes
        //  K0_recvd.len=key_size;
            printf("\n Ki(Hi(Ki)) == K0? :\n");
            OCT_output(&K0_recvd);


            OCT_fromHex(&outputMAC_recvd, (char *) temp.substr(key_size*2, output_digest_size*2).data()); 
            printf("\n outputMAC_recvd:\n ");
            OCT_output(&outputMAC_recvd);

            HMAC(MC_SHA2, input_digest_size, &outputMAC, output_digest_size, &Ki, &auth_tag_message);
            printf("\n outputMAC:\n ");
            OCT_output(&outputMAC);

            
            if (!OCT_comp(&outputMAC, &outputMAC_recvd))
            {
                printf("*** Auth tag exchanged Failed\n");
                return -1;
            }
            } else if(security_level == 3 || security_level == 4 ){
                    OCT_fromHex(&Ki, (char *) temp.substr(0, key_size*2).data()); 
                    printf("\n Ki:\n ");
                    OCT_output(&Ki);

                    //Key verification
                    generateKeyChainCommit(&Ki, &K0_recvd, i, key_size);
                    //Truncate keysize to 16 bytes
                //  K0_recvd.len=key_size;
                    printf("\n Ki(Hi(Ki)) == K0? :\n");
                    OCT_output(&K0_recvd);

                    OCT_fromHex(&outputMAC_recvd, (char *) temp.substr(key_size*2, output_digest_size*2).data()); 
                    printf("\n outputMAC_recvd:\n ");
                    OCT_output(&outputMAC_recvd);

                    HMAC(MC_SHA2, input_digest_size, &outputMAC, output_digest_size, &Ki, &auth_tag_message);
                    printf("\n outputMAC:\n ");
                    OCT_output(&outputMAC);

                    //SETTING UP B.F.
                    int z = MAX_SLOTS_DATA_SIZE - (output_digest_size+key_size+application_meta_size);
                    int k = log(2) * (z / number_of_messages);
                    BloomFilter bloomf(z*8, k);
                    
                    std::string bf = ais[message_count].d.message.substr( ais[message_count].d.message.length() - (z*8), z*8);
                 //   std::cout<<"\n bf: \n"<<bf;
                    bloomf.to_bits(bf);

                    for(int j = ais_vector.size(); j > 0; j--) {
                        if ( ais_vector[j-1].d.type == 8)
                            break;
                        std::string message = ais_vector[j-1].d.message;
                        std::cout<<"\nContains "<< (bloomf.possiblyContains((const unsigned char *)message.c_str(), message.length()));
                    }

                    if (!OCT_comp(&outputMAC, &outputMAC_recvd))
                    {
                        printf("*** Auth tag exchanged Failed\n");
                        return -1;
                    }

            }
            else if((security_level == 5 || security_level == 6  || security_level == 7) && ais[message_count].d.appmeta_bits==0){
                //if(nextBloomf==false){
                    nextBloomf = true;
                 //   continue;
               // }
              //  std::cout<<"\nContains if ";
             //   continue;
            }
            else if((security_level == 5 || security_level == 6  || security_level == 7) && ais[message_count].d.appmeta_bits==1 ){
                
                int j = ais_vector.size() - 1; //previous message is TESLA
                std::string TESLA = std::string(ais_vector[j].d.payload_buffer);
                TESLA = bintohex(TESLA);

                OCT_fromHex(&Ki, (char *) TESLA.substr(0, key_size*2).data()); 
                printf("\n Ki:\n ");
                OCT_output(&Ki);

                //Key verification
                generateKeyChainCommit(&Ki, &K0_recvd, i, key_size);
                //Truncate keysize to 16 bytes
            //  K0_recvd.len=key_size;
                printf("\n Ki(Hi(Ki)) == K0? :\n");
                OCT_output(&K0_recvd);


                OCT_fromHex(&outputMAC_recvd, (char *) TESLA.substr(key_size*2, output_digest_size*2).data()); 
                printf("\n outputMAC_recvd:\n ");
                OCT_output(&outputMAC_recvd);

                HMAC(MC_SHA2, input_digest_size, &outputMAC, output_digest_size, &Ki, &auth_tag_message);
                printf("\n outputMAC:\n ");
                OCT_output(&outputMAC);

                //SETTING UP B.F.
                int z = MAX_SLOTS_DATA_SIZE - application_meta_size;
                int k = log(2) * (z / number_of_messages);
                BloomFilter bloomf(z*8, k);
                
                std::string bf = std::string(ais[message_count].d.payload_buffer).substr(0,  std::string::npos);
                   
                bloomf.to_bits(bf);

                for(int j = ais_vector.size()-1; j >= 0; j--) {
                    if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==1 && ais_vector[j].d.security_level >= 5 ){
                        break;
                    }
                    else if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==0 && ais_vector[j].d.security_level >= 5 ){
                        continue;
                    }
                    else if ( security_level <5 && ais_vector[j].d.type == 8  ){
                        break;
                    }
                    std::string message = ais_vector[j].d.message;

                    auto start = std::chrono::high_resolution_clock::now();
                    std::cout<<"\nContains "<< (bloomf.possiblyContains((const unsigned char *)message.c_str(), message.length()));
                    auto elapsed = std::chrono::high_resolution_clock::now() - start;
                    long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
                    printf("\nTime taken to check if element in B.F. : %lld nanoseconds\n\n",  nanoseconds);
                    
                }

                if (!OCT_comp(&outputMAC, &outputMAC_recvd))
                {
                    printf("*** Auth tag exchanged Failed\n");
                    return -1;
                }
                
                nextBloomf = false;
            }else{
                //Separate Bloomfilter message
            }
            
        
        }else if(ais[message_count].d.type == 4 ){
           //go back till last message==8 and source MMSI is equal
          //  std::cout<<"\n Temp: \n"<<ais[message_count].d.message;

          //  ais_vector.push_back(ais[message_count]);
        
        }
        //Add element to vector
        ais_vector.push_back(ais[message_count]);


        // make sure everything makes it to the output
        fflush(stdout);         
    }
}
