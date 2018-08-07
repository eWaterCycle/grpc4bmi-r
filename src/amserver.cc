#define STRICT_R_HEADERS
#include <Rcpp.h>
using namespace Rcpp;

#include <grpcpp/grpcpp.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <grpc++/support/status.h>

#include <iostream>
#include <queue>
#include <sstream>

#include "bmi.grpc.pb.h"

using grpc::ServerCompletionQueue;
using grpc::Status;
using bmi::Empty;
using bmi::InitializeRequest;
using bmi::GetComponentNameResponse;
using bmi::BmiService;

class CallData {
public:
  CallData(BmiService::AsyncService* service, ServerCompletionQueue* cq, Function callback)
   : cq_(cq), service_(service), callback_(callback) {

   }
  virtual ~CallData() {}
  virtual void process(bool ok) = 0;

protected:
  bool done_ = false;
  grpc::ServerContext context_;
  grpc::ServerCompletionQueue* cq_;
  BmiService::AsyncService* service_;
  Function callback_;
private:
  CallData(const CallData&) = delete;
  CallData& operator=(const CallData&) = delete;
};

class GetComponentNameCallData : public CallData {
  public:
    GetComponentNameCallData(BmiService::AsyncService* service, ServerCompletionQueue* cq, Function callback)
     : CallData(service, cq, callback), writer_(&context_)
    {
      service->RequestgetComponentName(&context_, &request_, &writer_, cq_, cq_, this);
    }
    void process(bool ok) final {
      if (done_) {
        new GetComponentNameCallData(service_, cq_, callback_);
        delete this;
      } else {
        std::string name = as<std::string>(callback_());
        response_.set_name(name);
        writer_.Finish(response_, Status::OK, this);
        done_ = true;
      }
    }
  private:
    Empty request_;
    grpc::ServerAsyncResponseWriter<GetComponentNameResponse> writer_;
    GetComponentNameResponse response_;
};

class InitializeCallData : public CallData {
  public:
    InitializeCallData(BmiService::AsyncService* service, ServerCompletionQueue* cq, Function callback)
     : CallData(service, cq, callback), writer_(&context_)
    {
      service->Requestinitialize(&context_, &request_, &writer_, cq_, cq_, this);
    }
    void process(bool ok) final {
      if (done_) {
        new InitializeCallData(service_, cq_, callback_);
        delete this;
      } else {
        CharacterVector config_file = CharacterVector::create(request_.config_file());
        callback_(config_file);
        writer_.Finish(response_, Status::OK, this);
        done_ = true;
      }
    }
  private:
    InitializeRequest request_;
    grpc::ServerAsyncResponseWriter<Empty> writer_;
    Empty response_;
};

void handle(grpc::ServerCompletionQueue* cq) {
  void* tag = nullptr;
  bool ok = false;
  while (true) {
    std::cout << "Handle" << std::endl;
    if (!cq->Next(&tag, &ok)) {
      // TODO: Need to retrieve remaining call data to delete.
      break;
    }
    static_cast<CallData*>(tag)->process(ok);
  }
}

//' Async multi method server
//'
//' @export
// [[Rcpp::export]]
void runAsyncMultiServer(Environment model, std::string ip="0.0.0.0", std::string port="50051") {
  grpc::ServerBuilder builder;

  std::string addr(ip + ":" + port);
  builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

  std::unique_ptr<grpc::ServerCompletionQueue> cq(builder.AddCompletionQueue());

  BmiService::AsyncService service;
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  new GetComponentNameCallData(&service, cq.get(), model["getComponentName"]);
  new InitializeCallData(&service, cq.get(), model["bmi_initialize"]);

  std::cout << "Server listening on " << addr << std::endl;
  handle(cq.get());
}
