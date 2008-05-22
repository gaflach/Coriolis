# - Find the Hurricane includes and libraries.
# The following variables are set if Hurricane is found.  If HURRICANE is not
# found, HURRICANE_FOUND is set to false.
#  HURRICANE_FOUND        - True when the Hurricane include directory is found.
#  HURRICANE_INCLUDE_DIR - the path to where the Hurricane include files are.
#  HURRICANE_LIBRARIES - The path to where the Hurricane library files are.


SET(HURRICANE_INCLUDE_PATH_DESCRIPTION "directory containing the Hurricane include files. E.g /usr/local/include or /asim/coriolis/include")

SET(HURRICANE_DIR_MESSAGE "Set the HURRICANE_INCLUDE_DIR cmake cache entry to the ${HURRICANE_INCLUDE_PATH_DESCRIPTION}")

# don't even bother under WIN32
IF(UNIX)

  SET(HURRICANE_DIR_SEARCH $ENV{HURRICANE_TOP})
  IF(HURRICANE_DIR_SEARCH)
    FILE(TO_CMAKE_PATH ${HURRICANE_DIR_SEARCH} HURRICANE_DIR_SEARCH)
  ENDIF(HURRICANE_DIR_SEARCH)

  #
  # Look for an installation.
  #
  FIND_PATH(HURRICANE_INCLUDE_PATH NAMES hurricane/Cell.h PATHS
    # Look in other places.
    ${HURRICANE_DIR_SEARCH}
    PATH_SUFFIXES include
    # Help the user find it if we cannot.
    DOC "The ${HURRICANE_INCLUDE_PATH_DESCRIPTION}"
  )

  FIND_LIBRARY(HURRICANE_LIBRARY_PATH
    NAMES hurricane
    PATHS ${HURRICANE_DIR_SEARCH}
    PATH_SUFFIXES lib
    # Help the user find it if we cannot.
    DOC "The ${HURRICANE_INCLUDE_PATH_DESCRIPTION}"
  )

  FIND_PATH(HURRICANE_VIEWER_INCLUDE_PATH
    NAMES hurricane/viewer/CellViewer.h hurricane/inspector/HInspectorWidget.h
    PATHS ${HURRICANE_DIR_SEARCH}
    PATH_SUFFIXES include
    # Help the user find it if we cannot.
    DOC "The ${HURRICANE_INCLUDE_PATH_DESCRIPTION}"
  )

  FIND_LIBRARY(HURRICANE_VIEWER_LIBRARY_PATH
    NAMES hviewer 
    PATHS ${HURRICANE_DIR_SEARCH}
    PATH_SUFFIXES lib
    # Help the user find it if we cannot.
    DOC "The ${HURRICANE_INCLUDE_PATH_DESCRIPTION}"
  )

  FIND_LIBRARY(HURRICANE_INSPECTOR_LIBRARY_PATH
    NAMES hinspector 
    PATHS ${HURRICANE_DIR_SEARCH}
    PATH_SUFFIXES lib
    # Help the user find it if we cannot.
    DOC "The ${HURRICANE_INCLUDE_PATH_DESCRIPTION}"
  )

  # Assume we didn't find it.
  SET(HURRICANE_FOUND 0)
  SET(HURRICANE_GRAPHICAL_FOUND 0)

  IF(HURRICANE_INCLUDE_PATH)
  IF(HURRICANE_LIBRARY_PATH)
      SET(HURRICANE_FOUND "YES")
      SET(HURRICANE_INCLUDE_DIR
        ${HURRICANE_INCLUDE_PATH}
      )
      SET(HURRICANE_LIBRARIES
        ${HURRICANE_LIBRARY_PATH}
      )
  ENDIF(HURRICANE_LIBRARY_PATH)
  ENDIF(HURRICANE_INCLUDE_PATH)

  IF(HURRICANE_VIEWER_INCLUDE_PATH)
  IF(HURRICANE_VIEWER_LIBRARY_PATH)
    SET(HURRICANE_GRAPHICAL_FOUND "YES")
    SET(HURRICANE_GRAPHICAL_INCLUDE_DIR
      ${HURRICANE_VIEWER_INCLUDE_PATH}
    )
    SET(HURRICANE_GRAPHICAL_LIBRARIES
      ${HURRICANE_VIEWER_LIBRARY_PATH}
    )
  ENDIF(HURRICANE_VIEWER_LIBRARY_PATH)
  ENDIF(HURRICANE_VIEWER_INCLUDE_PATH)

  IF (HURRICANE_INSPECTOR_LIBRARY_PATH)
    SET(HURRICANE_GRAPHICAL_LIBRARIES ${HURRICANE_GRAPHICAL_LIBRARIES} ${HURRICANE_INSPECTOR_LIBRARY_PATH})
  ENDIF(HURRICANE_INSPECTOR_LIBRARY_PATH)

  IF(HURRICANE_FOUND)
    IF(NOT HURRICANE_FIND_QUIETLY)
      MESSAGE(STATUS "Found HURRICANE : ${HURRICANE_LIBRARIES}")
    ENDIF(NOT HURRICANE_FIND_QUIETLY)
  ELSE(HURRICANE_FOUND)
    IF(HURRICANE_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "HURRICANE was not found. ${HURRICANE_DIR_MESSAGE}")
    ENDIF(HURRICANE_FIND_REQUIRED)
  ENDIF(HURRICANE_FOUND)

  MARK_AS_ADVANCED(
    HURRICANE_INCLUDE_PATH
    HURRICANE_LIBRARY_PATH
    HURRICANE_GRAPHICAL_INCLUDE_PATH
    HURRICANE_VIEWER_LIBRARY_PATH
    HURRICANE_INSPECTOR_LIBRARY_PATH
  )

ENDIF(UNIX)
