#include "artd/Uuid.h"
#include "artd/Formatf.h"

ARTD_BEGIN

int Uuid::toText(char *buf) const
{
	return(FormatfStream().sprintf(  buf, TextSize, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
									 d1_, d2_, d3_, b_[0], b_[1],
									 b_[2], b_[3], b_[4], b_[5], b_[6], b_[7] ));
}

ARTD_END
