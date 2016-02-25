
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_subdirectory(lib/googletest/googletest)

add_executable(Slice_unittest
        Slice_unittest.cc)
target_link_libraries(Slice_unittest gtest gtest_main)

add_executable(Status_unittest
        Status_unittest.cc
        ../src/Status.cc)
target_link_libraries(Status_unittest gtest gtest_main)

add_executable(SkipList_unittest
        SkipList_unittest.cc
        ../src/SkipList.h)
target_link_libraries(SkipList_unittest
        gtest gtest_main ${Boost_LIBRARIES} ${FOLLY_LIBRARIES})

add_executable(InternalKey_unittest
        InternalKey_unittest.cc
        ../src/InternalKey.cc)
target_link_libraries(InternalKey_unittest gtest gtest_main)

add_executable(WriteBatch_unittest
        WriteBatch_unittest.cc
        ../src/WriteBatch.cc
        ../src/WriteBatchImpl.h
        ../src/DataView.cc
        ../src/Status.cc
        ../src/Coding.cc)
target_link_libraries(WriteBatch_unittest gtest gtest_main)

add_executable(DataView_unittest
        DataView_unittest.cc
        ../src/DataView.cc)
target_link_libraries(DataView_unittest gtest gtest_main)

add_executable(BlockBuilder_unittest
        BlockBuilder_unittest.cc
        ../src/DataView.cc
        ../src/BlockBuilder.cc)
target_link_libraries(BlockBuilder_unittest gtest gtest_main)

add_executable(MemTable_unittest
        MemTable_unittest.cc
        ../src/MemTable.cc)
target_link_libraries(MemTable_unittest gtest gtest_main)

add_executable(Coding_unittest
        Coding_unittest.cc
        ../src/Coding.cc
        ../src/DataView.cc)
target_link_libraries(Coding_unittest gtest gtest_main)

add_executable(Comparator_unittest
        Comparator_unittest.cc
        ../src/Comparator.cc)
target_link_libraries(Comparator_unittest gtest gtest_main)
