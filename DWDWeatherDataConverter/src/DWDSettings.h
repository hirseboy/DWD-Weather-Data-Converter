#ifndef DWDSETTINGS_H
#define DWDSETTINGS_H

#include <QtExt_Settings.h>

class DWDSettings : public QtExt::Settings {
	Q_DECLARE_TR_FUNCTIONS(SVSettings)
	Q_DISABLE_COPY(DWDSettings)
public:

	/*! Returns the instance of the singleton. */
	static DWDSettings & instance();

	/*! Standard constructor.
		\param organization Some string defining the group/organization/company (major registry root name).
		\param appName Some string defining the application name (second part of registry root name).

		You may only instantiate one instance of the settings object in your application. An attempt to
		create a second instance will raise an exception in the constructor.
	*/
	DWDSettings(const QString & organization, const QString & appName);

	/*! Reads the user specific config data.
		The data is read in the usual method supported by the various platforms.
	*/
	void read() override;


	/*! Sets default options (after first program start). */
	void setDefaults() override;

	/*! Writes the user specific config data.
		The data is writted in the usual method supported by the various platforms.
		\param geometry A bytearray with the main window geometry (see QMainWindow::saveGeometry())
		\param state A bytearray with the main window state (toolbars, dockwidgets etc.)
					(see QMainWindow::saveState())
	*/
	void write(QByteArray geometry, QByteArray state) override;


	/*! The global pointer to the SVSettings object.
		This pointer is set in the constructor, and cleared in the destructor.
	*/
	static DWDSettings			*m_self;

};

#endif // DWDSETTINGS_H
