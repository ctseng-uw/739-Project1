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
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::HelloRequestInt;
using helloworld::HelloRequestDouble;

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
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  

  std::string user("world");
  std::vector<double> resultsStr, resultsInt, resultsDouble;
  float avgStr, avgInt, avgDouble;

  for(int i=0;i<1000;i++){
    std::string reply = greeter.SayHello(user, resultsStr);
    avgStr = accumulate(resultsStr.begin(), resultsStr.end(), 0.0) / resultsStr.size();

    std::string replyInt = greeter.SayHelloInt(i, resultsInt);
    avgInt = accumulate(resultsInt.begin(), resultsInt.end(), 0.0) / resultsInt.size();

    std::string replyDouble = greeter.SayHelloDouble(i, resultsDouble);
    avgDouble = accumulate(resultsDouble.begin(), resultsDouble.end(), 0.0) / resultsDouble.size();
  }

  std::cout<<avgStr<<" for str\n";
  std::cout<<avgInt<<" for int\n";
  std::cout<<avgDouble<<" for double\n";
  return 0;
}
