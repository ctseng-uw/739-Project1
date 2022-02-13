Installation:

1. Download the tar file from : https://thrift.apache.org/download
2. Run the configuration scripts to configure thrift.: ./configure --with-cpp  --with-boost=/usr/local --with-python --without-csharp --without-java --without-erlang --without-perl --without-php --without-php_extension --without-ruby --without-haskell --without-go here, we’re only enabling cpp and python and disabling the rest
3. Now, run the command to build thrift: make
4. Run the test suite: make check
5. Run the install command: make install. You’ll have to be the root user to perform this operation (which is one of the reasons for not being able to implement this on CSL machines)
6. To run with increased optimization execute step 2 with the flag  CXXFLAGS='-g -<optimization level>’. Replace <optimization level> with the relevant optimization flag (such as O0, O2 etc). By default it is O0. Execute from step 3 to proceed with the changes.

Implementation and Execution:

1. Now that you have thrift installed, create the thrift IDL (Interface Definition Language). You’ll be adding the abstract methods that you would want to implement. Refer to ClientServerIdl.thrift
2. Run the command - thrift --gen cpp ClientServerIdl.thrift to generate the stubs and skeleton code for the server
3. Now, create the Server.cpp file from the skeleton code and add the implementation to all the methods from ClientServerIdl.thrift (or the file created in step 6)
4. Create the Client.cpp file to implement the methods exposed by the server (in Server.cpp)
5. Run the command - ./server & to run the server in the background
6. Now run the client by running the command - client -a <flag> in the above command, flag can be the following: a. 1: marshalling and unmarshalling of messages and the time taken for them b. 2: round trip time for various data types c. 3: bandwidth calculation for varying message sizes. run with the -h flag for help

  
Reference: 
1. https://thrift.apache.org/
2. Programmer's guide to Apache Thrift by Randy Abernethy
