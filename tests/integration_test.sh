#!/bin/bash

cd /usr/src/projects/build 
# Build the web server binary
echo 'BUILDING'
make clean
make
echo "BUILD COMPLETE"

#config for the server. Outputs config to file
echo "http {
  server {
    listen 8080;
    server_name 127.0.0.1;
  }
}" > ../tests/integration_test_config

#build the server with the config file
./bin/server_main ../tests/integration_test_config & 
#PROCESS_ID = $!
#echo $PROCESS_ID
#send server a request
echo "SENDING SERVER REQUEST"
curl -i http://localhost:8080 > ../tests/integration_test_response
#curl -i -s GET / HTTP/1.1> /usr/src/projects/git-r-done/tests/integration_test_response
echo "CHECKING REQUEST FOR CORRECTNESS"
#check response of correctness
DIFF=$(diff -w -B ../tests/integration_expected_response ../tests/integration_test_response)
EXIT_STATUS=$? 

if [ "$EXIT_STATUS" -eq 0 ]
then
    echo "SUCCESS: Integration test passed"
else
    echo "FAILED: Integration test failed"
    echo $DIFF
fi 

echo "SHUTTING DOWN"
# Shutdown the webserver 
pkill server_main

rm -f ../tests/integration_test_response

# success (0) or failure (1)
exit "$EXIT_STATUS"
