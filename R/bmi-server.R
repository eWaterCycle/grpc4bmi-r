#!/usr/bin/env r

#' @useDynLib grpc4bmi
#' @importFrom Rcpp evalCpp
NULL

#' @export
MockedBmi <- R6Class(
  inherit = AbstractBmi,
  public = list(
    bmi_initialize = function(config_file) {
      print('Running initialize')
      print(config_file)
    },
    getComponentName = function() return('mock'),
    updateFrac = function(arg) {
      print('Been here')
      print(arg)
    },
    getInputVarNameCount = function() return(42),
    getInputVarNames = function() return(c('ETact', 'Q', 'fGS', 'fQS', 'dV', 'dVeq', 'dG', 'hQ', 'hS', 'w')),
    getVarGrid = function(name) {
      print(name);
      return(1);
    },
    getValue = function(name) {
      print(name);
      return(array(1, dim=c(2,3,4)))
      # return(array(as.integer(1), dim=c(2,3,4)))
      # return(array("1", dim=c(2,3,4)))
    }
  )
)

#' Command line interface of grpc4 bmi server
#' Use following environment variables for configuration:
#'
#' * BMI_MODULE, file which contains bmi class
#' * BMI_CLASS, class name which subclasses AbstractBmi
#' * BMI_PORT, a tcp port number to run the server on
#'
#' @export
run <- function() {
    module = Sys.getenv('BMI_MODULE')
    if (module != "") {
     source(module)
    }

    class = Sys.getenv('BMI_CLASS')
    if (class == "") {
     stop("Require BMI_CLASS environment variable with name of class which implements the bmi interface")
    }
    model <- get(class)$new()

    port = Sys.getenv('BMI_PORT', '55555')
   runAsyncMultiServer(model, port);
}

#' Start a gRPC server wrapping a mocked bmi model
#' @export
runMocked <- function() {
   model = MockedBmi$new()
   runAsyncMultiServer(model);
}
