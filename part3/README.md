1. Build and locally install gRPC and Protocol Buffers using cmake.
Choose a directory to hold locally installed packages. This page assumes that the environment variable MY_INSTALL_DIR holds this directory path.

$ export MY_INSTALL_DIR=$HOME/.local
$ mkdir -p $MY_INSTALL_DIR
$ export PATH="$MY_INSTALL_DIR/bin:$PATH"

The CSL machines already have cmake installed. Verify this with
$ cmake --version

2. Next, clone the gRPC repo
$ git clone --recurse-submodules -b v1.43.0 https://github.com/grpc/grpc

3. The following commands build and locally install gRPC and Protocol Buffers:
$ cd grpc
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
$ make -j 4
$ make install
$ popd

4. Change to the example’s directory:
$ cd examples/cpp/helloworld

Build the example using cmake
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
$ make -j 4

5. Run the client and the server
On one terminal, run the server as follows
$ ./greeter_server

On another terminal,
$ ./greeter_client

6. Replace the contents of examples/protos/helloworld.proto with the contents of helloworld.proto provided here in part3 and make 
$make -j 4

7. Inside the examples/cpp/helloworld/ replace greeter_server.cc and greeter_client.cc with the files with the same name provided here in part3 and make.

8. Repeat step 5 to get the measurements

9. For running the client on a separate machine, ssh into another machine, royal-13 for instance, and navigate to examples/cpp/helloworld/cmake.build
$ ./greeter_client --target=royal-13.cs.wisc.edu:50051

10. For compiler optimization, add the following in CMakeLists.txt under # Options
add_compile_options(-O3)
make clean and repeat step 1,3,4,5 to get measurements