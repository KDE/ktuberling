project(ktuberling)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.3.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 REQUIRED COMPONENTS 
    CoreAddons
    Config
    ItemModels
    WidgetsAddons
    WindowSystem
    Config
    DBusAddons
    I18n
    ConfigWidgets
    TextWidgets
    XmlGui
    KIO
    NotifyConfig
    NewStuff
    KDELibs4Support)

find_package(KF5KDEGames 4.9.0 REQUIRED)
find_package(Phonon4Qt5 CONFIG REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DTRANSLATION_DOMAIN="ktuberling")

add_subdirectory( sounds ) 
add_subdirectory( pics ) 
add_subdirectory( doc )

########### next target ###############

set(ktuberling_SRCS 
   action.cpp 
   main.cpp 
   toplevel.cpp 
   playground.cpp 
   todraw.cpp 
   soundfactory.cpp 
   playgrounddelegate.cpp )

kde4_add_app_icon(ktuberling_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-ktuberling.png")
add_executable(ktuberling ${ktuberling_SRCS})

target_link_libraries(ktuberling   KF5::KIOCore KF5KDEGames Phonon::phonon4qt5 Qt5::Svg KF5::KDELibs4Support Phonon::phonon4qt5)

install(TARGETS ktuberling  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS ktuberling.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES ktuberlingui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/ktuberling )

ecm_install_icons( ${ICON_INSTALL_DIR}   )





