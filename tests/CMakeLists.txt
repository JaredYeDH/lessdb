
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_subdirectory(lib/googletest/googletest)

add_executable(Status_unittest
        Status_unittest.cc
        ../src/Status.cc)
target_link_libraries(Status_unittest gtest gtest_main)

add_executable(SkipList_unittest
        SkipList_unittest.cc)
target_link_libraries(SkipList_unittest
        gtest gtest_main ${Boost_LIBRARIES} ${FOLLY_LIBRARIES})

add_executable(InternalKey_unittest
        InternalKey_unittest.cc
        ../src/InternalKey.cc
        ../src/Comparator.cc)
target_link_libraries(InternalKey_unittest gtest gtest_main)

add_executable(WriteBatch_unittest
        WriteBatch_unittest.cc
        ../src/WriteBatch.cc
        ../src/WriteBatchImpl.h
        ../src/Status.cc
        ../src/MemTable.cc
        ../src/InternalKey.cc)
target_link_libraries(WriteBatch_unittest gtest gtest_main ${FOLLY_LIBRARIES})

add_executable(BlockBuilder_unittest
        BlockBuilder_unittest.cc
        ../src/Block.cc
        ../src/Options.cc
        ../src/Comparator.cc
        ../src/Status.cc)
target_link_libraries(BlockBuilder_unittest gtest gtest_main)

add_executable(MemTable_unittest
        MemTable_unittest.cc
        ../src/MemTable.cc
        ../src/InternalKey.cc
        ../src/Comparator.cc
        ../src/WriteBatch.cc
        ../src/Status.cc)
target_link_libraries(MemTable_unittest gtest gtest_main ${FOLLY_LIBRARIES})

add_executable(PosixFiles_unittest
        PosixFiles_unittest.cc
        ../src/FileUtils.cc
        ../src/Status.cc)
target_link_libraries(PosixFiles_unittest gtest gtest_main ${Boost_LIBRARIES})

add_executable(Cache_unittest
        CacheStrategy_unittest.cc
        ../src/CacheStrategy.cc)
target_link_libraries(Cache_unittest gtest gtest_main)

add_executable(FilterStrategy_unittest
        FilterStrategy_unittest.cc
        ../src/FilterStrategy.cc)
target_link_libraries(FilterStrategy_unittest gtest gtest_main)

add_executable(SSTable_unittest
        SSTable_unittest.cc
        ../src/FileUtils.cc
        ../src/Options.cc
        ../src/Comparator.cc
        ../src/Status.cc
        ../src/TableFormat.cc
        ../src/SSTable.cc
        ../src/Block.cc)
target_link_libraries(SSTable_unittest gtest gtest_main ${SILLY_LIBRARY}
        ${Boost_LIBRARIES} ${GLOG_LIBRARY})
