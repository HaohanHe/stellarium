INCLUDE_DIRECTORIES(
     .
     gui
     ${CMAKE_BINARY_DIR}
     ${CMAKE_BINARY_DIR}/plugins/MiMoAI/src
     ${CMAKE_BINARY_DIR}/plugins/MiMoAI/src/gui
)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

SET(MiMoAI_SRCS
     MiMoAI.hpp
     MiMoAI.cpp
     MiMoAPIClient.hpp
     MiMoAPIClient.cpp
     MiMoTool.hpp
     MiMoTool.cpp
     MiMoToolManager.hpp
     MiMoToolManager.cpp
     MiMoTools.hpp
     MiMoTools.cpp
)

IF (STELLARIUM_GUI_MODE STREQUAL "Standard")
LIST(APPEND MiMoAI_SRCS
     gui/MiMoConfigDialog.hpp
     gui/MiMoConfigDialog.cpp
     gui/MiMoChatDialog.hpp
     gui/MiMoChatDialog.cpp
)

SET(MiMoAI_UIS
     gui/MiMoConfigDialog.ui
     gui/MiMoChatDialog.ui
)
ENDIF (STELLARIUM_GUI_MODE STREQUAL "Standard")

################# compiles resources files ############
SET(MiMoAI_RES ../MiMoAI.qrc)
IF (${QT_VERSION_MAJOR} EQUAL "5")
     IF (STELLARIUM_GUI_MODE STREQUAL "Standard")
          QT5_WRAP_UI(MiMoAI_UIS_H ${MiMoAI_UIS})
     ENDIF()
     QT5_ADD_RESOURCES(MiMoAI_RES_CXX ${MiMoAI_RES})
ELSE()
     IF (STELLARIUM_GUI_MODE STREQUAL "Standard")
          QT_WRAP_UI(MiMoAI_UIS_H ${MiMoAI_UIS})
     ENDIF()
     QT_ADD_RESOURCES(MiMoAI_RES_CXX ${MiMoAI_RES})
ENDIF()

ADD_LIBRARY(MiMoAI-static STATIC ${MiMoAI_SRCS} ${MiMoAI_RES_CXX} ${MiMoAI_UIS_H})
TARGET_LINK_LIBRARIES(MiMoAI-static Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::Network)
# The library target "MiMoAI-static" has a default OUTPUT_NAME of "MiMoAI", so change it.
SET_TARGET_PROPERTIES(MiMoAI-static PROPERTIES OUTPUT_NAME "MiMoAI")
SET_TARGET_PROPERTIES(MiMoAI-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
ADD_DEPENDENCIES(AllStaticPlugins MiMoAI-static)

SET_TARGET_PROPERTIES(MiMoAI-static PROPERTIES FOLDER "plugins/MiMoAI")
