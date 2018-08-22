Wrap a model written in R which implements the basic model interface (bmi) in a gRPC server.

See https://github.com/eWaterCycle/grpc4bmi for the Python server/client implementation.

# Install

System requirements:
* Protobuf
* [grpc](https://github.com/grpc/grpc/blob/master/BUILDING.md) >=v1.14.x

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

## Docker container

To install grpc is a bit of a hassle so a Docker image is provided called [ewatercycle/grpc4bmi-r](https://hub.docker.com/r/ewatercycle/grpc4bmi-r/) which contains R and grpc installation.

To create a Docker image of a R based hydrology model use this image as a start.

So for example for https://github.com/ClaudiaBrauer/WALRUS

```Dockerfile
FROM ewatercycle/grpc4bmi-r

RUN installGithub.r ClaudiaBrauer/WALRUS

RUN mkdir /opt/walrus-bmi

COPY walrus-bmi.r /opt/walrus-bmi/walrus-bmi.r

ENV BMI_MODULE=/opt/walrus-bmi/walrus-bmi.r
ENV BMI_CLASS=WalrusBmi
```

# Release

## Generate man pages

```bash
Rscript -e "devtools::document(roclets=c('rd', 'collate', 'namespace'))"
```

## Generate cpp files from proto file

```bash
cd src
protoc -I ../inst/proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../inst/proto/bmi.proto
protoc -I ../inst/proto --cpp_out=. ../inst/proto/bmi.proto
cd -
```

## Local build & install

```bash
Rscript -e 'Rcpp::compileAttributes();devtools::document()'
R CMD INSTALL .
Rscript -e 'grpc4bmi::run()'
```
