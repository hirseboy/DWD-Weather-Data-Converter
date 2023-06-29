#ifndef DWDDownloaderH
#define DWDDownloaderH

#include <QtNetwork>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QtCore>
#include <QThread>
#include <QProgressDialog>
#include <QLabel>
#include "IBK_Path.h"

class DWDProgressBar;

class DWDDownloader: public QObject
{
	Q_OBJECT

public:
	DWDDownloader(QWidget * parent);
	void doDownload(const QUrl &url);
	static QString saveFileName(const QUrl &url);
	bool saveToDisk(const QString &filename, QIODevice *data);
	static bool isHttpRedirect(QNetworkReply *reply);
	void setFilepath(IBK::Path filepath);

	QStringList					m_urls;
	qint64						m_bytesReceived = 0;
	qint64						m_bytesTotal = 0;
	bool						m_isRunning = true;
	int							m_finishedDownloads;
	IBK::Path					m_filepath;

	/*! Progress Dialog. */
	QProgressDialog				*m_progressDlg;
signals:
	void finished();
	QNetworkRequest request();

private:
	QNetworkAccessManager		m_manager;
	QVector<QNetworkReply *>	m_currentDownloads;
public slots:
	void execute();
	void downloadProgress(qint64,qint64);
	void downloadFinished(QNetworkReply *reply);
	void sslErrors(const QList<QSslError> &errors);

};

#endif // DWDDownloaderH
