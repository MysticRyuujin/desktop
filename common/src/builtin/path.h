// Copyright (c) 2024 Private Internet Access, Inc.
//
// This file is part of the Private Internet Access Desktop Client.
//
// The Private Internet Access Desktop Client is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// The Private Internet Access Desktop Client is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Private Internet Access Desktop Client.  If not, see
// <https://www.gnu.org/licenses/>.

#include "common.h"
#line HEADER_FILE("builtin/path.h")

#ifndef PATH_H
#define PATH_H
#pragma once

#include <QString>

namespace Files
{
    extern const COMMON_EXPORT QString wireguardGoBasename;
}

class COMMON_EXPORT Path
{
public:
    // Hard-coded expected installation directory on each platform
    // Windows: C:/Program Files/Private Internet Access
    // macOS: /Applications/Private Internet Access.app
    // Linux: /opt/privateinternetaccess
    static Path InstallationDir;

    // Base directory of the current running application (e.g. the installation directory or the debug directory)
    static Path BaseDir;

    // Path to all main binaries
    // Windows: <BaseDir>
    // macOS: <BaseDir>/Contents/MacOS
    // Linux: <BaseDir>/bin
    static Path ExecutableDir;

    // Path to shipped libraries
    static Path LibraryDir;

    // Read-only resource directory
    // Windows: <BaseDir>
    // macOS: <BaseDir>/Contents/Resources
    // Linux: <BaseDir>/share
    static Path ResourceDir;

    // Executable dir in installation (different from ExecutableDir if this
    // executable isn't running from the installation)
    // Windows: <InstallationDir>
    // macOS: <InstallationDir>/Contents/MacOS
    // Linux: <InstallationDir>/bin
    static Path InstallationExecutableDir;

    // System global writable temporary data directory for daemon use
    // Windows: <BaseDir>/data
    // macOS: /Library/Application Support/com.privateinternetaccess.vpn
    // Linux: <BaseDir>/var
    static Path DaemonDataDir;

    // Writable data directory for installers downloaded by daemon (to ensure
    // they don't clobber other data files)
    // All: <DaemonDataDir>/update
    static Path DaemonUpdateDir;

    // System global writable persistent settings directory for daemon use
    // Windows: <BaseDir>/data
    // macOS: /Library/Preferences/com.privateinternetaccess.vpn
    // Linux: <BaseDir>/etc
    static Path DaemonSettingsDir;

    // Daemon executable
    // Windows: <ExecutableDir>/vpn-service.exe
    // macOS & Linux: <ExecutableDir>/vpn-daemon
    static Path DaemonExecutable;

    // Client executable
    // Windows: <ExecutableDir>/vpn-client.exe
    // macOS: <ExecutableDir>/{PIA_PRODUCT_NAME}
    // Linux: <ExecutableDir>/vpn-client
    static Path ClientExecutable;

    // Cli executable
    // Windows: <ExecutableDir>/vpnctl.exe
    // macOS: /usr/local/bin/vpnctl
    // Linux: /usr/local/bin/vpnctl
    static Path CliExecutable;

    // Daemon log file
    // All: <DaemonDataDir>/daemon.log
    static Path DaemonLogFile;

    // Config log file (used on Windows for driver and service installation/
    // configuration performed by pia-service.exe)
    // All: <DaemonDataDir>/config.log
    static Path ConfigLogFile;

    // Updown script log file
    // All: <DaemonDataDir>/updown.log
    static Path UpdownLogFile;

    // Daemon Diagnostics directory, where diagnostics files are written
    // All: <DaemonDataDir>/diagnostics/
    static Path DaemonDiagnosticsDir;

    // Daemon local socket file/identifier
    // Windows: \\.\pipe\PrivateInternetAccessService
    // macOS & Linux: <DaemonDataDir>/daemon.sock
    static Path DaemonLocalSocket;

    // Daemon local socket for IPC from the OpenVPN helper - used to push
    // environment variables and errors back from the OpenVPN helper to the
    // daemon.
    static Path DaemonHelperIpcSocket;

    // Directory to store client crash reports
    // All: <ClientDataDir>/crashes/
    static Path ClientCrashReportDir;
    // Directory to store daemon crash reports
    // Daemon: <DaemonDataDir>/crashes/
    static Path DaemonCrashReportDir;

    // Crash reporter tool
    // Windows: <ExecutableDir>/pia-support-tool.exe
    // macOS: <ExecutableDir>/pia-support-tool
    // Linux: <ExecutableDir>/support-tool-launcher
    static Path SupportToolExecutable;

    // OpenVPN working directory
    // All: <ExecutableDir>
    static Path OpenVPNWorkingDir;

    // OpenVPN executable
    // Windows: <OpenVPNWorkingDir>/pia-openvpn.exe
    // macOS & Linux: <OpenVPNWorkingDir>/pia-openvpn
    static Path OpenVPNExecutable;

    // Generated config file for OpenVPN connections
    // All: <DaemonDataDir>/pia.ovpn
    static Path OpenVPNConfigFile;

    // Script or binary to be passed as OpenVPN's --up and --down arguments
    static Path OpenVPNUpDownScript;

    // hnsd executable (Handshake daemon)
    // Windows: <ExecutableDir>/pia-hnsd.exe
    // macOS & Linux: <ExecutableDir>/pia-hnsd
    static Path HnsdExecutable;

    // ss-local executable (Shadowsocks local client)
    // Windows: <ExecutableDir>/pia-ss-local.exe
    // macOS & Linux: <ExecutableDir>/pia-ss-local
    static Path SsLocalExecutable;

    // Unbound executable
    static Path UnboundExecutable;

    // Unbound config file
    static Path UnboundConfigFile;

    // Config file for Unbound instance used to block DNS on Mac
    static Path UnboundDnsStubConfigFile;

    // wireguard-go executable (Userspace Wireguard implementation - Mac/Linux)
    static Path WireguardGoExecutable;

    // Interface name file written by wireguard-go or wgservice.exe (Mac/Win only)
    static Path WireguardInterfaceFile;

    // Region override files
    static Path ModernShadowsocksOverride;
    static Path ModernRegionOverride;
    static Path ModernRegionMetaOverride;

    // Bundled region files
    static Path ModernShadowsocksBundle;
    static Path ModernRegionBundle;
    static Path ModernRegionMetaBundle;
#ifdef Q_OS_WIN
    // Directory of TAP drivers
    // Windows: <BaseDir>/tap
    static Path TapDriverDir;
    // Directory of WFP Callout drivers
    // Windows: <BaseDir>/wfp_callout
    static Path WfpCalloutDriverDir;
    // wgservice executable (Userspace Wireguard implementation - Windows)
    static Path WireguardServiceExecutable;
    // Wireguard config file (Userspace Wireguard implementation - Windows)
    static Path WireguardConfigFile;
#endif

#ifdef Q_OS_LINUX
    // The VFS net_cls cgroup file that contains PIDs for apps we wish to exclude from the VPN
    static Path VpnExclusionsFile;

    // The VFS net_cls cgroup file that contains PIDs for apps we wish to force on the VPN only
    static Path VpnOnlyFile;

    // The parent cgroup file of VpnExclusionsFile - adding a PID to this file removes it from exclusions.
    static Path ParentVpnExclusionsFile;
#endif

    // Per-user writable temporary data directory for client use
    // Windows: C:/Users/<USER>/AppData/Local/Private Internet Access
    // macOS: ~/Library/Application Support/com.privateinternetaccess.vpn
    // Linux: ~/.local/share/privateinternetaccess
    static Path ClientDataDir;

    // Per-user writable persistent settings directory for client use
    // Windows: C:/Users/<USER>/AppData/Local/Private Internet Access
    // macOS: ~/Library/Preferences/com.privateinternetaccess.vpn
    // Linux: ~/.config/privateinternetaccess
    static Path ClientSettingsDir;

    // Per-user client log file
    // All: <ClientDataDir>/client.log
    static Path ClientLogFile;

    // Per-user CLI log file
    // All: <ClientDataDir>/cli.log
    static Path CliLogFile;

#ifdef Q_OS_MAC
    // Update directory used by client to decompress installer.  Only used on
    // Mac, because the Mac download is a compressed app bundle.  On Windows and
    // Linux, the download is executed directly.
    // macOS: <ClientDataDir>/update
    static Path ClientUpdateDir;

    // Path to the launch agent property list used to launch the client at login
    static Path ClientLaunchAgentPlist;

    // Path to the PIA Split Tunnel Binary binary used to control the split tunnel on macos
    static Path TransparentProxyCliExecutable;
    static Path TransparentProxyLogFile;
#endif

#ifdef Q_OS_LINUX
    // An XDG Auto start file used to start the app automatically on login
    // Used on Linux
    // Usually ~/.config/autostart/pia-client.desktop
    static Path ClientAutoStartFile;
#endif

#ifdef Q_OS_MAC
    static Path SplitTunnelKextPath;
    // For tcpdump pcap files (used in diagnostics)
    static Path PcapDir;
#endif

    // Debug log config file
    static Path DebugFile;

public:
    Path();
    Path(const QString& path);

    // Initialize a few paths before QCoreApplication has been created.  A few
    // paths are needed in order to check client settings that set app
    // attributes.
    static void initializePreApp();
    // Initialize all remaining paths after QCoreAppliation has been created.
    // Most paths depend on the QCoreApplication.
    static void initializePostApp();

    // Return the path to the Program Files folder
    static Path getProgramsFolder();

    Path& operator=(const QString& path) { _path = path; return *this; }

    bool operator==(const Path &other) const {return _path == other._path;}
    bool operator==(const QString &path) const {return _path == path;}
    bool operator!=(const Path &other) const {return !(*this == other);}
    bool operator!=(const QString &path) const {return !(*this == path);}

    operator const QString&() const { return _path; }
    const QString &str() const {return _path; }

    operator std::string() const {return _path.toStdString();}
    operator std::wstring() const {return _path.toStdWString();}

    Path  operator/(const QString& child) const &;
    Path& operator/(const QString& child) &&;
    Path  operator+(const QString& suffix) const &;
    Path& operator+(const QString& suffix) &&;

    Path  operator/(const QStringView& child) const &;
    Path& operator/(const QStringView& child) &&;

    Path  operator/(const QLatin1String& child) const &;
    Path& operator/(const QLatin1String& child) &&;
    Path  operator+(const QLatin1String& suffix) const &;
    Path& operator+(const QLatin1String& suffix) &&;

    Path  operator/(const char* child) const &;
    Path& operator/(const char* child) &&;
    Path  operator+(const char* suffix) const &;
    Path& operator+(const char* suffix) &&;

    Path parent() const;

    Path& mkpath();
    const Path& mkpath() const;
    Path& mkparent();
    Path& touch();

    // Clean a directory of all but the most recently-modified files.
    // The newest 'keepCount' files (by modified time) are retained.
    // Subdirectories in the directory are not touched; any files that can't be
    // removed are logged and ignored.
    void cleanDirFiles(int keepCount);

private:
    template<typename Iterator> Path& appendPath(Iterator begin, Iterator end);
    Path& appendSegment(const QChar* first, const QChar* last);
    Path& appendSegment(const char* first, const char* last);
    Path& up();

private:
    QString _path;
};

COMMON_EXPORT std::ostream &operator<<(std::ostream &os, const Path &path);

#endif // PATH_H
