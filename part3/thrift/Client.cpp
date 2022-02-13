#include "gen-cpp/CustomSvc.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <memory>
#include <iostream>
#include <string>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <chrono>
#include <bits/stdc++.h>
#include <math.h>

#define BUFFER_SIZE 64000

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

using namespace std;

#define ITERATIONS 100

using std::shared_ptr;
using std::make_shared;

//using namespace std::chrono:chrono_literals;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

string CreateString(std::int32_t size){
    std::string ret = "";
    for (int i = 0; i < size; i++)
    {
      ret += "a";
    }
    return ret;
}

struct Trade {
    int size;
    double price;
    std::string symbol;
};

void showOptionUsage()
{
    cout << "Usage: -t <num> to run test cases" << endl;
    cout << "Where num is" << endl;
    cout << "1 = Marshalling and unmarshalling of messages and the time taken for them" << endl;
    cout << "2 = Round trip time (RTT) for various data types" << endl;
    cout << "3 = Bandwidth calculation for varying message sizes" << endl;
}

void roundTripSameMachine(CustomSvcClient client){

    int print_count = 0;

    std::cout<< " *** TESTING INT *** " << std::endl;
    auto rtt_time = 0.0;
    for (int i = 0; i < ITERATIONS; i++)
    {
        int test_int = 1;
        std::string ret_string;
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptInt(ret_string, test_int);
        auto time_after_message_sent = high_resolution_clock::now();
        rtt_time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "RTT INT for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    auto avg_rtt = rtt_time/ITERATIONS;
    cout << "avg rtt time : " << avg_rtt << " ns" << std::endl;
    rtt_time = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING DOUBLE *** " << std::endl;
    rtt_time = 0.0;
    avg_rtt = 0.0;
    for (int i = 0; i < ITERATIONS; i++)
    {
        double test_double = 1;
        std::string ret_string;
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptDouble(ret_string, test_double);
        auto time_after_message_sent = high_resolution_clock::now();
        rtt_time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "RTT DOUBLE for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avg_rtt = rtt_time/ITERATIONS;
    cout << "avg rtt time : " << avg_rtt << " ns" << std::endl;
    rtt_time = 0.0;
    avg_rtt = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING STRING OF LEN = 512 *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        string reply_str;
        std::string stringReq = CreateString(512);
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptString(reply_str, stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        rtt_time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "RTT STRING OF LEN = 512 for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avg_rtt = rtt_time/ITERATIONS;
    cout << "avg rtt time : " << avg_rtt << " ns" << std::endl;
    rtt_time = 0.0;
    avg_rtt = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING STRING OF LEN = 1024 *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        string reply_str;
        std::string stringReq = CreateString(1024);
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptString(reply_str, stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        rtt_time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "RTT STRING OF LEN = 1024 for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avg_rtt = rtt_time/ITERATIONS;
    cout << "avg rtt time : " << avg_rtt << " ns" << std::endl;
    rtt_time = 0.0;
    avg_rtt = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING STRING OF LEN = 2048 *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        string reply_str;
        std::string stringReq = CreateString(2048);
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptString(reply_str, stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        rtt_time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "RTT STRING OF LEN = 2048 for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avg_rtt = rtt_time/ITERATIONS;
    cout << "avg rtt time : " << avg_rtt << " ns" << std::endl;
}

void clientStreaming(CustomSvcClient client)
{
    int sizeArr[] = {128, 256, 512, 1024, 2048, 4096, 8192};
    int len = sizeof(sizeArr)/sizeof(sizeArr[0]);
    cout << "len : " << len << std::endl;
    // double average_time ;
    double time_sum = 0;
    string reply_str = "";
    for (int i = 0; i < len; i++)
    {
        std::string toSend = CreateString(sizeArr[i]);
        std::cout<< " *** TESTING STRING OF LEN = " << sizeArr[i]  << " *** " << std::endl;
        
        auto time_before_message_sent = high_resolution_clock::now();
        client.AcceptClientSideStream(reply_str, toSend);
        auto time_after_message_sent = high_resolution_clock::now();
        time_sum = std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
    
        // average_time = time_sum/ITERATIONS;
        auto bandwidth = toSend.length()/time_sum;
        bandwidth = bandwidth * pow(10, 3) * 8;
        cout<< "Bandwidth for size " << toSend.length() << ": " << bandwidth << " Mbps" << endl;
        // average_time = 0;
        time_sum = 0;
    }
}

void marshallMsgs(){

    int32_t size_written;
    auto trans_m = make_shared<TMemoryBuffer>(40960);
    TBinaryProtocol binary_proto(trans_m);

    //pack int and measure time

    std::cout<< " *** TESTING INT *** " << std::endl;
    auto total_time_marshall = 0.0;
    auto total_time_unmarshall = 0.0;
    int print_count = 0;

    for (int i = 0; i < ITERATIONS; i++)
    {
        std::int32_t intReq = 1456;
        auto time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.writeI32(intReq);
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to marshall INT for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        int recInt;
        time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.readI32(recInt);
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to unmarshall INT for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    auto avgMarshall = total_time_marshall/ITERATIONS;
    auto avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;

    //pack double and measure time

    total_time_marshall = 0.0;
    total_time_unmarshall = 0.0;
    avgMarshall = 0.0;
    avgUnmarshall = 0.0;
    print_count = 0;
    std::cout<< " *** TESTING DOUBLE *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        std::double_t doubleReq = 1.0;
        auto time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.writeDouble(doubleReq);
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to marshall DOUBLE for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        std::double_t recDouble = 0.0;
        time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.readDouble(recDouble);
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to unmarshall DOUBLE for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }

    avgMarshall = total_time_marshall/ITERATIONS;
    avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;

    //pack string (len=512) and measure time

    total_time_marshall = 0.0;
    total_time_unmarshall = 0.0;
    avgMarshall = 0.0;
    avgUnmarshall = 0.0;
    print_count = 0;
    std::cout<< " *** TESTING STRING (512 len) *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        std::string stringReq = CreateString(512);
        auto time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.writeString(stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to marshall STRING (512 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        std::string recString;
        time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.readString(recString);
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to unmarshall STRING (512 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avgMarshall = total_time_marshall/ITERATIONS;
    avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;


    //pack string (len=1024) and measure time

    total_time_marshall = 0.0;
    total_time_unmarshall = 0.0;
    avgMarshall = 0.0;
    avgUnmarshall = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING STRING (1024 len) *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        std::string stringReq = CreateString(1024);
        auto time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.writeString(stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to marshall STRING (1024 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        std::string recString;
        time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.readString(recString);
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        if(print_count < 5)
            cout << "Time to unmarshall STRING (1024 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avgMarshall = total_time_marshall/ITERATIONS;
    avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;

    //pack string (len=2048) and measure time

    total_time_marshall = 0.0;
    total_time_unmarshall = 0.0;
    avgMarshall = 0.0;
    avgUnmarshall = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING STRING (2048 len) *** " << std::endl;
    for (int i = 0; i < ITERATIONS; i++)
    {
        std::string stringReq = CreateString(2048);
        auto time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.writeString(stringReq);
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
         if(print_count < 5)
            cout << "Time to marshall STRING (2048 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        std::string recString;
        time_before_message_sent = high_resolution_clock::now();
        size_written = binary_proto.readString(recString);
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
         if(print_count < 5)
            cout << "Time to unmarshall STRING (2048 len) for iteration "<< print_count <<" : " << std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count() << " ns" << std::endl;
        print_count++;
    }
    avgMarshall = total_time_marshall/ITERATIONS;
    avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;

    //pack complex DS and measure time

    total_time_marshall = 0.0;
    total_time_unmarshall = 0.0;
    avgMarshall = 0.0;
    avgUnmarshall = 0.0;
    print_count = 0;

    std::cout<< " *** TESTING COMPLEX DS *** " << std::endl;
    for (int j = 0; j < ITERATIONS; j++)
    {

        std::string stringReq = CreateString(2048);
        Trade trade{10, 12.34, "Hello"};
        int i = 0;
        auto time_before_message_sent = high_resolution_clock::now();
        i += binary_proto.writeStructBegin("Trade");
        i += binary_proto.writeFieldBegin("size", T_I32, 1);
        i += binary_proto.writeI32(trade.size);
        i += binary_proto.writeFieldEnd();
        i += binary_proto.writeFieldBegin("price", T_DOUBLE, 2);
        i += binary_proto.writeDouble(trade.price);
        i += binary_proto.writeFieldEnd();
        i += binary_proto.writeFieldBegin("symbol", T_STRING, 3);
        i += binary_proto.writeString(std::string(trade.symbol));
        i += binary_proto.writeFieldEnd();
        i += binary_proto.writeFieldStop();
        i += binary_proto.writeStructEnd();
        auto time_after_message_sent = high_resolution_clock::now();
        total_time_marshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();
        
        
        string struct_name; 
        string field_name;
        TType field_type;
        int16_t field_id;
        time_before_message_sent = high_resolution_clock::now();

        binary_proto.readStructBegin(struct_name);

        binary_proto.readFieldBegin(field_name, field_type, field_id);
        binary_proto.readI32(trade.size);
        binary_proto.readFieldEnd();

        binary_proto.readFieldBegin(field_name, field_type, field_id);
        binary_proto.readDouble(trade.price);
        binary_proto.readFieldEnd();

        binary_proto.readFieldBegin(field_name, field_type, field_id);
        binary_proto.readString(trade.symbol);
        binary_proto.readFieldEnd();
        
        time_after_message_sent = high_resolution_clock::now();
        total_time_unmarshall += std::chrono::duration_cast<std::chrono::nanoseconds>(time_after_message_sent - time_before_message_sent).count();

        print_count++;
    }    
    avgMarshall = total_time_marshall/ITERATIONS;
    avgUnmarshall = total_time_unmarshall/ITERATIONS;
    cout<< "avgMarshall " << avgMarshall << "ns" << endl;
    cout<< "avgUnmarshall " << avgUnmarshall << "ns" << endl;
}

int main(int argc, char *argv[]) {

    shared_ptr<TTransport> trans;
    trans = make_shared<TSocket>("34.134.35.35", 5050); //change hostname here
    trans = make_shared<TBufferedTransport>(trans);
    auto proto = make_shared<TBinaryProtocol>(trans);
    CustomSvcClient client(proto);

    int32_t tFlagValue = 0;
    int32_t hflag = 0;
    int32_t c;
    while ((c = getopt(argc, argv, "ha:")) != -1)
    {
        switch (c)
        {
        case 'h':
            hflag = 1;
            break;
        case 'a':
            tFlagValue = std::stoi(optarg);
            break;
        case '?':
            if (optopt == 'c')
            {
                cout << "Option -" << optopt << "requires an argument."<< endl;
            }
            else if (isprint (optopt))
            {
                cout << "Unknown option " << optopt << endl;
            }
            else
            {  
                cout << "Unknown option character" << optopt << endl;
            }
        default:
            abort();
        }
    }

    if (hflag)
    {
        showOptionUsage();
    }

    if (tFlagValue == 0)
    {
        return 0;
    }

    int argIntVal = tFlagValue;

    try {
        trans->open();
    
        switch(argIntVal)
        {
            case 1:
            {
                marshallMsgs();
                break;
            }

            case 2:
            {
                roundTripSameMachine(client);
                break;
            }

            case 3:
            {
                clientStreaming(client);
                break;
            }
        }
    } 
    catch (const std::exception& e)
    {
        std::cout << "Exception found: " << e.what();
    }
    trans->close();
}

