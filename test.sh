make clean

make all

rm filesys.dat

./mkfs filesys.dat 256 40

./mkdir /rohit

echo "howdy, podner" | ./tee /rohit/file.txt

./ls /rohit/file.txt

./ln /rohit/file.txt /abc/newFile.txt

./ls /rohit/file.txt

./mkdir /abc

./ln /rohit/file.txt /abc/newFile.txt

./ls /rohit/file.txt

./ls /abc/newFile.txt

./ls /rohit

./ls /abc


