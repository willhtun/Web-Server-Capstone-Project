#!/bin/bash

cd /usr/src/project 
# Build the web server binary
echo 'BUILDING'
make clean
make
echo "BUILD COMPLETE"

#config for the server. Outputs config to file
echo "http {
  server {
    listen 8080;
    handler echo {
      url /echo;
    }
    handler proxy {
      url /proxy1;
      host www3.coolcoders.cs130.org;
      path /static_1/a.txt;
      port 80;
    }
    server_name 127.0.0.1;
  }
}" > ./tests/integration/integration_test_config

#build the server with the config file
./build/bin/server_main ./tests/integration/integration_test_config & 
#PROCESS_ID = $!
#echo $PROCESS_ID
#send server a request
echo "TESTING ECHO HANDLER"
echo "SENDING SERVER REQUEST"
curl -i http://35.233.235.101/echo > ./tests/integration/integration_test_response
#curl -i -s GET / HTTP/1.1> /usr/src/projects/git-r-done/tests/integration_test_response
echo "CHECKING REQUEST FOR CORRECTNESS"
#check response of correctness
DIFF=$(diff -w -B ./tests/integration/integration_expected_response ./tests/integration/integration_test_response)
EXIT_STATUS=$? 

if [ "$EXIT_STATUS" -eq 0 ]
then
    echo "SUCCESS: Echo handler integration test passed"
else
    echo "FAILED: Echo handler integration test failed"
    echo $DIFF
fi 

# echo "TESTING PROXY HANDLER"
# echo "SENDING SERVER REQUEST"
# curl -L localhost:8080/proxy1 > ./tests/integration/integration_test_proxy_response
# #check response of correctness
# DIFF=$(diff -w -B ./tests/integration/integration_expected_proxy_response ./tests/integration/integration_test_proxy_response)
# EXIT_STATUS=$? 

# if [ "$EXIT_STATUS" -eq 0 ]
# then
#     echo "SUCCESS: Proxy handler integration test passed"
# else
#     echo "FAILED: Proxy handler integration test failed"
#     echo $DIFF
# fi 

echo "SHUTTING DOWN"
# Shutdown the webserver 
pkill server_main

rm -f ./tests/integration/integration_test_response

# success (0) or failure (1)
exit "$EXIT_STATUS"
