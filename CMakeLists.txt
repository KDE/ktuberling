cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "23")
set(RELEASE_SERVICE_VERSION_MINOR "07")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ktuberling VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.91.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS PrintSupport Svg Widgets Xml Multimedia)
find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS Config I18n)

if (QT_MAJOR_VERSION EQUAL "6")
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS SvgWidgets)
endif()

if(NOT ANDROID)
    find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
        Completion
        ConfigWidgets
        CoreAddons
        Crash
        DBusAddons
        KIO
        DocTools
        WidgetsAddons
        XmlGui
    )

    if (QT_MAJOR_VERSION STREQUAL "6")
        find_package(KDEGames6 7.5.0 REQUIRED)
    else()
        find_package(KF5KDEGames 7.3.0 REQUIRED)
    endif()
endif()

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KTUBERLING VERSION_HEADER ktuberling_version.h)

# Added by KDE_COMPILERSETTINGS_LEVEL 5.85
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
)

ecm_set_disabled_deprecation_versions(
    QT 6.4
    KF 5.103
)

if(KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

########### next target ###############

set(ktuberling_common_SRCS
   action.cpp
   playground.cpp
   todraw.cpp
   soundfactory.cpp
   filefactory.cpp
)
ecm_qt_declare_logging_category(ktuberling_common_SRCS HEADER ktuberling_debug.h IDENTIFIER KTUBERLING_LOG CATEGORY_NAME org.kde.kdegames.ktuberling DESCRIPTION "ktuberling (kdegames)" EXPORT KTUBERLING)

if(ANDROID)
    set(ktuberling_mobile_SRCS
        ${ktuberling_common_SRCS}
        main_mobile.cpp
        android_data/resources.qrc
    )

    add_executable(ktuberling_mobile ${ktuberling_mobile_SRCS})

    target_link_libraries(ktuberling_mobile
        Qt${QT_MAJOR_VERSION}::Gui
        Qt${QT_MAJOR_VERSION}::Svg
        Qt${QT_MAJOR_VERSION}::Multimedia
        Qt${QT_MAJOR_VERSION}::Xml
        Qt${QT_MAJOR_VERSION}::Widgets
        KF${KF_MAJOR_VERSION}::ConfigCore )

    if (QT_MAJOR_VERSION EQUAL "6")
        target_link_libraries(ktuberling_mobile Qt6::SvgWidgets)
    endif()

    install(TARGETS ktuberling_mobile RUNTIME DESTINATION bin)

else()

    set(ktuberling_SRCS
        ${ktuberling_common_SRCS}
        main.cpp
        toplevel.cpp
        playgrounddelegate.cpp

        ktuberling.qrc
    )

    file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-ktuberling.png")
    ecm_add_app_icon(ktuberling_SRCS ICONS ${ICONS_SRCS})

    add_executable(ktuberling ${ktuberling_SRCS})

    if (QT_MAJOR_VERSION STREQUAL "6")
        target_link_libraries(ktuberling KDEGames6)
    else()
        target_link_libraries(ktuberling KF5KDEGames)
    endif()

    target_link_libraries(ktuberling
        Qt${QT_MAJOR_VERSION}::PrintSupport
        Qt${QT_MAJOR_VERSION}::Svg
        Qt${QT_MAJOR_VERSION}::Multimedia
        KF${KF_MAJOR_VERSION}::ConfigWidgets
        KF${KF_MAJOR_VERSION}::Completion
        KF${KF_MAJOR_VERSION}::Crash
        KF${KF_MAJOR_VERSION}::DBusAddons
        KF${KF_MAJOR_VERSION}::KIOCore
        KF${KF_MAJOR_VERSION}::XmlGui
    )

    if (QT_MAJOR_VERSION EQUAL "6")
        target_link_libraries(ktuberling Qt6::SvgWidgets)
    endif()

    install(TARGETS ktuberling  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

    install(PROGRAMS org.kde.ktuberling.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})

    ecm_install_icons(ICONS
        128-apps-ktuberling.png
        16-apps-ktuberling.png
        22-apps-ktuberling.png
        32-apps-ktuberling.png
        48-apps-ktuberling.png
        64-apps-ktuberling.png
        128-mimetypes-application-x-tuberling.png
        16-mimetypes-application-x-tuberling.png
        22-mimetypes-application-x-tuberling.png
        32-mimetypes-application-x-tuberling.png
        48-mimetypes-application-x-tuberling.png
        64-mimetypes-application-x-tuberling.png
        DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor
    )

    add_subdirectory(sounds)
    add_subdirectory(pics)
endif()

install(FILES org.kde.ktuberling.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
ecm_qt_install_logging_categories(EXPORT KTUBERLING FILE ktuberling.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

ki18n_install(po)
if(KF5DocTools_FOUND)
    kdoctools_install(po)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
