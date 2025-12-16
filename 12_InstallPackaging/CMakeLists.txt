cmake_minimum_required(VERSION 3.16)
project(Greeter LANGUAGES C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(TARGET Intl::Intl)
    set(GREETER_INTL_TARGET Intl::Intl)
else()
    set(GREETER_INTL_TARGET ${Intl_LIBRARIES})
endif()

add_library(greeter SHARED src/greeter.c)
target_include_directories(greeter PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(greeter PUBLIC ${GREETER_INTL_TARGET})
target_compile_definitions(greeter PRIVATE
    BUILD_LOCALE_DIR="${CMAKE_CURRENT_BINARY_DIR}/locale"
    INSTALL_LOCALE_DIR="${CMAKE_INSTALL_FULL_LOCALEDIR}")

add_executable(greet_tool src/main.c)
target_link_libraries(greet_tool PRIVATE greeter ${GREETER_INTL_TARGET})

# Translations
set(LOCALES en ru)
set(PO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/po)
set(LOCALE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/locale)

foreach(lang ${LOCALES})
    set(po_file ${PO_DIR}/${lang}.po)
    set(mo_file ${LOCALE_OUTPUT}/${lang}/LC_MESSAGES/greeter.mo)
    add_custom_command(
        OUTPUT ${mo_file}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${LOCALE_OUTPUT}/${lang}/LC_MESSAGES
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${mo_file} ${po_file}
        DEPENDS ${po_file}
        COMMENT "Compiling ${lang} translation"
    )
    list(APPEND MO_FILES ${mo_file})
endforeach()

add_custom_target(translations ALL DEPENDS ${MO_FILES})
add_dependencies(greet_tool translations)
add_dependencies(greeter translations)

# Documentation extraction
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docs/generated.md
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/scripts
            ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_docs.py
            ${CMAKE_CURRENT_SOURCE_DIR}/include/greeter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/project.md
            ${CMAKE_CURRENT_BINARY_DIR}/docs/generated.md
    DEPENDS include/greeter.h docs/project.md scripts/extract_docs.py
    COMMENT "Generating documentation from headers"
)

add_custom_target(docs ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docs/generated.md)

# Tests
include(CTest)
if(BUILD_TESTING)
    add_executable(test_greeter tests/test_greeter.c)
    target_link_libraries(test_greeter PRIVATE greeter ${GREETER_INTL_TARGET})
    add_test(NAME greeter_basic COMMAND test_greeter)

    add_test(NAME greet_tool_en
             COMMAND ${CMAKE_COMMAND} -E env LANG=en_US.UTF-8
                     ${CMAKE_CURRENT_BINARY_DIR}/greet_tool --name CMake)
    add_test(NAME greet_tool_ru
             COMMAND ${CMAKE_COMMAND} -E env LANG=ru_RU.UTF-8 LANGUAGE=ru GREETER_LOCALE_DIR=${LOCALE_OUTPUT}
                     ${CMAKE_CURRENT_BINARY_DIR}/greet_tool --name CMake)

    add_test(NAME greet_tool_ru_output
             COMMAND /bin/sh -c "GREETER_LOCALE_DIR='${LOCALE_OUTPUT}' LANG=ru_RU.UTF-8 LANGUAGE=ru '${CMAKE_CURRENT_BINARY_DIR}/greet_tool' --name CMake | grep -q 'Здравствуйте, CMake!'"
    )
endif()

# Install rules
install(TARGETS greeter greet_tool
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES include/greeter.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

foreach(lang ${LOCALES})
    install(FILES ${LOCALE_OUTPUT}/${lang}/LC_MESSAGES/greeter.mo
            DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${lang}/LC_MESSAGES)
endforeach()

install(FILES man/greet_tool.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(DIRECTORY docs/ DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/docs/generated.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake
                   IMMEDIATE @ONLY)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake)
endif()
