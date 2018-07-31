Wrap a model written in R which implements the basic model interface (bmi) in a gRPC server.

See https://github.com/eWaterCycle/grpc4bmi for the Python server/client implementation.

# Install

System requirements:
* Protobuf
* grpc

From R

```R
devtools::install_github("eWaterCycle/grpc4bmi-r")
```

# Usage

First the model should be wrapped in a basic model interface be subclassing the AbstractBmi class.
Then the server can be started with:

```
export BMI_MODULE=<path to r script with bmi class>
export BMI_CLASS=<bmi class name>
export BMI_PORT=<port on which to run grpc server, default is 55555> 
Rscript -e 'grpc4bmi::run()'
```

# Release

## Generate man pages

```
Rscript -e "devtools::document(roclets=c('rd', 'collate', 'namespace'))"
```
