#ifndef __artd_ArtdClassId_h
#define __artd_ArtdClassId_h

#include "artd/jlib_base.h"
#include "artd/int_types.h"
#include "artd/Uuid.h"
#include "artd/ObjectBase.h"
#include "artd/RcArray.h"


ARTD_BEGIN

class ARTD_API_JLIB_BASE ArtdClassId
{
	static const uint8_t NullDomain = 0xFF;

	ARTD_ALWAYS_INLINE void setNull() {
		length_ = 0;
		bytes_[0] = NullDomain;
	}

public:
	static const int MaxLength = 19;
	
	static const int GlobalDomain = 0;
	static const int AppLocalDomain   = 1;
    static const int ConnectionDomain = 2;

	ArtdClassId() {
		setNull();
	}

	//ArtdClassId(UuidRef uuid) {
	//	length_ = uuid->getBytes(&bytes_[1]);
	//	bytes_[0] = GlobalDomain;
	//}
	ArtdClassId(int length, const char *bytes);

	ArtdClassId(const ByteArray &a) {
		if (a) {
			setFromBytes(a.elements(), a.length());
		} else {
			setNull();
		}
	}
	ARTD_ALWAYS_INLINE ArtdClassId(const ArtdClassId &&from) noexcept {
		operator=(from);
	}
	ArtdClassId &operator=(const ArtdClassId &from) noexcept;
	~ArtdClassId() {
	}
	
	ARTD_ALWAYS_INLINE int length() const {
        return(length_);
    }
	ARTD_ALWAYS_INLINE int domain() const {
        return(bytes_[0]);
    }

	void setFromBytes(const void *data, int length);
	int equals(const ArtdClassId &b) const;

	ARTD_ALWAYS_INLINE int operator==(const ArtdClassId &b) const {
		return(equals(b));
	}
	ARTD_ALWAYS_INLINE int operator==(const ArtdClassId *b) const {
		return(equals(*b));
	}
	int getBytes(void *data) const;
	ByteArray getBytes() const;

	RcString toString() const;

protected:	

	unsigned int length_;
	uint8_t bytes_[MaxLength+1];

};

ARTD_END

#endif // __artd_ArtdClassId_h
