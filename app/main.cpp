#include <QApplication>
#include <QSplashScreen>
#include <QtCore>
#include <QLibraryInfo>
#include "ui/mainwindow.h"
#include "log4qt/logger.h"
#include "log4qt/consoleappender.h"
#include "log4qt/ttcclayout.h"
#include "log4qt/logmanager.h"
#include "log4qt/fileappender.h"
#include "log4qt/rollingfileappender.h"
#include "log4qt/loggerrepository.h"

void setupRootLogger()
{
    // Create a layout
    auto logger = Log4Qt::Logger::rootLogger();
    auto *layout = new Log4Qt::TTCCLayout();
    layout->setName(QStringLiteral("CuteLayout"));
    layout->setDateFormat(Log4Qt::TTCCLayout::dfISO8601);
    layout->setThreadPrinting(true);
    layout->setContextPrinting(true);
    layout->setCategoryPrefixing(true);
    layout->activateOptions();
    // Create a console appender
    Log4Qt::ConsoleAppender *consoleAppender = new Log4Qt::ConsoleAppender(layout, Log4Qt::ConsoleAppender::STDOUT_TARGET);
    consoleAppender->setName(QStringLiteral("ConsoleAppender"));
    consoleAppender->activateOptions();
    // Add appender on root logger
    logger->addAppender(consoleAppender);
    // Create a file appender
    auto logDir = QApplication::applicationDirPath();
    QFileInfo dirInfo(logDir);
    if(!dirInfo.isWritable())
        logDir = QDir::homePath();

    //    Log4Qt::FileAppender *fileAppender = new Log4Qt::FileAppender(layout, QCoreApplication::applicationDirPath() + "/cuteboard.log", true);
    Log4Qt::RollingFileAppender *fileAppender = new Log4Qt::RollingFileAppender(layout, logDir + QDir::separator() + QApplication::applicationName() + ".log", true);
    fileAppender->setName(QStringLiteral("RollingFileAppender"));
    fileAppender->setMaxBackupIndex(3);
    fileAppender->activateOptions();
    // Add appender on root logger
    logger->addAppender(fileAppender);

    // Set level to info
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
    // Enable handling of Qt messages
    Log4Qt::LogManager::setHandleQtMessages(true);

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          START                               #"));
    logger->info(QStringLiteral("################################################################"));
    logger->info() << QString("Logging directory - %1").arg(logDir);
}

void shutDownRootLogger()
{
    auto logger = Log4Qt::Logger::rootLogger();

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          STOP                                #"));
    logger->info(QStringLiteral("################################################################"));

    logger->removeAllAppenders();
    logger->loggerRepository()->shutdown();
}

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    app.setApplicationName("CuteBoard");

    setupRootLogger();

    QString profileName;
    QCommandLineParser parser;
    parser.setApplicationDescription("CuteBoard real time visualization");
    parser.addHelpOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption profileOption("profile",
                                     QApplication::translate("main", "Data profile <profileName>"),
                                     QApplication::translate("main", "profileName"));
    parser.addOption(profileOption);

    QCommandLineOption screenOption("screen",
                                    QApplication::translate("main", "screen number <number>"),
                                    QApplication::translate("main", "number"));
    parser.addOption(screenOption);

    QCommandLineOption projectOption("project",
                                     QApplication::translate("main", "Visualization project <path>"),
                                     QApplication::translate("main", "dir_path"));
    parser.addOption(projectOption);

    QCommandLineOption pageOption("page",
                                  QApplication::translate("main", "Page displayed <page_name>"),
                                  QApplication::translate("main", "page_name"));
    parser.addOption(pageOption);

    QCommandLineOption fullscreenOption("fullscreen",
                                        QApplication::translate("main", "fullscreen"));
    parser.addOption(fullscreenOption);

    parser.process(app);

    if(parser.isSet(profileOption)) {
        profileName = parser.value(profileOption);
    }

    QFont ft = app.font();
    ft.setPointSizeF(8.5);
    app.setFont(ft);

    QScreen* screen = nullptr;
    if(parser.isSet(screenOption)) {
        int screenNumber = parser.value(screenOption).toInt();
        auto screensList = app.screens();
        if(screenNumber >= 0 && screenNumber < screensList.count())
            screen = screensList.at(screenNumber);
    }

    if(screen == nullptr) {
        screen = app.primaryScreen();
    }

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QString("palettes.ini"),
                       QSettings::IniFormat);

    QPalette pal = app.palette();

    auto customPaletteName = settings.value("DefaultPalette").toString();
    Log4Qt::Logger::rootLogger()->info() << QString("Requested theme palette -  %1").arg(customPaletteName);
    settings.beginGroup("Palettes");
    if(settings.childGroups().contains(customPaletteName)) {
        settings.beginGroup(customPaletteName);

        auto states = settings.childGroups();
        for(auto state : states) {
            settings.beginGroup(state);

            auto keys = settings.childKeys();
            for(auto key : keys) {
                pal.setColor(QPalette::ColorGroup(QMetaEnum::fromType<QPalette::ColorGroup>().keyToValue(state.toStdString().c_str())),
                             QPalette::ColorRole(QMetaEnum::fromType<QPalette::ColorRole>().keyToValue(key.toStdString().c_str())),
                             QColor(settings.value(key).toString()));
            }

            settings.endGroup();
        }

        settings.endGroup();
    }
    settings.endGroup();

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setPalette(pal);

    QColor color_overlay = pal.color(QPalette::Active, QPalette::Text);
    QColor color_base = pal.color(QPalette::Active, QPalette::Window);
    QPixmap overlay(":/cuteboard.png");

    QPainter painter_overlay(&overlay);
    painter_overlay.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_overlay.setBrush(color_overlay);
    painter_overlay.setPen(color_overlay);
    painter_overlay.drawRect(overlay.rect());

    QPixmap background(overlay.width(), overlay.height());
    background.fill(color_base);

    QPixmap result(overlay.width(), overlay.height());
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.drawPixmap(0,0,background);
    painter.drawPixmap(0,0,overlay);

    QSplashScreen splash(result);
    splash.move(screen->availableGeometry().center() - splash.rect().center());
    splash.show();
    QApplication::processEvents();

    splash.showMessage("Building application", Qt::AlignLeft, color_overlay);
    MainWindow mainWindow;
    mainWindow.move(screen->availableGeometry().center() - mainWindow.rect().center());
    mainWindow.setWindowState(Qt::WindowMaximized);
    splash.showMessage("Loading profile", Qt::AlignLeft, color_overlay);
    mainWindow.init(profileName);

    bool fullScreen = false;
    if(parser.isSet(projectOption)) {
        auto projectPath = parser.value(projectOption);
        QString pageName;
        if(parser.isSet(pageOption))
            pageName = parser.value(pageOption);
        if(mainWindow.loadProject(projectPath, pageName)) {
            if(parser.isSet(fullscreenOption))
                fullScreen = true;
        }
    }

    if(fullScreen)
        mainWindow.fullScreen();
    else
        mainWindow.show();

    splash.finish(&mainWindow);
    splash.hide();

    int ret = app.exec();

    shutDownRootLogger();

    return ret;
}
