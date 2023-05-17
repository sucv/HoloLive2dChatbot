@echo off

:: Set the CMAKE path contained in Qt
set CMAKE=E:\Qt\Tools\CMake_64\bin\cmake.exe

::::::::::::::::::::
:: Setup OpenSSL ::
::::::::::::::::::::

:: Set the version of curl to download
set OPENSSL_VERSION=cmake-1.1.1t-20230216

:: Set the installation directory
set INSTALL_DIR=%~dp0\openssl

:: Create the installation directory if it doesn't exist
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

:: Download openssl
echo Downloading openssl %OPENSSL_VERSION%...
curl -L "https://github.com/janbar/openssl-cmake/archive/refs/tags/1.1.1t-20230216.zip" -o openssl.zip

:: Unzip openssl
echo Unzipping openssl...
unzip openssl.zip

:: Create the build directory
mkdir openssl-%OPENSSL_VERSION%\build
cd openssl-%OPENSSL_VERSION%\build

:: Configure and build openssl for Release
echo Configuring openssl...
%CMAKE% .. -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
%CMAKE% --build . --config Release --target install


:: Remove the downloaded files and build directory
if %errorlevel% equ 0 (
	cd ..\..
	echo Cleaning up...
	del /Q openssl.zip
	rd /s /q openssl-%OPENSSL_VERSION%

	:: Rename the libraries to be standard
	cd openssl\lib
	ren libcrypto-1_1-x64.lib libcrypto.lib
	ren libssl-1_1-x64.lib libssl.lib
	cd ..\..
) else (
	echo "CMake failed on building openssl..."
)

::::::::::::::::
:: Setup CURL ::
::::::::::::::::

:: Set the version of curl to download
set CURL_VERSION=8.0.1

:: Set the installation directory
set INSTALL_DIR=%~dp0\curl
set OPENSSL_ROOT_DIR=%~dp0\openssl

:: Create the installation directory if it doesn't exist
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

:: Download curl
echo Downloading curl %CURL_VERSION%...
curl -L "https://github.com/curl/curl/releases/download/curl-8_0_1/curl-8.0.1.zip" -o curl.zip

:: Unzip curl
echo Unzipping curl...
unzip curl.zip

:: Create the build directory
mkdir curl-%CURL_VERSION%\build
cd curl-%CURL_VERSION%\build


:: Configure and build curl for Release
echo Configuring curl...
%CMAKE% .. -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DCURL_USE_OPENSSL=ON -DOPENSSL_ROOT_DIR=%OPENSSL_ROOT_DIR%
%CMAKE% --build . --config Release --target install


:: Remove the downloaded files and build directory
if %errorlevel% equ 0 (
	echo Cleaning up...
	cd ..\..
	del /Q curl.zip
	rd /s /q curl-%CURL_VERSION%
	rd /s /q openssl

	curl -L https://curl.se/ca/cacert.pem -o curl\bin\cacert.pem

	cd curl\lib
	ren libcurl_imp.lib libcurl.lib
	cd ..\..
) else (
	echo "CMake failed on building curl..."
)


::::::::::::::::
:: Setup OPENCV ::
::::::::::::::::


:: Set the version of OpenCV to download
set OPENCV_VERSION=4.7.0

:: Set the list of OpenCV libraries to include
set LIBS=core highgui imgproc

:: Set the installation directory
set INSTALL_DIR=%~dp0\opencv

:: Create the installation directory if it doesn't exist
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

:: Download OpenCV
echo Downloading OpenCV %OPENCV_VERSION%...
curl -L "https://github.com/opencv/opencv/archive/%OPENCV_VERSION%.zip" -o opencv.zip

:: Unzip OpenCV
echo Unzipping OpenCV...
unzip opencv.zip

:: Create the build directory
mkdir opencv-%OPENCV_VERSION%\build
cd opencv-%OPENCV_VERSION%\build

:: Configure and build OpenCV for Release
echo Configuring OpenCV...
%CMAKE% .. -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_BUILD_TYPE=Release -DOPENCV_DONT_USE_ALL_WITH_OPTIONS=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_LIST=core,highgui,imgproc,imgcodecs %LIBS%
%CMAKE% --build . --config Release --target install


:: Remove the downloaded files and build directory
if %errorlevel% equ 0 (
	echo Cleaning up...
	cd ..\..
	del /Q opencv.zip
	rd /s /q opencv-%OPENCV_VERSION%
	rd /s /q opencv\etc
) else (
	echo "CMake failed on building opencv..."
)