#ifndef DWDDateTimeScaleEngineH
#define DWDDateTimeScaleEngineH

#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>

// re-implemented scale engine for month scales
class DWDDateTimeScaleEngine : public QwtLinearScaleEngine {
public:

	virtual QwtScaleDiv divideScale(double x1, double x2,
		int maxMajSteps, int maxMinSteps, double stepSize) const;
};



#endif // DWDDateTimeScaleEngineH
