FROM r-base
LABEL maintainer="Stefan Verhoeven <s.verhoeven@esciencecenter.nl>"

RUN apt-get update \ 
	&& apt-get install -y --no-install-recommends \
		libcurl4-openssl-dev curl \
        git libgit2-dev libssl-dev \
        build-essential autoconf libtool pkg-config dh-autoreconf \
	&& rm -rf /var/lib/apt/lists/*

# Support for `Remotes: <github>` in DESCRIPTION
RUN install.r remotes devtools

RUN git clone --recursive -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc && \
  cd /grpc && make -j $(nproc) && cd third_party/protobuf && make install && cd - && \
  make install && ldconfig && cd / && rm -rf /grpc

COPY . /opt/grpc4bmi

RUN Rscript -e 'devtools::install("/opt/grpc4bmi", dependencies=TRUE)'

RUN mkdir /data
WORKDIR /data
VOLUME /data

ENV BMI_PORT=55555

CMD ["Rscript", "-e", "grpc4bmi::run()"]

EXPOSE 55555
