cmake_minimum_required(VERSION 3.8)
project(5103P3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        BitBlock.cc
        BitBlock.h
        Block.cc
        Block.h
        cat.cc
        cp.cc
        DirectoryEntry.cc
        DirectoryEntry.h
        dump.cc
        FileDescriptor.cc
        FileDescriptor.h
        FileSystem.cc
        FileSystem.h
        IndexNode.cc
        IndexNode.h
        Kernel.cc
        Kernel.h
        ls.cc
        mkdir.cc
        mkfs.cc
        ProcessContext.cc
        ProcessContext.h
        Stat.cc
        Stat.h
        SuperBlock.cc
        SuperBlock.h
        tee.cc ln.cc)

add_executable(5103P3 ${SOURCE_FILES} ln.cc)