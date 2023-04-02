#include "artd/ArtdClassId.h"
#include "artd/pointer_math.h"
#include "artd/HexFormatter.h"
#include <stdlib.h>
#include <string.h>
#include "artd/RcArray.h"

ARTD_BEGIN


ArtdClassId &
ArtdClassId::operator=(const ArtdClassId &from) noexcept {
	if (this  != &from) {
		length_ = from.length_;
		::memcpy(bytes_,from.bytes_,sizeof(bytes_));
	} else {
		setNull();
	}
	return(*this);
}



ArtdClassId::ArtdClassId(int length, const char *bytes) {
	if (length > MaxLength) {
		length = MaxLength;
	}
	length_ = length;
	--bytes; // to compensate for extra first byte
	while (length > 0) {
		bytes_[length] = bytes[length];
		--length;
	}
	bytes_[0] = GlobalDomain;
}



void
ArtdClassId::setFromBytes(const void *data, int length) {
	if(length > MaxLength) {
		length = MaxLength;
	}
	length_ = length;
	::memcpy(&bytes_[1], data, length);
	bytes_[0] = GlobalDomain;
}

int
ArtdClassId::getBytes(void *data) const {
	::memcpy(data, &bytes_[1], length_);
	return(length_);
}

ByteArray 
ArtdClassId::getBytes() const {
	ByteArray ret(length_);
	getBytes(ret.data());
	return(ret);
}

int 
ArtdClassId::equals(const ArtdClassId &b) const {
	
	if(isNull(b)) {
		return(false);
	}
	int ret = 1;
	if (length_ == b.length_) {
		ret = ::memcmp(&bytes_, &b.bytes_, length_);
	}
	if(ret) {
		return(false);
	}
	return(true);
}
RcString
ArtdClassId::toString() const {
static RcString nul("[null]");

	return(length_ == 0 ? nul : HexFormatter::binToHex(&bytes_[1], length_));
}

ARTD_END
