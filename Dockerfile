FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libpq-dev \
    bison \
    flex \
    autoconf

WORKDIR /app

RUN git clone https://github.com/microsoft/vcpkg.git
RUN ./vcpkg/bootstrap-vcpkg.sh

COPY vcpkg.json ./
COPY CMakeLists.txt ./

COPY src ./src
COPY static ./static

RUN cmake . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
RUN cmake --build build

FROM ubuntu:22.04

WORKDIR /app

COPY --from=builder /app/build/app .
COPY --from=builder /app/static ./static

EXPOSE 18080

CMD ["./app"]
