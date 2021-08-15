CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

foreach(lang ${LANGS})
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${lang}.mo" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/locale/${lang}/LC_MESSAGES" RENAME "wxdfast.mo")
endforeach(lang)

