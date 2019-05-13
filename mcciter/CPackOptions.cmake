if (CPACK_GENERATOR MATCHES "NSIS")
  set (CPACK_NSIS_DISPLAY_NAME    "MCC-LIDAR 2.1")
  set (CPACK_NSIS_MENU_LINKS      "doc/Documentation.txt"  "Documentation"
                                  "License.txt"            "License"
                                  "Notice.txt"             "Notice")

  #  workaround per http://public.kitware.com/Bug/view.php?id=7828
  set (CPACK_PACKAGE_EXECUTABLES  "" "")

  #  For the next two variables to work with CPack and NSIS, the
  #  PROJECT_SOURCE_DIR variable must have no spaces, and the final path
  #  separator must be the Windows backslash.
  set (CPACK_NSIS_CREATE_ICONS_EXTRA   "!include /homedirs/hilley/pymcc/mcciter/package/win\\create_shortcut.nsh")
  set (CPACK_NSIS_DELETE_ICONS_EXTRA   "!include /homedirs/hilley/pymcc/mcciter/package/win\\delete_shortcut.nsh")
endif()
