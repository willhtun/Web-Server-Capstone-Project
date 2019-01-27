### Build/test container ###
FROM server_gitrdone:base as builder

COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN cmake ..
RUN make

### Build/run coverage report ###
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage

