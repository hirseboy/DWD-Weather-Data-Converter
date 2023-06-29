/*	DWD Data Converter Tool.

	Copyright (c) 2020-today, Institut für Bauklimatik, TU Dresden, Germany

	Primary authors:
	  Stephan Hirth  <stephan.hirth -[at]- tu-dresden.de>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#ifndef DWDMessageHandlerH
#define DWDMessageHandlerH

#include <QObject>
#include <IBK_MessageHandler.h>

class DWDMessageHandler : public QObject, public IBK::MessageHandler {
	Q_OBJECT
public:
	explicit DWDMessageHandler(QObject *parent = nullptr);
	virtual ~DWDMessageHandler();


	/*! Overloaded to received msg info. */
	virtual void msg(const std::string& msg,
		IBK::msg_type_t t = IBK::MSG_PROGRESS,
		const char * func_id = nullptr,
		int verbose_level = -1);

signals:
	/*! Emitted whenever a message was received.
		Shall be connected to the log window to display the message.
	*/
	void msgReceived(int type, QString msg);

};

#endif // DWDMessageHandlerH
