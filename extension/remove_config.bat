@echo off

:: Delete JSON config files
DEL com.wxdfast.chrome.json
DEL com.wxdfast.firefox.json

:: Delete registry entries
REG DELETE HKCU\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.wxdfast.chrome /f
REG DELETE HKCU\SOFTWARE\Mozilla\NativeMessagingHosts\com.wxdfast.firefox /f