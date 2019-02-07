### Build/test container ###
FROM server_gitrdone:base as builder

COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN cmake ..
RUN make
RUN ctest --output-on_failure

### Deploy container ###
FROM ubuntu:latest as deploy

# TODO(!): Copy server output binary to "."
 COPY --from=builder /usr/src/project/build/bin/server_main .

# TODO(!): Expose some port(s)
 EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
 ENTRYPOINT ["./server_main"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
 COPY --from=builder /usr/src/project/tests/echo_server_config .
 CMD ["./echo_server_config"]

 # Copy static files directory
 COPY --from=builder /usr/src/project/static_files/* ./static/static_files1
 COPY --from=builder /usr/src/project/static_files2/* ./static/static_files2
