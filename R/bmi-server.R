#!/usr/bin/env r

#' @useDynLib grpc4bmi
#' @importFrom Rcpp evalCpp
NULL

#' @export
MockedBmi <- R6Class(
  inherit = AbstractBmi,
  public = list(
    bmi_initialize = function(config_file) {
      print(config_file)
    },
    getComponentName = function() return('mock')
  )
)

#' Start a gRPC server wrapping a bmi model
#''
#' @return none
#' @export
run <- function() {
  model = MockedBmi$new()

  runServer(model, port="55555")
  invisible(NULL)
}
