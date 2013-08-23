#include <iostream>
#include <cstdlib>

#include <QDir>
#include <QCoreApplication>

#include "mythconfig.h"  // for CONFIG_DARWIN
#include "mythdirs.h"
#include "mythverbose.h"

static QString installprefix = QString::null;
static QString sharedir = QString::null;
static QString libdir = QString::null;
static QString confdir = QString::null;
static QString themedir = QString::null;
static QString pluginsdir = QString::null;
static QString translationsdir = QString::null;
static QString filtersdir = QString::null;

void InitializeMythDirs(void)
{
    installprefix = QString(RUNPREFIX);

    char *tmp_installprefix = std::getenv("MYTHTVDIR");
    if (tmp_installprefix)
        installprefix = tmp_installprefix;

	VERBOSE(VB_IMPORTANT, "installprefix = " + installprefix);

	
#if CONFIG_DARWIN
    // Work around bug in OS X where applicationDirPath() can crash
    // (if binary is not in a bundle, and is daemon()ized)

    QDir prefixDir = QFileInfo(qApp->argv()[0]).dir();
#else
    QDir prefixDir = qApp->applicationDirPath();
#endif

    if (QDir(installprefix).isRelative())
    {
        // If the PREFIX is relative, evaluate it relative to our
        // executable directory. This can be fragile on Unix, so
        // use relative PREFIX values with care.

        VERBOSE(VB_IMPORTANT+VB_EXTRA,
                "Relative PREFIX! (" + installprefix +
                ")\n\t\tappDir=" + prefixDir.canonicalPath());
        prefixDir.cd(installprefix);
        installprefix = prefixDir.canonicalPath();
    }

    VERBOSE(VB_IMPORTANT, "Using runtime prefix = " + installprefix);

    char *tmp_confdir = std::getenv("MYTHCONFDIR");
    if (tmp_confdir)
    {
        confdir = QString(tmp_confdir);
        VERBOSE(VB_IMPORTANT, QString("Read conf dir = %1").arg(confdir));
        //confdir.replace("$HOME", QDir::homePath());
    }
    else
        confdir = QDir::homePath() + "/.mythtv";

    VERBOSE(VB_IMPORTANT, QString("Using configuration directory = %1")
                                 .arg(confdir));

    sharedir = installprefix + "/mythtv/";
    libdir = installprefix + '/' + QString(LIBDIRNAME) + "/mythtv/";
    themedir = sharedir + "themes/";
    pluginsdir = libdir + "plugins/";
    translationsdir = sharedir + "i18n/";
    filtersdir = libdir + "filters/";
}

QString GetInstallPrefix(void) {VERBOSE(VB_GUI, QString("installprefix = %1")
                                 .arg(installprefix));  return installprefix; }
QString GetShareDir(void) { VERBOSE(VB_GUI, QString("sharedir = %1")
                                 .arg(sharedir)); return sharedir; }
QString GetLibraryDir(void) {VERBOSE(VB_GUI, QString("libdir = %1")
                                 .arg(libdir)); return libdir; }
QString GetConfDir(void) {VERBOSE(VB_GUI, QString("confdir = %1")
                                 .arg(confdir)); return confdir; }
QString GetThemesParentDir(void) {VERBOSE(VB_GUI, QString("themedir = %1")
                                 .arg(themedir)); return themedir; }
QString GetPluginsDir(void) {VERBOSE(VB_GUI, QString("pluginsdir = %1")
                                 .arg(pluginsdir)); return pluginsdir; }
QString GetTranslationsDir(void) {VERBOSE(VB_GUI, QString("translationsdir = %1")
                                 .arg(translationsdir)); return translationsdir; }
QString GetFiltersDir(void) {VERBOSE(VB_GUI, QString("filtersdir = %1")
                                 .arg(filtersdir)); return filtersdir; }

// These defines provide portability for different
// plugin file names.
#if CONFIG_DARWIN
static const QString kPluginLibPrefix = "lib";
static const QString kPluginLibSuffix = ".dylib";
#elif USING_MINGW
static const QString kPluginLibPrefix = "lib";
static const QString kPluginLibSuffix = ".dll";
#else
static const QString kPluginLibPrefix = "lib";
static const QString kPluginLibSuffix = ".so";
#endif

QString GetPluginsNameFilter(void)
{
    return kPluginLibPrefix + '*' + kPluginLibSuffix;
}

QString FindPluginName(const QString &plugname)
{
    return GetPluginsDir() + kPluginLibPrefix + plugname + kPluginLibSuffix;
}

QString GetTranslationsNameFilter(void)
{
    return "mythfrontend_*.qm";
}

QString FindTranslation(const QString &translation)
{
    return GetTranslationsDir()
           + "mythfrontend_" + translation.toLower() + ".qm";
}

QString GetFontsDir(void)
{
    return GetShareDir() + "fonts/";
}

