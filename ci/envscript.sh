echo "Cloning EMSDK"

git clone https://github.com/emscripten-core/emsdk.git/
cd emsdk
pwd

git reset --hard 322c7aa   #this is the commit before the change messed us up.

# Download and install the right SDK tool
#./emsdk install sdk-fastcomp-tag-1.38.30-64bit
./emsdk install sdk-1.38.30-64bit

# Uninstall version of node that causes issues
#./emsdk uninstall node-8.9.1-64bit 

# Activate the right version of the SDK
#./emsdk activate sdk-fastcomp-tag-1.38.30-64bit
./emsdk activate sdk-1.38.30-64bit

# activate PATH and other env variables in the current terminal
source ./emsdk_env.sh

./emsdk list

emcc -v

cd ..

echo $EMSDK