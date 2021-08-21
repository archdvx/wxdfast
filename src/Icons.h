/***************************************************************
 * Name:      Icons.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#ifndef ICONS_H
#define ICONS_H

wxIcon makeIcon(const wxString &path, const wxString &name);
wxIcon makeGreyedIcon(const wxString &path, const wxString &name);
wxIcon makeScaledIcon(const wxString& path, int max=16);
void makeAllIcons(const wxString &path);

extern wxIcon ICO_ICON;
extern wxIcon ICO_TRAY;
extern wxIcon ICO_LOGO;
extern wxIcon ICO_ABOUT;
extern wxIcon ICO_COMPLETED;
extern wxIcon ICO_COPYDATA;
extern wxIcon ICO_COPYURL;
extern wxIcon ICO_DETAILS;
extern wxIcon ICO_DOWNLOADING;
extern wxIcon ICO_ERROR;
extern wxIcon ICO_FIND;
extern wxIcon ICO_GRAPH;
extern wxIcon ICO_HELP;
extern wxIcon ICO_NEW;
extern wxIcon ICO_OPTIONS;
extern wxIcon ICO_PASTEURL;
extern wxIcon ICO_PAUSE;
extern wxIcon ICO_PROGRESSBAR;
extern wxIcon ICO_PROPERTIES;
extern wxIcon ICO_QUIT;
extern wxIcon ICO_REMOVE;
extern wxIcon ICO_SCHEDULE;
extern wxIcon ICO_SCHEDULED;
extern wxIcon ICO_START;
extern wxIcon ICO_STARTALL;
extern wxIcon ICO_STOP;
extern wxIcon ICO_STOPALL;
extern wxIcon ICO_DOWNLOAD_INFO;
extern wxIcon ICO_DOWNLOAD_MOVE_DOWN;
extern wxIcon ICO_DOWNLOAD_MOVE_UP;
extern wxIcon ICO_DOWNLOAD_NEW;
extern wxIcon ICO_DOWNLOAD_REMOVE;
extern wxIcon ICO_DOWNLOAD_SCHEDULE;
extern wxIcon ICO_DOWNLOAD_START;
extern wxIcon ICO_DOWNLOAD_START_ALL;
extern wxIcon ICO_DOWNLOAD_STOP;
extern wxIcon ICO_DOWNLOAD_STOP_ALL;
extern wxIcon ICO_GDOWNLOAD_INFO;
extern wxIcon ICO_GDOWNLOAD_MOVE_DOWN;
extern wxIcon ICO_GDOWNLOAD_MOVE_UP;
extern wxIcon ICO_GDOWNLOAD_NEW;
extern wxIcon ICO_GDOWNLOAD_REMOVE;
extern wxIcon ICO_GDOWNLOAD_SCHEDULE;
extern wxIcon ICO_GDOWNLOAD_START;
extern wxIcon ICO_GDOWNLOAD_START_ALL;
extern wxIcon ICO_GDOWNLOAD_STOP;
extern wxIcon ICO_GDOWNLOAD_STOP_ALL;

#endif // ICONS_H
