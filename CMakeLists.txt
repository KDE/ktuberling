cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "08")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ktuberling VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

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

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Multimedia
    PrintSupport
    Svg
    SvgWidgets
    Widgets
    Xml
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
)

if(NOT ANDROID)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
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

    find_package(KDEGames6 6.0.0 REQUIRED)
endif()

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KTUBERLING VERSION_HEADER ktuberling_version.h)

ecm_set_disabled_deprecation_versions(
    QT 6.9.0
    KF 6.15.0
)
if (NOT ANDROID)
    ecm_set_disabled_deprecation_versions(
        KDEGAMES 6.0
    )
endif()

if(KF6DocTools_FOUND)
    add_subdirectory(doc)
endif()

########### next target ###############

set(ktuberling_common_SRCS
   action.cpp action.h
   playground.cpp playground.h
   todraw.cpp todraw.h
   soundfactory.cpp soundfactory.h
   filefactory.cpp filefactory.h
)
ecm_qt_declare_logging_category(ktuberling_common_SRCS
    HEADER ktuberling_debug.h
    IDENTIFIER KTUBERLING_LOG
    CATEGORY_NAME ktuberling
    OLD_CATEGORY_NAMES org.kde.kdegames.ktuberling
    DESCRIPTION "KTuberling game"
    EXPORT KTUBERLING
)

if(ANDROID)
    set(ktuberling_mobile_SRCS
        ${ktuberling_common_SRCS}
        main_mobile.cpp
        android_data/resources.qrc
    )

    add_executable(ktuberling_mobile ${ktuberling_mobile_SRCS})

    target_link_libraries(ktuberling_mobile
        KF6::ConfigCore
        Qt6::Gui
        Qt6::Svg
        Qt6::Multimedia
        Qt6::Xml
        Qt6::Widgets
        Qt6::SvgWidgets
    )

    install(TARGETS ktuberling_mobile RUNTIME DESTINATION bin)

else()

    set(ktuberling_SRCS
        ${ktuberling_common_SRCS}
        main.cpp
        toplevel.cpp toplevel.h
        playgrounddelegate.cpp playgrounddelegate.h

        ktuberling.qrc
    )

    file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-ktuberling.png")
    ecm_add_app_icon(ktuberling_SRCS ICONS ${ICONS_SRCS})

    add_executable(ktuberling ${ktuberling_SRCS})

    target_link_libraries(ktuberling
        KDEGames6
        KF6::ConfigWidgets
        KF6::Completion
        KF6::Crash
        KF6::DBusAddons
        KF6::I18n
        KF6::KIOCore
        KF6::XmlGui
        Qt6::PrintSupport
        Qt6::Svg
        Qt6::Multimedia
        Qt6::SvgWidgets
    )

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
if(KF6DocTools_FOUND)
    kdoctools_install(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
