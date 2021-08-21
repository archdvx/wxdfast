/***************************************************************
 * Name:      Defs.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef DEFS_H
#define DEFS_H

#include <wx/defs.h>

#define MANUAL                     0
#define NOW                        1
#define SCHEDULE                   2

#define MAX_SCHEDULE_EXCEPTIONS    9

#define STATUS_STOPED              0
#define STATUS_ACTIVE              1
#define STATUS_FINISHED            2
#define STATUS_ERROR               3
#define STATUS_QUEUE               4
#define STATUS_SCHEDULE_QUEUE      5
#define STATUS_SCHEDULE_ACTIVE     6

#define IPC_SERVICE                24242
#define MAX_BUFF_LEN               1024

enum
{
    ID_MENU_HIDE = wxID_HIGHEST+1,
    ID_MENU_NEW,
    ID_MENU_OFF,
    ID_MENU_ON,
    ID_MENU_PERDOWNLOAD,
    ID_MENU_REMOVE,
    ID_MENU_SCHEDULE,
    ID_MENU_START,
    ID_MENU_STOP,
    ID_MENU_STARTALL,
    ID_MENU_STOPALL,
    ID_MENU_UP,
    ID_MENU_DOWN,
    ID_MENU_INFO,
    ID_MENU_PASTEURL,
    ID_MENU_COPYURL,
    ID_MENU_COPYDOWNLOADDATA,
    ID_MENU_REMOVEALL,
    ID_MENU_FIND,
    ID_MENU_SHOWGRAPH,
    ID_MENU_DETAILS,
    ID_MENU_SITE,
    ID_MENU_BUG,
    ID_MENU_DONATE,
    ID_MENU_MOVE,
    ID_MENU_MD5,
    ID_MENU_OPENDESTINATION,
    ID_MENU_AGAIN,
    ID_MENU_EXPORT,
    ID_MENU_IMPORT,
    ID_MENU_SHUTDOWN,
    ID_MENU_DISCONNECT,
    ID_NOTEBOOK,
    ID_LIST_PROGRESS,
    ID_LIST_FINISHED,
    ID_OPTIONS_DESTINATION,
    ID_OPTIONS_FILEMANAGERPATH,
    ID_DATE_SELECT,
    ID_DATE_CALENDAR,
    ID_TIME_SELECT,
    ID_TIME_HOURS,
    ID_TIME_MINUTES,
    ID_TIME_SECONDS,
    ID_OPTIONS_STARTDATE,
    ID_OPTIONS_STARTTIME,
    ID_OPTIONS_FINISHDATE,
    ID_OPTIONS_FINISHTIME,
    ID_OPTIONS_EXCEPTION_ADD,
    ID_OPTIONS_EXCEPTION_REMOVE,
    ID_OPTIONS_NOTIFY,
    ID_ENGINE_GLOBAL,
    ID_ENGINE_DOWNLOAD,
    ID_ENGINE_VERSION,
    ID_ENGINE_REMOVED,
    ID_ENGINE_STARTED,
    ID_NEW_DIRECTORY,
    ID_NEW_ADD,
    ID_NEW_EDIT,
    ID_IPC,
    ID_IPC_SOCKET,
    ID_BATCH_DIRECTORY
};

#endif // DEFS_H
