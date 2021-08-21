/*
 * wxdfast-integrator.cxx
 *
 * Copyright 2021 David Vachulka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#if defined(_WIN32) || defined(_WIN64)
#define WIDLE 1
#define WIN32_LEAN_AND_MEAN
#define PATHSEPSTRING     "\\"
#define MAXPATHLEN        1024
#define ISPATHSEP(c)      ((c)=='/' || (c)=='\\')
#include <windows.h>
#include <shellapi.h>
#include <winsock2.h>
#include <sys/types.h>
#include <ws2tcpip.h>
#else
#define WIDLE 0
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pwd.h>
#endif
#include "config.h"
#include "../src/IpcDefs.h"

#define INTEGRATOR_VERSION "1.0.0"

int dpid = 0;
std::ofstream *logger = nullptr;

void logLine(const std::string &line)
{
#ifdef DEBUG
    if(logger)
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        *logger << std::put_time(&tm, "[%x %X] ") << line << std::endl;
    }
#endif
}

std::string currentUserName()
{
#if !WIDLE
    struct passwd *pwd= getpwuid(geteuid());
    if(pwd) return pwd->pw_name;
#else
    TCHAR buffer[MAXPATHLEN];
    DWORD size=MAXPATHLEN;
    if(GetUserName(buffer,&size)) return buffer;
#endif
    return "";
}

std::string getTempDirectory()
{
#if WIDLE
  TCHAR buffer[MAXPATHLEN];
  DWORD len=GetTempPath(MAXPATHLEN,buffer);
  if(1<len && ISPATHSEP(buffer[len-1]) && !ISPATHSEP(buffer[len-2])) len--;
  return std::string(buffer,len);
#else
  return "/tmp";
#endif
}

std::string getHomeDirectory()
{
#if !WIDLE
    struct passwd *pwd;
    const char* str;
    if((str=getenv("HOME"))!=NULL) return str;
    if((str=getenv("USER"))!=NULL || (str=getenv("LOGNAME"))!=NULL)
    {
        if((pwd=getpwnam(str))!=NULL) return pwd->pw_dir;
    }
    if((pwd=getpwuid(getuid()))!=NULL) return pwd->pw_dir;
    return "/";
#else
    const char *str1,*str2;
    char home[MAXPATHLEN];
    DWORD size=MAXPATHLEN;
    HKEY hKey;
    LONG result;
    if((str1=getenv("USERPROFILE"))!=NULL) return str1;
    if((str1=getenv("HOME"))!=NULL) return str1;
    if((str2=getenv("HOMEPATH"))!=NULL)
    {
        if((str1=getenv("HOMEDRIVE"))==NULL) str1="c:";
        strncpy(home,str1,MAXPATHLEN);
        strncat(home,str2,MAXPATHLEN);
        return home;
    }
    if(RegOpenKeyExA(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",0,KEY_READ,&hKey)==ERROR_SUCCESS)
    {
        result=RegQueryValueExA(hKey,"Personal",NULL,NULL,(LPBYTE)home,&size);
        RegCloseKey(hKey);
        if(result==ERROR_SUCCESS) return home;
    }
    return "c:" PATHSEPSTRING;
#endif
    return "";
}

bool wxdfastRunning()
{
#if WIDLE
    HWND existingApp = FindWindow(0, "wxDownload Fast");
    if(existingApp) return true;
    else return false;
#else
#if __APPLE__
    std::string path = getHomeDirectory()+std::string("/wxDownloadFast-")+currentUserName();
#else
    std::string path = getHomeDirectory()+std::string("/wxdfast-")+currentUserName();
#endif
    struct flock fl;
    int fd;
    fd = open(path.c_str(), O_RDWR | O_CREAT, 0600);
    if(fd < 0)
    {
        perror(path.c_str());
        _exit(1);
    }
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    if(fcntl(fd, F_SETLK, &fl) < 0)
    {
        return true;
    }
    unlink(path.c_str());
    return false;
#endif
}

void wxdfastStart(const std::string &path="")
{
    if(wxdfastRunning())
        return;
#if WIDLE
    logLine(path);
    std::wstring app_w(path.length(), L' ');
    std::copy(path.begin(), path.end(), app_w.begin());
    std::wstring arg_w(L"-i");
    const wchar_t* arg_const = arg_w.c_str();
    const wchar_t* app_const = app_w.c_str();
    ShellExecuteW(HWND_DESKTOP, L"open", app_const, arg_const, NULL, SW_NORMAL);
#else
    int pid = fork();
    if(pid == 0)
    {
        setsid();
#if __APPLE__
        int ret = system(path.c_str());
#else
        int ret = system("wxdfast -i");
#endif
        if(ret == -1)
        {
            if(logger)
                *logger << "wxdfast start failed" << std::endl;
        }
    }
    else
    {
        dpid = pid;
    }
#endif
}

void sendMessage(const std::string &data)
{
    unsigned int len = data.length();
    std::cout.write(reinterpret_cast<const char *>(&len), 4);
    std::cout << data.c_str() << std::flush;
}

#if WIDLE
static int winsock2Initialised = 0;

// initialise the winsock2 DLL
bool winsock2init()
{
    if(winsock2Initialised == 0)
    {
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(2,0),&wsa) != 0 || LOBYTE(wsa.wVersion) != 2)
            return FALSE;
    }
    winsock2Initialised++;
    return TRUE;
}

// and another - cleaup winsock2 DLL resources
void winsock2cleanup()
{
    winsock2Initialised--;
    if(winsock2Initialised == 0) WSACleanup();
}
#endif

std::vector<std::string> split(const std::string& str)
{
   int numSubstrings = str.length() / MAX_BUFF_LEN;
   std::vector<std::string> ret;
   for(auto i = 0; i < numSubstrings; i++)
   {
        ret.push_back(str.substr(i * MAX_BUFF_LEN, MAX_BUFF_LEN));
   }
   // If there are leftover characters, create a shorter item at the end
   if(str.length() % MAX_BUFF_LEN != 0)
   {
        ret.push_back(str.substr(MAX_BUFF_LEN * numSubstrings));
   }
   return ret;
}

bool sendToIPC(const std::string &data)
{
#if WIDLE
    logLine("sendToIPC() started");
    if(!winsock2init())
    {
        logLine("Unable to initiliaze socket");
        logLine("sendToIPC() ended with error");
        return false;
    }
    addrinfo hints;
    addrinfo *servinfo = nullptr;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo("127.0.0.1", ICP_SERVICE_STR, &hints, &servinfo) != 0)
    {
        logLine("Bad host 'localhost'");
        logLine("sendToIPC() ended with error");
        return false;
    }
    if(servinfo == NULL)
    {
        logLine("Unable to connect to '127.0.0.1'");
        logLine("sendToIPC() ended with error");
        return false;
    }
    SOCKET sd;
    if((sd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == INVALID_SOCKET)
    {
        freeaddrinfo(servinfo);
        logLine("Unable to create socket");
        logLine("sendToIPC() ended with error");
        return false;
    }
    if(connect(sd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        closesocket(sd);
        freeaddrinfo(servinfo);
        logLine("Unable to connect to '127.0.0.1'");
        logLine("sendToIPC() ended with error");
        return false;
    }
	int rc;
    if(data.size() > MAX_BUFF_LEN)
    {
        std::vector<std::string> parts = split(data);
        for(const auto &line : parts)
        {
            rc = send(sd, line.c_str(), line.size(), 0);
            if(rc < 0)
            {
                logLine("send() failed");
                shutdown(sd, SD_BOTH);
                closesocket(sd);
                logLine("sendToIPC() ended with error");
                return false;
            }
        }
    }
    else
    {
        rc = send(sd, data.c_str(), data.size(), 0);
        if(rc < 0)
        {
            logLine("send() failed");
            shutdown(sd, SD_BOTH);
            closesocket(sd);
            logLine("sendToIPC() ended with error");
            return false;
        }
    }
    shutdown(sd, SD_BOTH);
    closesocket(sd);
    logLine("sendToIPC() ended");
    return true;
#else
    logLine("sendToIPC() started");
    int sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sd < 0)
    {
        logLine("socket() failed");
        return false;
    }
    int rc;
    struct sockaddr_un serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    std::string filename = getHomeDirectory()+"/.wxdfast.socket";
    strcpy(serveraddr.sun_path, filename.c_str());
    rc = connect(sd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
    if(rc < 0)
    {
        logLine("connect() failed");
        return false;
    }
    if(data.size() > MAX_BUFF_LEN)
    {
        std::vector<std::string> parts = split(data);
        for(const auto &line : parts)
        {
            rc = send(sd, line.c_str(), line.size(), 0);
            if(rc < 0)
            {
                logLine("send() failed");
                return false;
            }
        }
    }
    else
    {
        rc = send(sd, data.c_str(), data.size(), 0);
        if(rc < 0)
        {
            logLine("send() failed");
            return false;
        }
    }
    logLine("sendToIPC() ended");
    return true;
#endif
}

int main(int /*argc*/, char **argv)
{
#ifdef DEBUG
    if(!logger)
#if WIDLE
        logger = new std::ofstream(getTempDirectory()+PATHSEPSTRING+"wxdfast-log", std::ios::out|std::ios::app);
#else
#if __APPLE__
        logger = new std::ofstream(getHomeDirectory()+"/Library/Application Support/wxDownloadFast/log", std::ios::out|std::ios::app);
#else
        logger = new std::ofstream(getHomeDirectory()+"/.wxdfast/log", std::ios::out|std::ios::app);
#endif
#endif
#endif
    if(argv)
        logLine(argv[0]);
    using namespace std::chrono_literals;
    while(1)
    {
        if(!wxdfastRunning())
        {
#if WIDLE
            int tryes = 0;
            while(tryes < 3)
            {
                std::string path = argv[0];
                path.replace(path.find("wxdfast-integrator.exe"),22,"wxdfast.exe");
                wxdfastStart(path);
                if(wxdfastRunning())
                {
                    break;
                }
                std::this_thread::sleep_for(1250ms);
                tryes++;
            }
#else
            int tryes = 0;
            while(tryes < 3)
            {
#if __APPLE__
                std::string path = argv[0];
                path.replace(path.find("wxdfast-integrator"),18,"wxDownloadFast -i");
                wxdfastStart(path);
#else
                wxdfastStart();
#endif
                if(wxdfastRunning())
                {
                    break;
                }
                std::this_thread::sleep_for(750ms);
                tryes++;
            }
#endif
        }
        if(!wxdfastRunning())
        {
            logLine("wxDownload Fast didn't started");
            if(logger)
            {
                logger->close();
                delete logger;
                logger = nullptr;
            }
            return 0;
        }
        else
        {
            logLine("wxDownload Fast running");
        }
        char len[4];
        std::cin.read(len, 4);
        unsigned int ilen = *reinterpret_cast<unsigned int *>(len);
        if(!ilen)
        {
            logLine("wxdfast-integrator ended (zero length of message)");
            if(logger)
            {
                logger->close();
                delete logger;
                logger = nullptr;
            }
            return 0;
        }
        logLine("Message length: " + std::to_string(ilen));
        char *inMsg = new char[ilen];
        memset(inMsg,0,ilen+1);
        std::cin.read(inMsg, ilen);
        std::string inStr(inMsg);
        logLine("Received message: "+inStr);
        delete[] inMsg;
        if(inStr.find("URL") == std::string::npos)
        {
            std::string version = "{\"Status\": \"Available\", \"Version\": \"";
            version += INTEGRATOR_VERSION;
            version += "\", \"wxdfast\": \"";
            version += VERSION;
            version += "\"}";
            sendMessage(version);
            if(logger)
            {
                logger->close();
                delete logger;
                logger = nullptr;
            }
            return 0;
        }
        std::string version = "{\"Status\": \"Available\", \"Version\": \"";
        version += INTEGRATOR_VERSION;
        version += "\", \"wxdfast\": \"";
        version += VERSION;
        version += "\"}";
        sendMessage(version);
        if(inStr.find("\"URL\":\"\",") == std::string::npos)
        {
            if(inStr.find("\"Batch\":true") != std::string::npos)
            {
                logLine("Batch URLs received");
            }
            else
            {
                logLine("One URL received");
            }
            sendToIPC(std::string("<url>")+inStr+"</url>");
        }
        logLine("wxdfast-integrator ended");
        if(logger)
        {
            logger->close();
            delete logger;
            logger = nullptr;
        }
        return 0;
    }
    logLine("wxdfast-integrator ended");
    if(logger)
    {
        logger->close();
        delete logger;
        logger = nullptr;
    }
    return 0;
}

