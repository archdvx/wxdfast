@echo off

@setlocal enableextensions
@cd /d "%~dp0"

SET "current_path=%~dp0wxdfast-integrator.exe"
SET "current_path=%current_path:\=\\%"

:: Create JSON config files
@echo {"name":"com.wxdfast.chrome","description":"Integrate wxDownload Fast with Google Chrome and similar","path":"%current_path%","type":"stdio","allowed_origins":["chrome-extension://blddhoccpjdmfjhoedgoopccicopnjbc/","chrome-extension://kpijmhekjhfhdngjfhcgjoleekefmomb/","chrome-extension://dncifgibkggaiflipilmikckmfkjonle/"]}> com.wxdfast.chrome.json

@echo {"name":"com.wxdfast.firefox","description":"Integrate wxDownload Fast with Mozilla Firefox","path":"%current_path%","type":"stdio","allowed_extensions":["wxdfast@archdvx"]}> com.wxdfast.firefox.json

:: Create required registry entries
REG ADD HKCU\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.wxdfast.chrome /f /ve /t REG_SZ /d "%~dp0com.wxdfast.chrome.json"
REG ADD HKCU\SOFTWARE\Mozilla\NativeMessagingHosts\com.wxdfast.firefox /f /ve /t REG_SZ /d "%~dp0com.wxdfast.firefox.json"