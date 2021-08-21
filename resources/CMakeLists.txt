CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

if(NOT WIN32 AND NOT APPLE)
install(FILES wxdfast.desktop DESTINATION share/applications)
endif(NOT WIN32 AND NOT APPLE)
install(FILES RipStop/icon/wxdfast.png DESTINATION share/icons/hicolor/48x48/apps)
install(FILES RipStop/icon/wxdfast.svg DESTINATION share/icons/hicolor/scalable/apps)
install(FILES RipStop/icon/wxdfast.png DESTINATION share/wxdfast/icons)
install(FILES extensionid DESTINATION share/wxdfast)
install(DIRECTORY RipStop/logo/ DESTINATION share/wxdfast/icons/logo FILES_MATCHING PATTERN "*.png")
install(DIRECTORY RipStop/menubar/ DESTINATION share/wxdfast/icons/menubar FILES_MATCHING PATTERN "*.png")
install(DIRECTORY RipStop/toolbar/ DESTINATION share/wxdfast/icons/toolbar FILES_MATCHING PATTERN "*.png")



