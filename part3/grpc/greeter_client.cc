/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <fstream>
#include <numeric>
#include <memory>
#include <string>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::HelloRequestInt;
using helloworld::HelloRequestDouble;
using helloworld::StreamRequest;
using helloworld::HelloRequestComplex;

class GreeterClient {
public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user, std::vector<double> &results) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    auto start = std::chrono::high_resolution_clock::now();
    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::ofstream myfile;
    myfile.open("RTT_str_opt.txt", std::ios_base::app); // append instead of overwrite
    myfile << elapsed_seconds.count()<<std::endl; 
    results.push_back(elapsed_seconds.count());

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  std::string SayHelloInt(const int d, std::vector<double> &results) {
    HelloRequestInt request;
    request.set_name(d);
    HelloReply reply;
    ClientContext context;

    auto start = std::chrono::high_resolution_clock::now();
    
    Status status = stub_->SayHelloInt(&context, request, &reply);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::ofstream myfile;
    myfile.open("RTT_int_opt.txt", std::ios_base::app); // append instead of overwrite
    myfile <<elapsed_seconds.count()<<std::endl; 
    results.push_back(elapsed_seconds.count());

    if (status.ok()) {
      return reply.message();
    } else {
        std::cout << "Failed here: "<< status.error_code() << ": " << status.error_message()
          << std::endl;
        return "RPC failed";
      }
  }

  std::string SayHelloDouble(const double d, std::vector<double> &results) {
    HelloRequestDouble request;
    request.set_name(d);
    HelloReply reply;
    ClientContext context;

    auto start = std::chrono::high_resolution_clock::now();
    Status status = stub_->SayHelloDouble(&context, request, &reply);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::ofstream myfile;
    myfile.open("RTT_double_opt.txt", std::ios_base::app); // append instead of overwrite
    myfile <<elapsed_seconds.count()<<std::endl; 
    results.push_back(elapsed_seconds.count());

    if (status.ok()) {
      return reply.message();
    } else {
        std::cout << "Failed here: "<< status.error_code() << ": " << status.error_message()
          << std::endl;
        return "RPC failed";
    }
  }

  void DoClientStream(int buf_size, int transmit_size, std::vector<double> &results) {
    StreamRequest req;
    HelloReply reply;
    ClientContext context;

    std::string buf(buf_size, 'a');
    req.set_buffer(buf);

    std::unique_ptr<ClientWriter<StreamRequest>> writer(stub_->DoClientStream(&context, &reply));

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < transmit_size / buf_size; i++) {
      if (!writer->Write(req)) {
        std::cout << "broken connection!" << std::endl;
        break;
      }
    }
    writer->WritesDone();
    Status status = writer->Finish();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    results.push_back(elapsed_seconds.count());

    if (!status.ok()) {
      std::cout << "Failed here: " << status.error_code() << ":" << status.error_message() << std::endl;
    }
  }

  std::string SayHelloComplex(const double val, const int num, const std::string& user, std::vector<double> &results) {
    HelloRequestComplex request;
    request.set_name(user);
    request.set_num(num);
    request.set_val(val);
    HelloReply reply;
    ClientContext context;


    auto start = std::chrono::high_resolution_clock::now();
    Status status = stub_->SayHelloComplex(&context, request, &reply);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::ofstream myfile;
    myfile.open("RTT_complex_opt.txt", std::ios_base::app); // append instead of overwrite
    myfile << elapsed_seconds.count()<<std::endl; 
    results.push_back(elapsed_seconds.count());

    if (status.ok()) {
      return reply.message();
    } else {
        std::cout << "Failed here: "<< status.error_code() << ": " << status.error_message()
          << std::endl;
        return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str("localhost:50052");
  std::string arg_target_str("--target");
  std::string arg_buf_size_str("--buf_size");
  std::string arg_transmit_str("--transmit");
  int buf_size = 1024;
  int transmit_size = 1024*1024;
  for (int i = 1; i < argc; i++) {
    std::string arg_val = argv[i];
    size_t start_pos = arg_val.find(arg_target_str);

    if (start_pos != std::string::npos) {
      start_pos += arg_target_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    }

    start_pos = arg_val.find(arg_buf_size_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_buf_size_str.size();
      if (arg_val[start_pos] == '=') {
        std::string tmp = arg_val.substr(start_pos + 1);
        buf_size = stoi(tmp);
      } else {
        std::cout << "Must be in the format of --buf_size=" << std::endl;
        return 0;
      }
    }

    start_pos = arg_val.find(arg_transmit_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_transmit_str.size();
      if (arg_val[start_pos] == '=') {
        std::string tmp = arg_val.substr(start_pos + 1);
        transmit_size = stoi(tmp);
      } else {
        std::cout << "Must be in the format of --transmit=" << std::endl;
        return 0;
      }
    }
  }

  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  

  std::string user("world");
  std:: string smallStr, medStr, longStr;
  for (int i = 0; i < 512; i++)
        smallStr = smallStr + 'a';
  for (int i = 0; i < 1024; i++)
        medStr = medStr + 'a';
  for (int i = 0; i < 2048; i++)
        longStr = longStr + 'a';
  

  std::vector<double> resultsSmallStr,resultsMedStr, resultsInt, resultsDouble, resultsComplex, resultsLongStr, resultsStream;
  float avgSmallStr, avgMedStr, avgInt, avgDouble, avgComplex, avgLongStr, avgStreamBW;

  for(int i=0;i<100;i++){
    std::string replySmallStr = greeter.SayHello(smallStr, resultsSmallStr);
    avgSmallStr = accumulate(resultsSmallStr.begin(), resultsSmallStr.end(), 0.0) / resultsSmallStr.size();

    std::string replyMedStr = greeter.SayHello(medStr, resultsMedStr);
    avgMedStr = accumulate(resultsMedStr.begin(), resultsMedStr.end(), 0.0) / resultsMedStr.size();

    std::string replyLongStr = greeter.SayHello(longStr, resultsLongStr);
    avgLongStr = accumulate(resultsLongStr.begin(), resultsLongStr.end(), 0.0) / resultsLongStr.size();

    std::string replyInt = greeter.SayHelloInt(i, resultsInt);
    avgInt = accumulate(resultsInt.begin(), resultsInt.end(), 0.0) / resultsInt.size();

    std::string replyDouble = greeter.SayHelloDouble(i, resultsDouble);
    avgDouble = accumulate(resultsDouble.begin(), resultsDouble.end(), 0.0) / resultsDouble.size();

    std::string replyComplex = greeter.SayHelloComplex(i+1, i, user, resultsComplex);
    avgComplex = accumulate(resultsComplex.begin(), resultsComplex.end(), 0.0) / resultsComplex.size();
  
    greeter.DoClientStream(buf_size, transmit_size, resultsStream);
  }
  
  auto streamTime = accumulate(resultsStream.begin(), resultsStream.end(), 0.0) / resultsStream.size();
  avgStreamBW = transmit_size * 8 / streamTime;
  
  std::cout<<avgSmallStr<<" for str\n";
  std::cout<<avgLongStr<<" for med str\n";
  std::cout<<avgLongStr<<" for long str\n";
  std::cout<<avgInt<<" for int\n";
  std::cout<<avgDouble<<" for double\n";
  std::cout<<avgComplex<<" for complex\n";

  std::cout<<avgStreamBW / 1000 / 1000 <<" streaming Mbits/second\n";

  return 0;
}
