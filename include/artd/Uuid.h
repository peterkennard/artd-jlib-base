#ifndef __artd_Uuid_h
#define __artd_Uuid_h
/*-
 * Copyright (c) 1998-2022 Peter Kennard and aRt&D Lab
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of the source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Any redistribution solely in binary form must conspicuously
 *    reproduce the following disclaimer in documentation provided with the
 *    binary redistribution.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'', WITHOUT ANY WARRANTIES, EXPRESS
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  LICENSOR SHALL
 * NOT BE LIABLE FOR ANY LOSS OR DAMAGES RESULTING FROM THE USE OF THIS
 * SOFTWARE, EITHER ALONE OR IN COMBINATION WITH ANY OTHER SOFTWARE.
 * 
 */

#include "artd/jlib_base.h"
#include "artd/int_types.h"

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

class ARTD_API_JLIB_BASE Uuid
{
public:

	uint32_t    d1_;
	uint16_t    d2_;
	uint16_t    d3_;
	uint8_t     b_[8];

    // TODO: 64 bit compares for 64 bit compile
	bool operator == (const Uuid &b) const
	{   
		const uint32_t *ia = &d1_;
		const uint32_t *ib = &b.d1_;

		return(    (ia[3] == ib[3])
				&& (ia[2] == ib[2])
				&& (ia[1] == ib[1])
				&& (ia[0] == ib[0]) );
	}
	bool operator != (const Uuid &b) const
	{   
		const uint32_t *ia = &d1_;
		const uint32_t *ib = &b.d1_;

		return(    (ia[3] != ib[3])
				|| (ia[2] != ib[2])
				|| (ia[1] != ib[1])
				|| (ia[0] != ib[0]) );
	}
	bool operator < (const Uuid &b) const
	{   
		const uint32_t *ia = &d1_;
		const uint32_t *ib = &b.d1_;
		if(ia[3] != ib[3]) return(ia[3] < ib[3]);
		if(ia[2] != ib[2]) return(ia[2] < ib[2]);
		if(ia[1] != ib[1]) return(ia[1] < ib[1]);
		return(ia[0] < ib[0]);
	}
	// assignment
private:
	/** @brief compiler often moves each field individually and this is faster */
	INL void assign(const void *src) { class mem { protected: uint32_t d[4]; }; *((mem *)this) = *((mem *)src); }
public:
	/** @brief assignment operator */
	INL Uuid &operator =(const Uuid &src) { assign(&src); return(*this); }

	// for windows compatibility
	#ifdef GUID_DEFINED
		INL operator GUID &() { return(*reinterpret_cast<GUID *>(this)); }
		INL operator const GUID &() const { return(*reinterpret_cast<const GUID*>(this)); }
		INL Uuid &operator =(const GUID &src) { assign(&src); return(*this); }
	#endif

	/** @brief Size of buffer to contain text output by toText() including terminal 0 (null) */
	static const int TextSize = 37;

	/** formats text in "uuid format" of this.
	 * Formats this Uuid to standard string representation
	 * putting result into provided buffer.
	 * The result is null terminated.
	 * ie: "F3452518-ABCD-4105-9928-A18794D57BAF"
	 * size of text not including terminal null.
	 */
	int toText(char *buf) const;

	// returns < 0 if failure >= 0 if ok
	// TODO: implement
	int fromText(const char *txt);

	/** Translates from native to network byte order
     * Does not know state so will flip if on li'l endian box
	 */
	void toNetBytes();
	/** @brief  Translates this structure to network byte order to native
	 * Does not know state so will flip if on little endian box
	 */
	void fromNetBytes();
    // TODO: add generators
};

ARTD_END

#endif // __artd_Uuid_h

