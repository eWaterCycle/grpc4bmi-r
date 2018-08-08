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
using bmi::GetVarRequest;
using bmi::GridRequest;

IntegerVector intrepeat2vector(::google::protobuf::RepeatedField<::google::protobuf::int32> input) {
  IntegerVector vector(input.size());
  for( int i=0; i < input.size(); i++ ) {
    vector[i] = input[i];
  }
  return vector;
}

DoubleVector floatrepeat2vector(::google::protobuf::RepeatedField<float> input) {
  DoubleVector vector(input.size());
  for( int i=0; i < input.size(); i++ ) {
    vector[i] = input[i];
  }
  return vector;
}

DoubleVector doublerepeat2vector(::google::protobuf::RepeatedField<double> input) {
  DoubleVector vector(input.size());
  for( int i=0; i < input.size(); i++ ) {
    vector[i] = input[i];
  }
  return vector;
}

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

class GetVarGridCallData : public CallData {
 public:
  GetVarGridCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetVarGrid(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetVarGridCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        NumericVector grid_id = callback_(name);
        response_.set_grid_id(grid_id[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarGridResponse> writer_;
  bmi::GetVarGridResponse response_;
};

class GetVarTypeCallData : public CallData {
 public:
  GetVarTypeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetVarType(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetVarTypeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        StringVector type = callback_(name);
        response_.set_type(type[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarTypeResponse> writer_;
  bmi::GetVarTypeResponse response_;
};

class GetVarItemSizeCallData : public CallData {
 public:
  GetVarItemSizeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetVarItemSize(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetVarItemSizeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        NumericVector size = callback_(name);
        response_.set_size(size[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarItemSizeResponse> writer_;
  bmi::GetVarItemSizeResponse response_;
};

class GetVarUnitsCallData : public CallData {
 public:
  GetVarUnitsCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetVarUnits(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetVarUnitsCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        StringVector unit = callback_(name);
        response_.set_units(unit[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarUnitsResponse> writer_;
  bmi::GetVarUnitsResponse response_;
};

class GetVarNBytesCallData : public CallData {
 public:
  GetVarNBytesCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetVarNBytes(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetVarNBytesCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        NumericVector unit = callback_(name);
        response_.set_nbytes(unit[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetVarNBytesResponse> writer_;
  bmi::GetVarNBytesResponse response_;
};

class GetValueCallData : public CallData {
 public:
  GetValueCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetValue(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetValueCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        SEXP values = callback_(name);
        switch(TYPEOF(values)) {
          case REALSXP: {
            NumericVector double_values = as<NumericVector>(values);
            for( int i=0; i < double_values.size(); i++ ) {
              response_.add_values_double(double_values[i]);
            }
           break;
          }
          case INTSXP: {
            IntegerVector int_values = as<IntegerVector>(values);
            for( int i=0; i < int_values.size(); i++ ) {
              response_.add_values_int(int_values[i]);
            }
            break;
          }
          default: {
            stop("Incompatible value type, compatible types are integer or double");
          }
        }
        NumericVector dims = as<NumericVector>(values).attr("dim");
        for( int i=0; i < dims.size(); i++ ) {
          response_.add_shape(dims[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GetVarRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetValueResponse> writer_;
  bmi::GetValueResponse response_;
};

class GetValueAtIndicesCallData : public CallData {
 public:
  GetValueAtIndicesCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetValueAtIndices(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetValueAtIndicesCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        IntegerVector indices = intrepeat2vector(request_.indices());
        SEXP values = callback_(name, indices);
        switch(TYPEOF(values)) {
          case REALSXP: {
            NumericVector double_values = as<NumericVector>(values);
            for( int i=0; i < double_values.size(); i++ ) {
              response_.add_values_double(double_values[i]);
            }
           break;
          }
          case INTSXP: {
            IntegerVector int_values = as<IntegerVector>(values);
            for( int i=0; i < int_values.size(); i++ ) {
              response_.add_values_int(int_values[i]);
            }
            break;
          }
          default: {
            stop("Incompatible value type, compatible types are integer or double");
          }
        }
        NumericVector dims = as<NumericVector>(values).attr("dim");
        for( int i=0; i < dims.size(); i++ ) {
          response_.add_shape(dims[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::GetValueAtIndicesRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetValueAtIndicesResponse> writer_;
  bmi::GetValueAtIndicesResponse response_;
};

class SetValueCallData : public CallData {
 public:
  SetValueCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestsetValue(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new SetValueCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        IntegerVector shape = intrepeat2vector(request_.shape());
        IntegerVector ints = intrepeat2vector(request_.values_int());
        DoubleVector values_float = floatrepeat2vector(request_.values_float());
        DoubleVector values_double = doublerepeat2vector(request_.values_double());
        if (ints.size() > 0) {
          ints.attr("dim") = shape;
          callback_(name, ints);
        } else if (values_float.size() > 0) {
          values_float.attr("dim") = shape;
          callback_(name, values_float);
        } else {
          values_double.attr("dim") = shape;
          callback_(name, values_double);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::SetValueRequest request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class SetValueAtIndicesCallData : public CallData {
 public:
  SetValueAtIndicesCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestsetValueAtIndices(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new SetValueAtIndicesCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        StringVector name = StringVector::create(request_.name());
        IntegerVector indices = intrepeat2vector(request_.values_int());
        IntegerVector ints = intrepeat2vector(request_.values_int());
        DoubleVector values_float = floatrepeat2vector(request_.values_float());
        DoubleVector values_double = doublerepeat2vector(request_.values_double());
        if (ints.size() > 0) {
          callback_(name, indices, ints);
        } else if (values_float.size() > 0) {
          callback_(name, indices, values_float);
        } else {
          callback_(name, indices, values_double);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  bmi::SetValueAtIndicesRequest request_;
  grpc::ServerAsyncResponseWriter<Empty> writer_;
  Empty response_;
};

class GetGridSizeCallData : public CallData {
 public:
  GetGridSizeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridSize(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridSizeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector size = callback_(grid_id);
        response_.set_size(size[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }

 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridSizeResponse> writer_;
  bmi::GetGridSizeResponse response_;
};

class GetGridTypeCallData : public CallData {
 public:
  GetGridTypeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridType(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridTypeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        StringVector type = callback_(grid_id);
        response_.set_type(type[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridTypeResponse> writer_;
  bmi::GetGridTypeResponse response_;
};

class GetGridRankCallData : public CallData {
 public:
  GetGridRankCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridRank(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridRankCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector rank = callback_(grid_id);
        response_.set_rank(rank[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridRankResponse> writer_;
  bmi::GetGridRankResponse response_;
};

class GetGridShapeCallData : public CallData {
 public:
  GetGridShapeCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridShape(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridShapeCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector shape = callback_(grid_id);
        for( int i=0; i < shape.size(); i++ ) {
          response_.add_shape(shape[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridShapeResponse> writer_;
  bmi::GetGridShapeResponse response_;
};

class GetGridSpacingCallData : public CallData {
 public:
  GetGridSpacingCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridSpacing(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridSpacingCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        DoubleVector spacing = callback_(grid_id);
        for( int i=0; i < spacing.size(); i++ ) {
          response_.add_spacing(spacing[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridSpacingResponse> writer_;
  bmi::GetGridSpacingResponse response_;
};

class GetGridOriginCallData : public CallData {
 public:
  GetGridOriginCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridOrigin(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridOriginCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        DoubleVector origin = callback_(grid_id);
        for( int i=0; i < origin.size(); i++ ) {
          response_.add_origin(origin[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridOriginResponse> writer_;
  bmi::GetGridOriginResponse response_;
};

class GetGridXCallData : public CallData {
 public:
  GetGridXCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridX(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridXCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        DoubleVector coord = callback_(grid_id);
        for( int i=0; i < coord.size(); i++ ) {
          response_.add_coordinates(coord[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridPointsResponse> writer_;
  bmi::GetGridPointsResponse response_;
};

class GetGridYCallData : public CallData {
 public:
  GetGridYCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridY(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridYCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        DoubleVector coord = callback_(grid_id);
        for( int i=0; i < coord.size(); i++ ) {
          response_.add_coordinates(coord[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridPointsResponse> writer_;
  bmi::GetGridPointsResponse response_;
};

class GetGridZCallData : public CallData {
 public:
  GetGridZCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridZ(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridZCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        DoubleVector coord = callback_(grid_id);
        for( int i=0; i < coord.size(); i++ ) {
          response_.add_coordinates(coord[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridPointsResponse> writer_;
  bmi::GetGridPointsResponse response_;
};

class GetGridCellCountCallData : public CallData {
 public:
  GetGridCellCountCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridCellCount(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridCellCountCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector count = callback_(grid_id);
        response_.set_count(count[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetCountResponse> writer_;
  bmi::GetCountResponse response_;
};

class GetGridPointCountCallData : public CallData {
 public:
  GetGridPointCountCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridPointCount(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridPointCountCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector count = callback_(grid_id);
        response_.set_count(count[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetCountResponse> writer_;
  bmi::GetCountResponse response_;
};

class GetGridVertexCountCallData : public CallData {
 public:
  GetGridVertexCountCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridVertexCount(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridVertexCountCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector count = callback_(grid_id);
        response_.set_count(count[0]);
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetCountResponse> writer_;
  bmi::GetCountResponse response_;
};

class GetGridConnectivityCallData : public CallData {
 public:
  GetGridConnectivityCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridConnectivity(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridConnectivityCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector links = callback_(grid_id);
        for( int i=0; i < links.size(); i++ ) {
          response_.add_links(links[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridConnectivityResponse> writer_;
  bmi::GetGridConnectivityResponse response_;
};

class GetGridOffsetCallData : public CallData {
 public:
  GetGridOffsetCallData(BmiService::AsyncService *service, ServerCompletionQueue *cq,
                 Function callback)
      : CallData(service, cq, callback), writer_(&context_) {
    service->RequestgetGridOffset(&context_, &request_, &writer_, cq_, cq_, this);
  }
  void process(bool ok) final {
    if (done_) {
      new GetGridOffsetCallData(service_, cq_, callback_);
      delete this;
    } else {
      try {
        IntegerVector grid_id = IntegerVector::create(request_.grid_id());
        IntegerVector offsets = callback_(grid_id);
        for( int i=0; i < offsets.size(); i++ ) {
          response_.add_offsets(offsets[i]);
        }
        writer_.Finish(response_, Status::OK, this);
      } catch (const std::exception &e) {
        writer_.FinishWithError(Status(StatusCode::INTERNAL, e.what()), this);
      }
      done_ = true;
    }
  }
 private:
  GridRequest request_;
  grpc::ServerAsyncResponseWriter<bmi::GetGridOffsetResponse> writer_;
  bmi::GetGridOffsetResponse response_;
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
  new GetVarGridCallData(&service, cq.get(), model["getVarGrid"]);
  new GetVarTypeCallData(&service, cq.get(), model["getVarType"]);
  new GetVarItemSizeCallData(&service, cq.get(), model["getVarItemSize"]);
  new GetVarUnitsCallData(&service, cq.get(), model["getVarUnits"]);
  new GetVarNBytesCallData(&service, cq.get(), model["getVarNBytes"]);
  new GetValueCallData(&service, cq.get(), model["getValue"]);
  new GetValueAtIndicesCallData(&service, cq.get(), model["getValueAtIndices"]);
  new SetValueCallData(&service, cq.get(), model["setValue"]);
  new SetValueAtIndicesCallData(&service, cq.get(), model["setValueAtIndices"]);
  new GetGridSizeCallData(&service, cq.get(), model["getGridSize"]);
  new GetGridTypeCallData(&service, cq.get(), model["getGridType"]);
  new GetGridRankCallData(&service, cq.get(), model["getGridRank"]);
  new GetGridShapeCallData(&service, cq.get(), model["getGridShape"]);
  new GetGridSpacingCallData(&service, cq.get(), model["getGridSpacing"]);
  new GetGridOriginCallData(&service, cq.get(), model["getGridOrigin"]);
  new GetGridXCallData(&service, cq.get(), model["getGridX"]);
  new GetGridYCallData(&service, cq.get(), model["getGridY"]);
  new GetGridXCallData(&service, cq.get(), model["getGridZ"]);
  new GetGridCellCountCallData(&service, cq.get(), model["getGridCellCount"]);
  new GetGridPointCountCallData(&service, cq.get(), model["getGridPointCount"]);
  new GetGridVertexCountCallData(&service, cq.get(), model["getGridVertexCount"]);
  new GetGridConnectivityCallData(&service, cq.get(), model["getGridConnectivity"]);
  new GetGridOffsetCallData(&service, cq.get(), model["getGridOffset"]);

  std::cout << "Server listening on " << addr << std::endl;
  handle(cq.get());
}
