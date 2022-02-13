#include "gen-cpp/CustomSvc.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace std;

#include <iostream>

class CustomSvcHandler : virtual public CustomSvcIf {
 public:
  CustomSvcHandler() {}

  virtual void getHelloMessage(std::string& _return, const std::string& name) override {
        cout << "Server received: " << name << ", from client" << endl;
        _return = "Hello " + name;
    }

    virtual void AcceptInt(std::string& _return, const int32_t num){
        _return = "Received Integer";
    }

    virtual void AcceptDouble(std::string& _return, const double num){
        _return = "Received double";
    }

    virtual void AcceptString(std::string& _return, const string& string_message){
        _return = "Received string";
    }

    virtual void AcceptComplexDataStructure(std::string& _return, const ComplexDataStructure& complex_data){
        _return = "Received Complex Data structure";
    }

    virtual void Ack(std::string& _return, const std::vector<int32_t> & nums) {
        _return = "Received streaming data in Ack";
    } 

    virtual void AcceptClientSideStream(std::string& _return, const string& string_message){
        _return = "Received string";
    }

     
};

int main(int argc, char **argv) {
    auto handler = make_shared<CustomSvcHandler>();
    auto proc = make_shared<CustomSvcProcessor>(handler);
    auto trans_svr = make_shared<TServerSocket>(5050);
    auto trans_fac = make_shared<TBufferedTransportFactory>();
    auto proto_fac = make_shared<TBinaryProtocolFactory>();
    TSimpleServer server(proc, trans_svr, trans_fac, proto_fac);
    server.serve();
    return 0;
}

