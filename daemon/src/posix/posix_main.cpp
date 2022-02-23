// Copyright (c) 2022 Private Internet Access, Inc.
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
#line SOURCE_FILE("posix/posix_main.cpp")

#if defined(UNIT_TEST)

// Entry point shouldn't be included for these projects
void dummyPosixMain() {}

#else

#include "posix.h"
#include "posix_daemon.h"
#include "path.h"
#include "version.h"
#include "brand.h"

#include <exception>
#include <stdexcept>

#include <cxxabi.h>
#include <stdio.h>
#include <sys/stat.h>
#include <QCommandLineParser>

static void (*g_oldTerminateHandler)() = nullptr;


static void terminateHandler()
{
    // This is always a programming error; we should never let exceptions
    // propagate outside the current event loop invocation as Qt is not
    // exception safe.

    std::set_terminate(g_oldTerminateHandler);

    char extra[128];
    extra[0] = 0;
    if (std::exception_ptr ep = std::current_exception())
    {
        try
        {
            std::rethrow_exception(ep);
        }
        catch (const std::exception& e)
        {
            snprintf(extra, sizeof(extra), "\n  what() = \"%s\"", e.what());
        }
        catch (...)
        {
            if (std::type_info* et = abi::__cxa_current_exception_type())
                snprintf(extra, sizeof(extra), "\n  type = %s", et->name());
        }
    }
    qFatal("Exiting due to unhandled exception%s", extra);
}


int main(int argc, char** argv)
{
    setUtf8LocaleCodec();
    Logger::initialize(isatty(2));

    FUNCTION_LOGGING_CATEGORY("posix.main");

    // Set default umask (not writable by group or others)
    umask(S_IWGRP | S_IWOTH);

    Path::initializePreApp();
    QCoreApplication app(argc, argv);

    g_oldTerminateHandler = std::set_terminate(terminateHandler);

    Path::initializePostApp();

    // The daemon just has the general --help and --version options.
    // --version is used by the install script on Linux to verify that the
    // daemon can be linked and executed.
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(QStringLiteral(
        BRAND_SHORT_NAME " daemon - manages the " BRAND_SHORT_NAME " VPN connection as root."));
    if(!parser.parse(QCoreApplication::arguments()))
    {
        QTextStream{stderr} << parser.errorText() << Qt::endl;
        return -1;
    }

    if(parser.isSet(QStringLiteral("help")))
    {
        QTextStream{stdout} << parser.helpText() << Qt::endl;
        return 0;
    }
    if(parser.isSet(QStringLiteral("version")))
    {
        QTextStream{stdout} << Version::semanticVersion() << Qt::endl;
        return 0;
    }
    Logger logSingleton{Path::DaemonLogFile};

    setUidAndGid();

    int exitCode = 1;
    try
    {
        // Instantiate and synchronously run the posix version of the daemon
        PosixDaemon daemon;
        QObject::connect(&daemon, &Daemon::started, &QCoreApplication::exec);
        QObject::connect(&daemon, &Daemon::stopped, &QCoreApplication::quit);
        daemon.start();

        exitCode = 0;
    }
    catch (const Error& error)
    {
        qCritical() << error;
    }

    std::set_terminate(g_oldTerminateHandler);

    if (exitCode)
        qWarning() << "Exited daemon with error code" << exitCode;
    else
        qInfo() << "Exited daemon successfully.";
}

#endif
