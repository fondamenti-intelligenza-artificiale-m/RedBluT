cd mio_progetto
mkdir build
cd build
cmake ..
make

cd build
rm -rf *
cmake ..
make

oppure 

cd ..
rm -rf build
mkdir build && cd build
cmake ..
make
