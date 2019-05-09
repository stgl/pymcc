#  This CMake file defines variables with version and release information.
#  Input variables:
#
#    VERSION = #.# (major.minor)
#    RELEASE = "alpha #" or "beta #" or "candidate #" or "final"
#
#  Variables defined:
#
#    VERSION_MAJOR = major version number
#    VERSION_MINOR = minor version number
# 
#    RELEASE_TYPE   = "alpha" or "beta" or "candidate" or "final"
#    RELEASE_NUMBER = release number (if type is not "final")
#    RELEASE_NAME   =  "alpha 1 release"
#                   or "beta # release"
#                   or "release candidate #"
#                   or "final release"
#    RELEASE_ABBR   = "a#", "b#", "rc#", "" (final)
#
#    VERSION_RELEASE =  "${VERSION}" if final
#                    or "${VERSION} (${RELEASE_NAME})" otherwise
#    VERSION_RELEASE_ABBR = ${VERSION}${RELEASE_ABBR}

if (NOT VERSION MATCHES "^[0-9]+[.][0-9]+$")
  message (FATAL_ERROR "Invalid version: '${VERSION}';  valid format: {integer}.{integer}")
endif()

string (REGEX MATCH "^[0-9]+" VERSION_MAJOR ${VERSION})
string (REGEX MATCH "[0-9]+$" VERSION_MINOR ${VERSION})

if (RELEASE STREQUAL "final")
  set (RELEASE_TYPE "final")
  set (RELEASE_NAME "final release")
  set (RELEASE_ABBR "")
elseif (RELEASE MATCHES "^(alpha|beta|candidate) [0-9]+$")
  string (REGEX MATCH "^[a-z]+" RELEASE_TYPE "${RELEASE}")
  string (REGEX MATCH "[0-9]+$" RELEASE_NUMBER "${RELEASE}")
  if (RELEASE_TYPE STREQUAL "candidate")
  	set (RELEASE_NAME "release candidate ${RELEASE_NUMBER}")
  	set (RELEASE_ABBR "rc${RELEASE_NUMBER}")
  else()
  	set (RELEASE_NAME "${RELEASE_TYPE} ${RELEASE_NUMBER} release")
  	if (RELEASE_TYPE STREQUAL "alpha")
  	  set (RELEASE_ABBR "a${RELEASE_NUMBER}")
	else()
  	  set (RELEASE_ABBR "b${RELEASE_NUMBER}")
	endif()
  endif()
else()
  message (FATAL_ERROR "Invalid release: '${RELEASE}';"
                       " valid formats: alpha #, beta #, candidate #, or final (where # = integer)")
endif()

if (RELEASE_TYPE STREQUAL "final")
  set (VERSION_RELEASE "${VERSION}")
else()
  set (VERSION_RELEASE "${VERSION} (${RELEASE_NAME})")
endif()
set (VERSION_RELEASE_ABBR "${VERSION}${RELEASE_ABBR}")
