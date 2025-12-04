function(copy_folder_if_changed SRC_DIR DST_DIR TARGET_NAME)
    # Normalize paths
    get_filename_component(SRC_DIR "${SRC_DIR}" ABSOLUTE)
    get_filename_component(DST_DIR "${DST_DIR}" ABSOLUTE)

    # Locations inside build folder
    set(_LIST_FILE "${CMAKE_BINARY_DIR}/${TARGET_NAME}_filelist.txt")
    set(_NEW_LIST  "${_LIST_FILE}.new")
    set(_SCRIPT    "${CMAKE_BINARY_DIR}/${TARGET_NAME}_gen_and_copy.cmake")

    # Create the script executed at build time
    file(WRITE "${_SCRIPT}"
"cmake_minimum_required(VERSION 3.15)\n"
"set(SRC \"${SRC_DIR}\")\n"
"set(DST \"${DST_DIR}\")\n"
"set(LIST \"${_LIST_FILE}\")\n"
"set(NEW_LIST \"${_NEW_LIST}\")\n"
"\n"
"# Gather files and compute new fingerprint\n"
"file(GLOB_RECURSE files \"${SRC_DIR}/*\")\n"
"file(WRITE \"${_NEW_LIST}\" \"\")\n"
"foreach(f IN LISTS files)\n"
"  file(TIMESTAMP \"\${f}\" ts UTC)\n"
"  file(SIZE \"\${f}\" sz)\n"
"  file(APPEND \"${_NEW_LIST}\" \"\${ts} \${sz} \${f}\\n\")\n"
"endforeach()\n"
"\n"
"# Determine if changed\n"
"set(changed TRUE)\n"
"if(EXISTS \"${_LIST_FILE}\")\n"
"  file(READ \"${_LIST_FILE}\" old)\n"
"  file(READ \"${_NEW_LIST}\" new)\n"
"  if(\"\${old}\" STREQUAL \"\${new}\")\n"
"    set(changed FALSE)\n"
"  endif()\n"
"endif()\n"
"\n"
"if(changed)\n"
"  message(STATUS \"[${TARGET_NAME}] folder changed, copying...\")\n"
"  file(MAKE_DIRECTORY \"${DST_DIR}\")\n"
"  file(COPY \"${SRC_DIR}\" DESTINATION \"${DST_DIR}\")\n"
"  file(WRITE \"${_LIST_FILE}\" \"\")\n"
"  file(READ \"${_NEW_LIST}\" new2)\n"
"  file(APPEND \"${_LIST_FILE}\" \"\${new2}\")\n"
"else()\n"
"  message(STATUS \"[${TARGET_NAME}] no changes, skipping copy\")\n"
"endif()\n"
"\n"
"file(REMOVE \"${_NEW_LIST}\")\n"
)

    # Actual build-time target
    add_custom_target(${TARGET_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -P "${_SCRIPT}"
        BYPRODUCTS "${_LIST_FILE}"
        COMMENT "[${TARGET_NAME}] Checking folder changes"
        VERBATIM
    )

endfunction()