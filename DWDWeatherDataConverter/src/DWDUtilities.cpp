#include "DWDUtilities.h"


unsigned int month(unsigned int day) {
	unsigned int k=0;
	while (k < 12 && day > dayAtMonthStart[k]) ++k;
	return k;
}
