; installer.nsi
; NSIS script for GenXPassword installer

# Define basic attributes
Name "GenXPassword"
OutFile "GenXPasswordInstaller.exe"
InstallDir "$PROGRAMFILES\GenXPassword"
RequestExecutionLevel admin

# Define installer icon
Icon "icons\appIcon.ico"

# Pages
Page directory
Page instfiles

# Sections
Section "Install"

    ; Create installation directory
    CreateDirectory "$INSTDIR"

    ; Set output path to installation directory
    SetOutPath "$INSTDIR"

    ; Copy executable
    File "build\bin\GenXPassword.exe"

    ; Copy icon
    File "icons\appIcon.ico"

    ; Copy translation files
    File "i18n\*.qm"

    ; Copy QSS stylesheets
    File "qss\*.css"

    ; Copy configuration files
    File "config\*.xml"
    File "config\*.ini"

    ; Create Start Menu shortcut
    CreateDirectory "$SMPROGRAMS\GenXPassword"
    CreateShortCut "$SMPROGRAMS\GenXPassword\GenXPassword.lnk" "$INSTDIR\GenXPassword.exe" "" "$INSTDIR\appIcon.ico"

    ; Create Desktop shortcut
    CreateShortCut "$DESKTOP\GenXPassword.lnk" "$INSTDIR\GenXPassword.exe" "" "$INSTDIR\appIcon.ico"

SectionEnd

Section "Uninstall"

    ; Remove installed files
    Delete "$INSTDIR\GenXPassword.exe"
    Delete "$INSTDIR\appIcon.ico"

    ; Remove translation files
    Delete "$INSTDIR\*.qm"

    ; Remove QSS stylesheets
    Delete "$INSTDIR\*.css"

    ; Remove configuration files
    Delete "$INSTDIR\*.xml"
    Delete "$INSTDIR\*.ini"

    ; Remove shortcuts
    Delete "$SMPROGRAMS\GenXPassword\GenXPassword.lnk"
    Delete "$DESKTOP\GenXPassword.lnk"

    ; Remove installation directory
    RMDir "$INSTDIR"

    ; Remove Start Menu directory
    RMDir "$SMPROGRAMS\GenXPassword"

SectionEnd
