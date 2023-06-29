#include "DWDSettings.h"
#include "qsettings.h"

DWDSettings * DWDSettings::m_self = nullptr;

DWDSettings &DWDSettings::instance() {
	Q_ASSERT_X(m_self != nullptr, "[DWDSettings::instance]", "You must create an instance of "
		"DWDSettings before accessing SVSettings::instance()!");
	return *m_self;
}

DWDSettings::DWDSettings(const QString & organization, const QString & appName)
	:	QtExt::Settings(organization, appName)
{
	// singleton check
	Q_ASSERT_X(m_self == nullptr, "[DWDSettings::DWDSettings]", "You must not create multiple instances of "
															  "classes that derive from SVSettings!");
	for(unsigned int i=0; i<NumCmdLineFlags; ++i)
		m_flags[i] = false;
	m_self = this;
}

void DWDSettings::read() {

	QtExt::Settings::read();
	QSettings settings( m_organization, m_appName );

}

void DWDSettings::setDefaults() {
	QtExt::Settings::setDefaults();
}

void DWDSettings::write(QByteArray geometry, QByteArray state){
	QtExt::Settings::write(geometry, state);
	QSettings settings( m_organization, m_appName );
}
