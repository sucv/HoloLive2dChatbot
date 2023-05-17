#!/bin/bash

# Set the CMAKE path contained in Qt
CMAKE=/path/to/cmake/bin/cmake

##############################
# Setup OpenSSL
##############################

# Set the version of OpenSSL to download
OPENSSL_VERSION=cmake-1.1.1t-20230216

# Set the installation directory
INSTALL_DIR=./openssl

# Create the installation directory if it doesn't exist
mkdir -p $INSTALL_DIR

# Download OpenSSL
echo "Downloading openssl $OPENSSL_VERSION..."
curl -L "https://github.com/janbar/openssl-cmake/archive/refs/tags/1.1.1t-20230216.zip" -o openssl.zip

# Unzip OpenSSL
echo "Unzipping openssl..."
unzip openssl.zip
cp openssl-$OPENSSL_VERSION/test/certs/ca-cert.pem .

# Create the build directory
mkdir openssl-$OPENSSL_VERSION/build
cd openssl-$OPENSSL_VERSION/build

# Configure and build OpenSSL for Release
echo "Configuring openssl..."
$CMAKE .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
$CMAKE --build . --config Release --target install

# Remove the downloaded files and build directory
if [ $? -eq 0 ]; then
    cd ../..
    echo "Cleaning up..."
    rm -f openssl.zip
    rm -rf openssl-$OPENSSL_VERSION

    # Rename the libraries to be standard
    cd openssl/lib
    mv libcrypto-1_1-x64.so libcrypto.so
    mv libssl-1_1-x64.so libssl.so
    cd ../..
else
    echo "CMake failed on building openssl..."
fi

##############################
# Setup CURL
##############################

# Set the version of CURL to download
CURL_VERSION=8.0.1

# Set the installation directory
INSTALL_DIR=./curl
OPENSSL_ROOT_DIR=./openssl

# Create the installation directory if it doesn't exist
mkdir -p $INSTALL_DIR

# Download CURL
echo "Downloading curl $CURL_VERSION..."
curl -L "https://github.com/curl/curl/releases/download/curl-8_0_1/curl-8.0.1.zip" -o curl.zip

# Unzip CURL
echo "Unzipping curl..."
unzip curl.zip

# Create the build directory
mkdir curl-$CURL_VERSION/build
cd curl-$CURL_VERSION/build

# Configure and build CURL for Release
echo "Configuring curl..."
$CMAKE .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DCURL_USE_OPENSSL=ON -DOPENSSL_ROOT_DIR=$OPENSSL_ROOT_DIR
$CMAKE --build . --config Release --target install

# Remove the downloaded files and build directory
if [ $? -eq 0 ]; then
    echo "Cleaning up..."
    cd ../..
    rm -f curl.zip
    rm -rf curl-$CURL_VERSION
    rm -rf openssl

    curl -L https://curl.se/ca/cacert.pem -o curl/bin/cacert.pem

    cd curl/lib
    mv libcurl_imp.so libcurl.so
    cd ../..
else
    echo "CMake failed on building curl..."
fi

##############################
# Setup OpenCV
##############################

# Set the version of OpenCV to download
OPENCV_VERSION=4.7.0

# Set the list of OpenCV libraries to include
LIBS=(core highgui imgproc)

# Set the installation directory
INSTALL_DIR=./opencv

# Create the installation directory if it doesn't exist
mkdir -p $INSTALL_DIR

# Download OpenCV
echo "Downloading OpenCV $OPENCV_VERSION..."
curl -L "https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip" -o opencv.zip

# Unzip OpenCV
echo "Unzipping OpenCV..."
unzip opencv.zip

# Create the build directory
mkdir opencv-$OPENCV_VERSION/build
cd opencv-$OPENCV_VERSION/build

# Configure and build OpenCV for Release
echo "Configuring OpenCV..."
$CMAKE .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_BUILD_TYPE=Release -DOPENCV_DONT_USE_ALL_WITH_OPTIONS=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_LIST=${LIBS[*]}
$CMAKE --build . --config Release --target install

# Remove the downloaded files and build directory
if [ $? -eq 0 ]; then
    echo "Cleaning up..."
    cd ../..
    rm -f opencv.zip
    rm -rf opencv-$OPENCV_VERSION
    rm -rf opencv/etc
else
    echo "CMake failed on building opencv..."
fi
