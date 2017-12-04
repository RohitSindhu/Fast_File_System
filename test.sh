 #  Rohit Sindhu [sindh010]
 #  Aravind Alagiri Ramkumar [alagi005]
 #  Aparna Mahadevan [mahad028]

make clean

make all

rm filesys.dat

# creating file system
./mkfs filesys.dat 256 40

# make a dir
./mkdir /ara

# create a file
echo "howdy, podner" | ./tee /ara/file.txt

# see if the file was created
./ls /ara/

# create target dir
./mkdir /dir

# try linking
./ln /ara/file.txt /dir/newFile.txt

# see if the file was created
./ls /ara
./ls /dir

# create a big file
cat wr.txt | ./tee /ara/wrFile.txt

./ls /ara
./ls /dir

# see contents of file 
./cat /ara/wrFile.txt

# remove the file, should free blocks
./rm /ara/wrFile.txt
./ls /ara
./ls /dir

# remove nlinks to one file, other file should still exist
./rm /dir/newFile.txt
./ls /ara
./ls /dir