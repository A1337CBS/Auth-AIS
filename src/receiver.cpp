/* 
  receiver.cpp 
  @authors Ahmed Aziz, Pietro Tedeschi, Savio Sciancalepore, Roberto Di Pietro
  @Description: Receiver program for implementing AIS_CAESAR Protocol PoC
  @version 1.0 25/02/19
**/
//g++ -O2 receiver.cpp ais_receiver/*.c core-master/cpp/core.a BloomFilter.cpp smhasher-master/src/MurmurHash3.cpp -o recvr
#include "main.h"

#ifndef PORT_RECEIVE
#define PORT_RECEIVE 51999
#endif


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

int main(void)
{
    AISConfiguration ais_config;

    //Store K0 of transmitter, should be same as in main.cpp
    char s0[2 * field_size_EGS];
    octet K0 = {0, sizeof(s0), s0};
    OCT_fromHex(&K0, (char *) "3befe8479939cbb8772d4fd0985a2502" ); 
    int ith_timeslot=0;//ith timeslot for TESLA 

    bool repeated_message = false;
    
    std::vector<ais_message_t> ais_vector;
    int fd1, message_count, last_count;
    
    message_count = 0;
    do {
        sleep(1);
        fd1 = socket_init(PORT_RECEIVE);
    }while(fd1 == -1);
    printf("CAESAR Receiver Connected!\n");
    printf("Transmitter Key K0:\n");
    OCT_output(&K0);

    load_configuration(NULL, &ais_config);

    while(true){

        double vm, rss;
        process_mem_usage(vm, rss);
        std::cout << "\n VM: " << vm << "; RSS: " << rss << std::endl;
        std::cout << "ith_timeslot: " << ith_timeslot << std::endl;
        ais_message_t ais[1];
        ais[message_count].fd = fd1;
        ais[message_count].d.seqnr = 0;
        bool nextBloomf=false;
        
        read_ais_message(&ais[message_count]);

        if(ais[message_count].d.type == 8){
            printf("security_level: %d\r\n", ais[message_count].d.security_level);
        }
       
        if(ais[message_count].d.type == 8 ){
           
            std::string temp = bintohex(std::string(ais[message_count].d.payload_buffer));
           // std::cout<<"\n Temp: \n"<<temp;

            //CAESAR config
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
                    input_digest_size = SHA512;
                    output_digest_size = 21;
                    number_of_messages = 1;
                    break;
                case 3:
                    //Tesla +BF in same message, 256 digest size
                    input_digest_size = SHA512;
                    output_digest_size = 32;
                    number_of_messages = 2;
                    sendBF = true;
                    break;
                case 4:
                    //Tesla +BF in same message, 256 digest size
                    input_digest_size = SHA512;
                    output_digest_size = 20;
                    number_of_messages = 4;
                    sendBF = true;
                    break;
                /*
                case 5:
                    //Tesla +BF(2 slots) in sep. message, 512 digest size
                    input_digest_size = SHA512;
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
        
            char s1[2 * field_size_EGS], s2[2 * field_size_EGS], z0[output_digest_size*2], z1[output_digest_size*2];
            octet K0_recvd = {0, sizeof(s1), s1};
            octet Ki = {0, sizeof(s2), s2};

            octet outputMAC = {0, static_cast<int> (sizeof(z0)), z0};
            octet outputMAC_recvd = {0, static_cast<int> (sizeof(z1)), z1};


            for(int j = ais_vector.size()-1; j >= 0; j--) {
                    //go back till last message==8 or source MMSI is equal 
                    if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==1 && ais_vector[j].d.security_level >= 5 ){
                        break;
                    }
                    else if ( ais_vector[j].d.type == 8 && ais_vector[j].d.appmeta_bits==0 && ais_vector[j].d.security_level >= 5 ){
                        continue;
                    }
                    else if ( security_level <5 && ais_vector[j].d.type == 8  ){
                        break;
                    }
                    
                    //std::cout<<"\n message: "<<j<< " \n"<<ais_vector[j].d.type;
                    std::string message = ais_vector[j].d.message;

                //  std::cout<<"\n message: "<<j<< " \n"<<message.data();
                //  printf("\n OCT output");
                    OCT_jstring(&auth_tag_message, (char *) message.data() );
                //  OCT_output(&auth_tag_message);
            }
            
        /*
        if(auth_tag_message.len <1){
            printf("\n OCT output");
            OCT_output(&auth_tag_message);
            //   continue;
        }*/

        if(security_level == 1 || security_level == 2 ){
            //Extract key from message
            OCT_fromHex(&Ki, (char *) temp.substr(0, key_size*2).data()); 
            printf("\n Ki:\n ");
            OCT_output(&Ki);

            //Key verification
            generateKeyChainCommit(&Ki, &K0_recvd, ith_timeslot, key_size);

            printf("\n Ki(Hi(Ki)) == K0? :\n");
            OCT_output(&K0_recvd);


            OCT_fromHex(&outputMAC_recvd, (char *) temp.substr(key_size*2, output_digest_size*2).data()); 
            printf("\n outputMAC_recvd:\n ");
            OCT_output(&outputMAC_recvd);

            HMAC(MC_SHA2, input_digest_size, &outputMAC, output_digest_size, &Ki, &auth_tag_message);
            printf("\n outputMAC:\n ");
            OCT_output(&outputMAC);
            
            
            if (!OCT_comp(&K0_recvd, &K0))
            {
                printf("*** Key exchanged Failed\n");
                return -1;
            }else{
                printf("*** Key K0 exchanged matches! \n");
            }
            
            if (!OCT_comp(&outputMAC, &outputMAC_recvd))
            {
                printf("*** MAC tag exchanged Failed\n");
                return -1;
            }else{
                printf("*** MAC tag matches! \n");
            }
            ith_timeslot = 0;

        } else if(security_level == 3 || security_level == 4 ){
                    OCT_fromHex(&Ki, (char *) temp.substr(0, key_size*2).data()); 
                    printf("\n Ki:\n ");
                    OCT_output(&Ki);

                    //Key verification
                    generateKeyChainCommit(&Ki, &K0_recvd, ith_timeslot, key_size);

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

                    for(int j = ais_vector.size(), k = ith_timeslot; j > 0,k>0; j--, k--) {
                        if ( ais_vector[j-1].d.type == 8)
                            break;
                        std::string message = ais_vector[j-1].d.message;
                        string contains = (bloomf.possiblyContains((const unsigned char *)message.c_str(), message.length())?"true":"false");
                        std::cout<<"\n Contains ais message 4 received#"<< k <<"\t"<<contains;
                    }

                    if (!OCT_comp(&K0_recvd, &K0))
                    {
                        printf("\n*** Key exchanged Failed\n");
                        return -1;
                    }else{
                        printf("\n*** Key K0 exchanged matches! \n");
                    }
                    
                    if (!OCT_comp(&outputMAC, &outputMAC_recvd))
                    {
                        printf("*** MAC tag exchanged Failed\n");
                        return -1;
                    }else{
                        printf("*** MAC tag matches! \n");
                    }
                    ith_timeslot = 0;

            }
            else if((security_level == 5 || security_level == 6  || security_level == 7) && ais[message_count].d.appmeta_bits==0){

                    nextBloomf = true;

            }
            else if((security_level == 5 || security_level == 6  || security_level == 7) && ais[message_count].d.appmeta_bits==1 ){
                
                int j = ais_vector.size() - 1; //previous message is TESLA
                std::string TESLA = std::string(ais_vector[j].d.payload_buffer);
                TESLA = bintohex(TESLA);

                OCT_fromHex(&Ki, (char *) TESLA.substr(0, key_size*2).data()); 
                printf("\n Ki:\n ");
                OCT_output(&Ki);

                //Key verification
                generateKeyChainCommit(&Ki, &K0_recvd, ith_timeslot, key_size);

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

                for(int j = ais_vector.size()-1, k = ith_timeslot; j >= 0,k>0; j--, k--) {
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

                    //auto start = std::chrono::high_resolution_clock::now();
                    string contains = (bloomf.possiblyContains((const unsigned char *)message.c_str(), message.length())?"true":"false");
                    std::cout<<"\n Contains ais message 4 received#"<< k <<"\t"<<contains;
                    //auto elapsed = std::chrono::high_resolution_clock::now() - start;
                    //long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
                    //printf("\nTime taken to check if element in B.F. : %lld nanoseconds\n\n",  nanoseconds);
                    
                }

                if (!OCT_comp(&K0_recvd, &K0))
                {
                    printf("\n*** Key exchanged Failed\n");
                    return -1;
                }else{
                    printf("\n*** Key K0 exchanged matches! \n");
                }
                
                if (!OCT_comp(&outputMAC, &outputMAC_recvd))
                {
                    printf("*** MAC tag exchanged Failed\n");
                    return -1;
                }else{
                    printf("*** MAC tag matches! \n");
                }
                ith_timeslot = 0;
                
                nextBloomf = false;
            }else{
                //Separate Bloomfilter message
            }
            
        
        }else if(ais[message_count].d.type == 4 ){
    
            //increment ith_timeslot everytime ais message is received/simulating one ais slot has passed
            ith_timeslot++;
        
        }
        //Add element to vector
        ais_vector.push_back(ais[message_count]);


        // make sure everything makes it to the output
        fflush(stdout);         
    }
}
