#!/bin/bash

# install.sh
# Installation and Uninstallation script for GenXPassword on Arch Linux

set -e  # Exit immediately if a command exits with a non-zero status

# Variables
APP_NAME="GenXPassword"
VERSION="2.0.0"
INSTALL_DIR="/opt/$APP_NAME"
BIN_DIR="/usr/local/bin"
SHARE_DIR="/usr/share/$APP_NAME"
ICON_DIR="/usr/share/icons/hicolor/256x256/apps"
DESKTOP_ENTRY="/usr/share/applications/$APP_NAME.desktop"
TRANSLATIONS_DIR="$SHARE_DIR/translations"
QSS_DIR="$SHARE_DIR/qss"
CONFIG_DIR="$SHARE_DIR/config"

# Function to display usage
usage() {
    echo "Usage: sudo $0 [install|uninstall]"
    exit 1
}

# Function to check for root privileges
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo "Error: This script must be run as root. Use sudo."
        exit 1
    fi
}

# Function to install dependencies
install_dependencies() {
    echo "Checking for required dependencies..."

    # Check for Qt5 base libraries
    if ! pacman -Qi qt5-base > /dev/null 2>&1; then
        echo "Qt5 base libraries not found. Installing..."
        pacman -S --noconfirm qt5-base
    else
        echo "Qt5 base libraries are already installed."
    fi

    # Check for ImageMagick
    if ! pacman -Qi imagemagick > /dev/null 2>&1; then
        echo "ImageMagick not found. Installing..."
        pacman -S --noconfirm imagemagick
    else
        echo "ImageMagick is already installed."
    fi

    echo "All dependencies are satisfied."
}

# Function to install the application
install_app() {
    echo "Installing $APP_NAME..."

    # Create installation directory
    mkdir -p "$INSTALL_DIR/bin"

    # Copy executable
    if [ ! -f "build/bin/$APP_NAME" ]; then
        echo "Error: Executable not found at build/bin/$APP_NAME. Please build the project first."
        exit 1
    fi
    cp "build/bin/$APP_NAME" "$INSTALL_DIR/bin/"

    # Create symbolic link in /usr/local/bin
    ln -sf "$INSTALL_DIR/bin/$APP_NAME" "$BIN_DIR/$APP_NAME"

    # Create shared directories
    mkdir -p "$TRANSLATIONS_DIR"
    mkdir -p "$QSS_DIR"
    mkdir -p "$CONFIG_DIR"

    # Copy translation files
    if ls i18n/*.qm 1> /dev/null 2>&1; then
        cp i18n/*.qm "$TRANSLATIONS_DIR/"
    else
        echo "No translation files found in i18n/*.qm."
    fi

    # Copy QSS stylesheets
    if ls qss/*.css 1> /dev/null 2>&1; then
        cp qss/*.css "$QSS_DIR/"
    else
        echo "No QSS files found in qss/*.css."
    fi

    # Copy configuration files
    if ls config/*.xml 1> /dev/null 2>&1; then
        cp config/*.xml "$CONFIG_DIR/"
    else
        echo "No XML configuration files found in config/*.xml."
    fi

    if ls config/*.ini 1> /dev/null 2>&1; then
        cp config/*.ini "$CONFIG_DIR/"
    else
        echo "No INI configuration files found in config/*.ini."
    fi

    # Install icons
    install_icons

    # Create desktop entry
    install_desktop_entry

    # Set executable permissions
    chmod +x "$INSTALL_DIR/bin/$APP_NAME"
    chmod +x "$BIN_DIR/$APP_NAME"

    echo "$APP_NAME installed successfully."
}

# Function to install icons
install_icons() {
    echo "Installing application icon..."

    # Define source and destination paths
    ICON_SRC_ICO="icons/appIcon.ico"
    ICON_DST_PNG="$ICON_DIR/genxpassword.png"

    # Convert ICO to PNG for Linux
    if [ -f "$ICON_SRC_ICO" ]; then
        magick "$ICON_SRC_ICO" -resize 256x256 "$APP_NAME.png"
        cp "$APP_NAME.png" "$ICON_DST_PNG"
        rm "$APP_NAME.png"
        echo "ICO icon converted to PNG and installed."
    else
        echo "Icon file appIcon.ico not found in icons/. Skipping icon installation."
    fi

    # Update icon cache
    gtk-update-icon-cache /usr/share/icons/hicolor > /dev/null 2>&1 || true

    echo "Icon installed."
}

# Function to install desktop entry
install_desktop_entry() {
    echo "Creating desktop entry..."

    # Create desktop entry file
    cat <<EOL > "$DESKTOP_ENTRY"
[Desktop Entry]
Name=GenXPassword
Exec=GenXPassword
Icon=genxpassword
Type=Application
Categories=Utility;Security;
StartupNotify=true
EOL

    echo "Desktop entry created."
}

# Function to uninstall the application
uninstall_app() {
    echo "Uninstalling $APP_NAME..."

    # Remove executable and symbolic link
    rm -f "$INSTALL_DIR/bin/$APP_NAME"
    rm -f "$BIN_DIR/$APP_NAME"

    # Remove shared directories
    rm -rf "$SHARE_DIR"

    # Remove desktop entry
    rm -f "$DESKTOP_ENTRY"

    # Remove icon
    rm -f "$ICON_DIR/genxpassword.png"

    # Update icon cache
    gtk-update-icon-cache /usr/share/icons/hicolor > /dev/null 2>&1 || true

    # Optionally remove install directory if empty
    rmdir "$INSTALL_DIR/bin" "$INSTALL_DIR" 2>/dev/null || true

    echo "$APP_NAME uninstalled successfully."
}

# Main function
main() {
    if [ "$#" -ne 1 ]; then
        usage
    fi

    case "$1" in
        install)
            check_root
            install_dependencies
            install_app
            ;;
        uninstall)
            check_root
            uninstall_app
            ;;
        *)
            usage
            ;;
    esac
}

# Execute main function with all script arguments
main "$@"
