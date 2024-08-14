FROM ubuntu:20.04
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y libpqxx-dev g++ make
RUN mkdir /src
WORKDIR /src
COPY . /src
CMD ["./server"]

