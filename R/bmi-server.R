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
    foobar = function() {
      print('Been here')
    }
  )
)

#' Start a gRPC server wrapping a bmi model
#'
#' @return none
#' @export
run <- function() {
  model = MockedBmi$new()

  runServer(model, port="55555")
  invisible(NULL)
}

#' Call R func via cpp
#'
#' @export
bla <- function() {
  model = MockedBmi$new()

  callmyR(model)
}

#' Run async server
#'
#' @export
aserver <- function() {
   model = MockedBmi$new()
   runAsyncMultiServer(model);
}
