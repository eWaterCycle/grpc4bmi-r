Contains abstract super class which implements the basic model interface (bmi) in R.

See https://github.com/eWaterCycle/grpc4bmi for the Python server/client implementation.

# Install

From R

```R
devtools::install_github("eWaterCycle/bmi-r")
```

For Python runner
```bash
pip install grpc4bmi[R]
```

# Serve the model using grpc4bmi

First the model should be wrapped in a basic model interface be subclassing the AbstractBmi class.
Then the server can be started with:

```
run-bmi-server --lang R --path <path to r script with bmi class> --name <bmi class name>
```

## Docker container

So for example for [WALRUS](https://github.com/ClaudiaBrauer/WALRUS) model, see its [Dockerfile](https://github.com/eWaterCycle/grpc4bmi-examples/blob/master/walrus/Dockerfile).

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
```
