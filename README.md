Contains abstract super class which implements the basic model interface (bmi) in R.

See https://github.com/eWaterCycle/grpc4bmi for the Python server/client implementation.

# Install

From R

```R
devtools::install_github("eWaterCycle/bmi-r")
```

# Usage

First the model should be wrapped in a basic model interface be subclassing the AbstractBmi class.
Then the server can be started with:

```
pip install grpc4bmi[R]
run-bmi-server --lang R --name <path to r script with bmi class>.<bmi class name>
```

## Docker container

So for example for https://github.com/ClaudiaBrauer/WALRUS

```Dockerfile
FROM r-base

RUN installGithub.r ClaudiaBrauer/WALRUS eWaterCycle/bmi-r

RUN mkdir /opt/walrus-bmi

COPY walrus-bmi.r /opt/walrus-bmi/walrus-bmi.r

WORKDIR /opt/walrus-bmi

RUN apt update && apt install -y python3-dev python3-pip && pip3 install grpc4bmi[R]
CMD run-bmi-server --lang R --name walrus-bmi.WalrusBmi --path /opt/walrus-bmi --port 55555
EXPOSE 55555
```

To run server use
```bash
docker run -d -v $PWD:/data -p 55555:55555 <docker image from ewatercycle/grpc4bmi-r>
```
The config file for the bmi initialize function should be put in current working directory and the initialize function should be called with `/data/<config filename`.

# Release

## Generate man pages

```bash
Rscript -e "devtools::document(roclets=c('rd', 'collate', 'namespace'))"
```

## Local build & install

```bash
Rscript -e 'devtools::document()'
R CMD INSTALL .
Rscript -e 'grpc4bmi::run()'
```
