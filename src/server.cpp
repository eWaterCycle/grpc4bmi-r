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
using grpc::StatusCode;
using bmi::Empty;
using bmi::BmiService;

class CallData {
 public:
  CallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
           Function callback)
      : cq_(cq), service_(service), callback_(callback) {}
  virtual ~CallData() {}
  virtual void process(bool ok) = 0;

 protected:
  bool done_ = false;
  grpc::ServerContext context_;
  grpc::ServerCompletionQueue *cq_;
  BmiService::AsyncService *service_;
  Function callback_;

 private:
  CallData(const CallData &) = delete;
  CallData &operator=(const CallData &) = delete;
};

class GetComponentNameCallData : public CallData {
 public:
  GetComponentNameCallData(BmiService::AsyncService *service,
                           ServerCompletionQueue *cq, Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetComponentName(&context_, &request_, &writer_, cq_, cq_,
                                     this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetComponentNameCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        std::string name = as<std::string>(callback_());
        response_.set_name(name);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetComponentNameResponse> writer_;
  bmi::GetComponentNameResponse response_;
};

class InitializeCallData : public CallData {
 public:
  InitializeCallData(BmiService::AsyncService *service,
                     ServerCompletionQueue *cq, Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->Requestinitialize(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new InitializeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        CharacterVector config_file =
            CharacterVector::create(request_.config_file());
        callback_(config_file);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::InitializeRequest request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class UpdateCallData : public CallData {
 public:
  UpdateCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->Requestupdate(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new UpdateCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        callback_();
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class UpdateUntilCallData : public CallData {
 public:
  UpdateUntilCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestupdateUntil(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new UpdateUntilCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector until = NumericVector::create(request_.until());
        callback_(until);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::UpdateUntilRequest request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class UpdateFracCallData : public CallData {
 public:
  UpdateFracCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestupdateFrac(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new UpdateFracCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector frac = NumericVector::create(request_.frac());
        callback_(frac);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::UpdateFracRequest request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class FinalizeCallData : public CallData {
 public:
  FinalizeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->Requestfinalize(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new FinalizeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        callback_();
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::Empty request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class GetInputVarNameCountCallData : public CallData {
 public:
  GetInputVarNameCountCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetInputVarNameCount(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetInputVarNameCountCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector result = callback_();
        int count = result[0];
        response_.set_count(count);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarNameCountResponse> writer_;
  bmi::GetVarNameCountResponse response_;
};

class GetOutputVarNameCountCallData : public CallData {
 public:
  GetOutputVarNameCountCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetOutputVarNameCount(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetOutputVarNameCountCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector result = callback_();
        int count = result[0];
        response_.set_count(count);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarNameCountResponse> writer_;
  bmi::GetVarNameCountResponse response_;
};

class GetInputVarNamesCallData : public CallData {
 public:
  GetInputVarNamesCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetInputVarNames(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetInputVarNamesCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector names = callback_();
        for( int i=0; i < names.size(); i++ ) {
          response_.add_names(names[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarNamesResponse> writer_;
  bmi::GetVarNamesResponse response_;
};

class GetOutputVarNamesCallData : public CallData {
 public:
  GetOutputVarNamesCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetOutputVarNames(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetOutputVarNamesCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector names = callback_();
        for( int i=0; i < names.size(); i++ ) {
          response_.add_names(names[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarNamesResponse> writer_;
  bmi::GetVarNamesResponse response_;
};

class GetTimeUnitsCallData : public CallData {
 public:
  GetTimeUnitsCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetTimeUnits(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetTimeUnitsCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector units = callback_();
        response_.set_units(units[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetTimeUnitsResponse> writer_;
  bmi::GetTimeUnitsResponse response_;
};

class GetTimeStepCallData : public CallData {
 public:
  GetTimeStepCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetTimeStep(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetTimeStepCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector interval = callback_();
        response_.set_interval(interval[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetTimeStepResponse> writer_;
  bmi::GetTimeStepResponse response_;
};

class GetCurrentTimeCallData : public CallData {
 public:
  GetCurrentTimeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetCurrentTime(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetCurrentTimeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector times = callback_();
        response_.set_time(times[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetTimeResponse> writer_;
  bmi::GetTimeResponse response_;
};

class GetStartTimeCallData : public CallData {
 public:
  GetStartTimeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetStartTime(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetStartTimeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector times = callback_();
        response_.set_time(times[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetTimeResponse> writer_;
  bmi::GetTimeResponse response_;
};

class GetEndTimeCallData : public CallData {
 public:
  GetEndTimeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetEndTime(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetEndTimeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        NumericVector times = callback_();
        response_.set_time(times[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  Empty request_;
  grpc::ServerAsyncResponseWriter<bmi::GetTimeResponse> writer_;
  bmi::GetTimeResponse response_;
};

void handle(grpc::ServerCompletionQueue *cq) {
  void *tag = nullptr; // uniquely identifies a request.
  bool ok = false;
  while (true) {
    GPR_ASSERT(cq->Next(&tag, &ok));
    GPR_ASSERT(ok);
    static_cast<CallData *>(tag)->process(ok);
  }
}

//' Async multi method server
//'
//' @export
// [[Rcpp::export]]
void runAsyncMultiServer(Environment model, std::string ip = "0.0.0.0",
                         std::string port = "50051") {
  grpc::ServerBuilder builder;

  std::string addr(ip + ":" + port);
  builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

  std::unique_ptr<grpc::ServerCompletionQueue> cq(builder.AddCompletionQueue());

  BmiService::AsyncService service;
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  new GetComponentNameCallData(&service, cq.get(), model["getComponentName"]);
  new InitializeCallData(&service, cq.get(), model["bmi_initialize"]);
  new UpdateCallData(&service, cq.get(), model["update"]);
  new UpdateUntilCallData(&service, cq.get(), model["updateUntil"]);
  new UpdateFracCallData(&service, cq.get(), model["updateFrac"]);
  new FinalizeCallData(&service, cq.get(), model["bmi_finalize"]);
  new GetInputVarNameCountCallData(&service, cq.get(), model["getInputVarNameCount"]);
  new GetOutputVarNameCountCallData(&service, cq.get(), model["getOutputVarNameCount"]);
  new GetInputVarNamesCallData(&service, cq.get(), model["getInputVarNames"]);
  new GetOutputVarNamesCallData(&service, cq.get(), model["getOutputVarNames"]);
  new GetTimeUnitsCallData(&service, cq.get(), model["getTimeUnits"]);
  new GetTimeStepCallData(&service, cq.get(), model["getTimeStep"]);
  new GetCurrentTimeCallData(&service, cq.get(), model["getCurrentTime"]);
  new GetStartTimeCallData(&service, cq.get(), model["getStartTime"]);
  new GetEndTimeCallData(&service, cq.get(), model["getEndTime"]);

  // TODO wrap other model methods

  std::cout << "Server listening on " << addr << std::endl;
  handle(cq.get());
}
