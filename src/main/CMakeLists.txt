# build sketch as executable  
add_executable (nb-decoder nb-decoder.c )
target_link_libraries(nb-decoder codec-amr)

add_executable (nb-encoder nb-encoder.c )
target_link_libraries(nb-encoder codec-amr m)

add_executable (wb-decoder wb-decoder.c )
target_link_libraries(wb-decoder codec-amr)

add_executable (wb-encoder wb-encoder.c )
target_link_libraries(wb-encoder codec-amr m)