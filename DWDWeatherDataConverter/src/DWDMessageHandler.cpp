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

#include "DWDMessageHandler.h"

DWDMessageHandler::DWDMessageHandler(QObject *parent) :
	QObject(parent)
{
}


DWDMessageHandler::~DWDMessageHandler() {
}


void DWDMessageHandler::msg(const std::string& msg,
	IBK::msg_type_t t,
	const char * func_id,
	int verbose_level)
{
	IBK::MessageHandler::msg(msg, t, func_id, verbose_level);
	if (verbose_level > m_requestedConsoleVerbosityLevel)
		return;

	emit msgReceived(t, QString::fromStdString(msg).trimmed());
}

