file(GLOB hayai_headers
  hayai.hpp
  hayai_benchmarker.hpp
  hayai_clock.hpp
  hayai_compatibility.hpp
  hayai_console.hpp
  hayai_console_outputter.hpp
  hayai_default_test_factory.hpp
  hayai_fixture.hpp
  hayai_json_outputter.hpp
  hayai_junit_xml_outputter.hpp
  hayai_outputter.hpp
  hayai_test.hpp
  hayai_test_descriptor.hpp
  hayai_test_factory.hpp
  hayai_test_result.hpp
  hayai_main.hpp
)

add_library(hayai_main
  hayai_posix_main.cpp
)

set_target_properties(hayai_main PROPERTIES
  PUBLIC_HEADER "${hayai_headers}"
)

if(MSVC)
    add_definitions("-D_UNICODE")
    set(CMAKE_CXX_FLAGS "/W4 /source-charset:utf-8 /MT /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "/MP8 /MTd /ZI /Od")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
endif()

# Install the targets if it is asked
if (${INSTALL_HAYAI})
  install(
    TARGETS hayai_main
    EXPORT hayai-targets
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT bin
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/hayai"
    COMPONENT dev
  )
endif (${INSTALL_HAYAI})
