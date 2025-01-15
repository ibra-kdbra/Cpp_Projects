#!/bin/bash

# install_macos.sh
# Installation and Uninstallation script for GenXPassword on macOS

set -e  # Exit immediately if a command exits with a non-zero status

# Variables
APP_NAME="GenXPassword"
VERSION="2.0.0"
APP_DIR="/Applications/$APP_NAME.app"
EXECUTABLE_NAME="$APP_NAME"
EXECUTABLE_PATH="$APP_DIR/Contents/MacOS/$EXECUTABLE_NAME"
RESOURCE_DIR="$APP_DIR/Contents/Resources"
ICON_SRC_ICO="icons/appIcon.ico"
ICON_SRC_PNG="icons/appIcon.png"  # Optional: Provide a PNG version for easier conversion
ICON_DST_ICNS="$RESOURCE_DIR/genxpassword.icns"

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

    # Check for Qt6 (installed via Homebrew)
    if ! brew list | grep -q qt@6; then
        echo "Qt6 not found. Installing via Homebrew..."
        brew install qt@6
    else
        echo "Qt6 is already installed."
    fi

    # Check for ImageMagick
    if ! brew list | grep -q imagemagick; then
        echo "ImageMagick not found. Installing via Homebrew..."
        brew install imagemagick
    else
        echo "ImageMagick is already installed."
    fi

    echo "All dependencies are satisfied."
}

# Function to create .icns from .ico
create_icns() {
    echo "Converting appIcon.ico to genxpassword.icns..."

    # Check if appIcon.png exists, else convert ico to png using ImageMagick
    if [ -f "$ICON_SRC_PNG" ]; then
        cp "$ICON_SRC_PNG" "$RESOURCE_DIR/genxpassword.png"
    elif [ -f "$ICON_SRC_ICO" ]; then
        magick convert "$ICON_SRC_ICO" "$RESOURCE_DIR/genxpassword.png"
    else
        echo "No appIcon.ico or appIcon.png found in icons/. Skipping icon conversion."
        return
    fi

    # Create a temporary folder for the iconset
    TEMP_ICONSET="temp.iconset"
    mkdir -p "$TEMP_ICONSET"

    # Resize the PNG to various sizes required for macOS icons
    sips -z 16 16     "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_16x16.png"
    sips -z 32 32     "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_16x16@2x.png"
    sips -z 32 32     "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_32x32.png"
    sips -z 64 64     "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_32x32@2x.png"
    sips -z 128 128   "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_128x128.png"
    sips -z 256 256   "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_128x128@2x.png"
    sips -z 256 256   "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_256x256.png"
    sips -z 512 512   "$RESOURCE_DIR/genxpassword.png" --out "$TEMP_ICONSET/icon_256x256@2x.png"
    cp "$RESOURCE_DIR/genxpassword.png" "$TEMP_ICONSET/icon_512x512.png"
    cp "$RESOURCE_DIR/genxpassword.png" "$TEMP_ICONSET/icon_512x512@2x.png"

    # Create .icns file from the iconset
    iconutil -c icns "$TEMP_ICONSET" -o "$ICON_DST_ICNS"

    # Clean up temporary files
    rm -rf "$TEMP_ICONSET" "$RESOURCE_DIR/genxpassword.png"

    echo "Icon converted to genxpassword.icns and installed."
}

# Function to create .app bundle
create_app_bundle() {
    echo "Creating application bundle..."

    mkdir -p "$APP_DIR/Contents/MacOS"
    mkdir -p "$RESOURCE_DIR"

    # Copy executable
    if [ ! -f "build/bin/$APP_NAME" ]; then
        echo "Error: Executable not found at build/bin/$APP_NAME. Please build the project first."
        exit 1
    fi
    cp "build/bin/$APP_NAME" "$EXECUTABLE_PATH"
    chmod +x "$EXECUTABLE_PATH"

    # Convert ICO to ICNS
    create_icns

    # Create Info.plist
    cat <<EOL > "$APP_DIR/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundleDisplayName</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>com.yourdomain.$APP_NAME</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>CFBundleExecutable</key>
    <string>$EXECUTABLE_NAME</string>
    <key>CFBundleIconFile</key>
    <string>genxpassword.icns</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOL

    echo "Info.plist created."
    echo "Application bundle created at $APP_DIR."
}

# Function to install the application
install_app() {
    echo "Installing $APP_NAME to $APP_DIR..."

    # Create the app bundle
    create_app_bundle

    echo "$APP_NAME installed successfully."
}

# Function to uninstall the application
uninstall_app() {
    echo "Uninstalling $APP_NAME..."

    # Remove the app bundle
    rm -rf "$APP_DIR"

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
