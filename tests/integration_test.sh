#!/bin/bash

cd /usr/src/projects/git-r-done/build 
# Build the web server binary
echo 'BUILDING'
make
echo "BUILD COMPLETE"

#config for the server. Outputs config to file
echo "http {
  server {
    listen 8080;
    server_name 127.0.0.1;
  }
}" > /usr/src/projects/git-r-done/tests/integration_test_config

#build the server with the config file
bin/server_main /usr/src/projects/git-r-done/tests/integration_test_config & 

#send server a request
echo "SENDING SERVER REQUEST"
curl -s http://localhost:8080 > /usr/src/projects/git-r-done/tests/integration_test_response
#curl -i -s GET / HTTP/1.1> /usr/src/projects/git-r-done/tests/integration_test_response
echo "CHECKING REQUEST FOR CORRECTNESS"
#check response of correctness
DIFF=$(diff /usr/src/projects/git-r-done/tests/integration_expected_response /usr/src/projects/git-r-done/tests/integration_test_response)
EXIT_STATUS=$? #flag

if [ "$EXIT_STATUS" -eq 0 ]
then
    echo "SUCCESS: Integration test passed"
else
    echo "FAILED: Integration test failed"
fi 

echo "SHUTTING DOWN"
# Shutdown the webserver 
pkill server_main
make clean
#rm -f /usr/src/projects/git-r-done/tests/integration_test_response

# success (0) or failure (1)
exit "$EXIT_STATUS"
