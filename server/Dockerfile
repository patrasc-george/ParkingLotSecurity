FROM ubuntu:22.04 AS build-server

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-10 g++-10 \
    wget \
    libpq-dev \
    nlohmann-json3-dev \
    libqrencode-dev \
    git \
    libopenmpi-dev \
    curl \
    zip \
    unzip \
    tar \
    libssl-dev \
	libssl3 \
    ca-certificates \
    libstdc++6 \
    libgcc-s1 \
    libpq5 \
    cmake \
    libexpat1-dev \
    zlib1g-dev \
    libcrypto++-dev \
    libboost-all-dev \
    libpoco-dev \
    libgtk-3-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libcanberra-gtk* \
    libatlas-base-dev \
    gfortran \
    python3-dev \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

ARG OPENCV_VERSION=4.10.0
RUN git clone --depth 1 --branch ${OPENCV_VERSION} https://github.com/opencv/opencv.git /opencv && \
    git clone --depth 1 --branch ${OPENCV_VERSION} https://github.com/opencv/opencv_contrib.git /opencv_contrib && \
    mkdir /opencv/build && \
    cd /opencv/build && \
    cmake -D CMAKE_BUILD_TYPE=Release \
          -D CMAKE_INSTALL_PREFIX=/usr/local \
          -D OPENCV_EXTRA_MODULES_PATH=/opencv_contrib/modules \
          -D BUILD_EXAMPLES=OFF \
          -D INSTALL_C_EXAMPLES=OFF \
          -D INSTALL_PYTHON_EXAMPLES=OFF \
          -D OPENCV_ENABLE_NONFREE=ON \
          -D BUILD_TESTS=OFF \
          -D BUILD_PERF_TESTS=OFF \
          -D BUILD_opencv_python3=OFF \
          -D BUILD_opencv_python2=OFF \
          -D CMAKE_CXX_STANDARD=17 .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    cd /vcpkg && \
    ./bootstrap-vcpkg.sh

RUN /vcpkg/vcpkg install poco[netssl,crypto]:x64-linux
RUN /vcpkg/vcpkg install boost-beast:x64-linux
RUN /vcpkg/vcpkg install boost-asio:x64-linux
RUN /vcpkg/vcpkg install nu-book-zxing-cpp:x64-linux

ENV VCPKG_ROOT=/vcpkg
ENV PATH="$VCPKG_ROOT:$PATH"
ENV LD_LIBRARY_PATH=/app/server:/usr/local/lib:/vcpkg/installed/x64-linux/lib:/usr/lib:/usr/lib/x86_64-linux-gnu

COPY . /app/server
WORKDIR /app/server

RUN cmake . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_POLICY_DEFAULT_CMP0167=NEW \
    -DCMAKE_PREFIX_PATH="/vcpkg/installed/x64-linux;/vcpkg/installed/x64-linux/share/poco" \
    && make VERBOSE=1

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libgcc-s1 \
    libpq5 \
    libqrencode4 \
    libjpeg-turbo8 \
    libpng16-16 \
    libtiff5 \
    wget \
    curl \
    ca-certificates \
    libssl3 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY --from=build-server /app/server /app/server
COPY --from=build-server /usr/local /usr/local

RUN ldconfig && opencv_version || true
RUN ldd /app/server/bin/Server | grep "not found" && exit 1 || true
RUN chmod +x /app/server/bin/Server

EXPOSE 8080 9002

ENV LD_LIBRARY_PATH=/app/server:/usr/local/lib:/usr/lib:/usr/lib/x86_64-linux-gnu

WORKDIR /app/server
CMD ["/app/server/bin/Server"]