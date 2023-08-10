#include <IBK_messages.h>
#include <IBK_Exception.h>

#include <QApplication>
#include <QDir>
#include <QProxyStyle>
#include <QStyleOptionViewItem>
#include <QSplashScreen>

#include <QtExt_Directories.h>
#include <QtExt_LanguageHandler.h>

#include "DWDMainWindow.h"
#include "DWDMessageHandler.h"

#include "DWDConstants.h"
#include "DWDSettings.h"
#include "qscreen.h"
#include "qtimer.h"

/*! qDebug() message handler function, redirects debug messages to IBK::IBK_Message(). */
void qDebugMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	(void) type;
	std::string contextstr;
	if (context.file != nullptr && context.function != nullptr)
		contextstr = "[" + std::string(context.file) + "::" + std::string(context.function) + "]";
	IBK::IBK_Message(msg.toStdString(), IBK::MSG_DEBUG, contextstr.c_str(), IBK::VL_ALL);
}

int main(int argc, char* argv[]) {
	FUNCID(main);

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QtExt::Directories::appname = "DWDWeatherDataConverter";
	QtExt::Directories::devdir = "DWDWeatherDataConverter";


	QFile styleDark(":/style/style.qss");
	styleDark.open(QFile::ReadOnly);
	QString style = QLatin1String(styleDark.readAll());


	const QString ProgramVersionName = QString("DWDWeatherDataConverter %1").arg(VERSION);
	QApplication a( argc, argv );

	QIcon icon(QPixmap(":/icon/DWDWeatherDataConverter.png"));
	qApp->setWindowIcon(icon);
	qApp->setApplicationName(ProgramVersionName);

	DWDSettings settings(ORG_NAME, ProgramVersionName);
	settings.setDefaults();
	settings.read();

	QtExt::LanguageHandler::instance().installTranslator(QtExt::LanguageHandler::langId());
	// install message handler to catch qDebug()
	qInstallMessageHandler(qDebugMsgHandler);

	// *** Create log file directory and setup message handler ***
	QDir baseDir;
	QtExt::Directories::appname = PROGRAM_NAME;
	baseDir.mkpath(QtExt::Directories::userDataDir());

	DWDMessageHandler messageHandler;
	IBK::MessageHandlerRegistry::instance().setMessageHandler( &messageHandler );


	std::string errmsg;
	messageHandler.openLogFile(QtExt::Directories::globalLogFile().toUtf8().data(), false, errmsg);


	// *** Create and show splash-screen ***
	std::unique_ptr<QSplashScreen> splash;

	if (!settings.m_flags[DWDSettings::NoSplashScreen]) {
		QPixmap pixmap;

		pixmap.load(":/splashscreen/DWDWeatherDataConverterSplash.png");

		// is needed for high dpi screens to prevent bluring
		double ratio = a.primaryScreen()->devicePixelRatio();
		pixmap.setDevicePixelRatio(ratio);

		// show splash screen
		splash.reset(new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint | Qt::SplashScreen));
		splash->show();
		QTimer::singleShot(5000, splash.get(), SLOT(close()));
	}

	// *** Setup and show MainWindow and start event loop ***
	int res;
	try { // open scope to control lifetime of main window, ensure that main window instance dies before settings or project handler

		MainWindow w;

		// qApp->setStyleSheet(style);

		// add user settings related window resize at program start
		w.loadDataFromDWDServer();
		w.showMaximized();
		w.activateWindow(); // set the focus
		// we set the data to the tableWidget
		// start event loop
		res = a.exec();
	} // here our mainwindow dies, main window goes out of scope and UI goes down -> destructor does ui and thread cleanup
	catch (IBK::Exception & ex) {
		ex.writeMsgStackToError();
		return EXIT_FAILURE;
	}
	catch (...) {
		return EXIT_FAILURE;
	}
	// return exit code to environment

	return res;
	return 0;
}


