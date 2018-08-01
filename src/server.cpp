#define STRICT_R_HEADERS
#include <Rcpp.h>
using namespace Rcpp;

#include <grpcpp/grpcpp.h>

#include "bmi.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using bmi::Empty;
using bmi::InitializeRequest;
using bmi::GetComponentNameResponse;
using bmi::BmiService;

// class AbstractBmi {
//   public:
//     void bmi_finalize(std::string config_file) {
//       Rcpp:stop("Not implemented");
//     }
//     std::string getComponentName() {
//       Rcpp:stop("Not implemented");
//     }
// };

class BmiServiceImpl final : public BmiService::Service {
  Environment model;
  public:
    explicit BmiServiceImpl(Environment inmodel) {
      model = inmodel;
    }
    Status initialize(ServerContext* context, const InitializeRequest* request, ::bmi::Empty* response) override {
      Function callback = model["bmi_initialize"];
      CharacterVector config_file = CharacterVector::create(request->config_file());
      callback(config_file);
      return Status::OK;
    }
    Status getComponentName(ServerContext* context, const Empty* request, GetComponentNameResponse* response) override {
      Function callback = model["getComponentName"];
      std::string name = as<std::string>(callback());
      // std::string name = "WALRUS";
      std::cout << "My name is " << name << std::endl;
      response->set_name(name);
      return Status::OK;
    }
};

//' Run grpc server using bmi model
//'
//' @param model model which is subclass of AbstractBmi
//' @param ip Ip adress to bind server to
//' @param port TCP port to listen on
//' @export
// [[Rcpp::plugins("cpp11")]]
// [[Rcpp::export]]
void runServer(Environment model, std::string ip="0.0.0.0", std::string port="500051") {
  std::string server_address(ip + ":" + port);
  BmiServiceImpl service(model);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}
