/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: smival.h
Description: SnmpSyntax∂®“Â 	 
Author: guanguochen       
Date: 2009.09.14
*****************************************************/
#if defined(WIN32)
#pragma once
#endif

#ifndef _SMIVALUE
#define _SMIVALUE

#include "smi.h"

//----[ macros ]-------------------------------------------------------
#if defined(USE_CPP_CASTS)
#define PP_CONST_CAST(___type, ___ptr)    const_cast< ___type >(___ptr)
#else
#define PP_CONST_CAST(___type, ___ptr)    ((___type)(___ptr))
#endif

//======================================================================
// SMI value structure conforming with SMI RFC
//
typedef struct 
{	/* smiVALUE portion of VarBind */
	SmiUINT32 syntax;		/* Insert SNMP_SYNTAX_<type> */
	union
	{
		SmiINT    sNumber;    /* SNMP_SYNTAX_INT
                                 SNMP_SYNTAX_INT32 */
		SmiUINT32 uNumber;    /* SNMP_SYNTAX_UINT32
                                 SNMP_SYNTAX_CNTR32
                                 SNMP_SYNTAX_GAUGE32
                                 SNMP_SYNTAX_TIMETICKS */
		SmiCNTR64 hNumber;    /* SNMP_SYNTAX_CNTR64 */
        SmiOCTETS string;     /* SNMP_SYNTAX_OCTETS
                                 SNMP_SYNTAX_BITS
                                 SNMP_SYNTAX_OPAQUE
                                 SNMP_SYNTAX_IPADDR
                                 SNMP_SYNTAX_NSAPADDR */
		SmiOID    oid;        /* SNMP_SYNTAX_OID */
		SmiBYTE   empty;      /* SNMP_SYNTAX_NULL
                                 SNMP_SYNTAX_NOSUCHOBJECT
                                 SNMP_SYNTAX_NOSUCHINSTANCE
                                 SNMP_SYNTAX_ENDOFMIBVIEW */
	}
	value;
} SmiVALUE, *SmiLPVALUE;

class SnmpSyntax
{
public:
	virtual const char *get_printable() const = 0;
	virtual SmiUINT32 get_syntax() const = 0;
	virtual  SnmpSyntax * clone() const = 0;
	virtual ~SnmpSyntax() {};
	virtual SnmpSyntax& operator=(const SnmpSyntax &/*val*/) { return *this; };
	virtual SmiVALUE& getvalue(void) { return smival; };
	virtual bool valid() const = 0;
	virtual int get_asn1_length() const = 0;
	virtual void clear() = 0;

protected:
	SmiVALUE smival;
};

typedef union vbvalue
{
	SmiINT    sNumber;    /* SNMP_SYNTAX_INT
						  SNMP_SYNTAX_INT32 */
	SmiUINT32 uNumber;    /* SNMP_SYNTAX_UINT32
						  SNMP_SYNTAX_CNTR32
						  SNMP_SYNTAX_GAUGE32
						  SNMP_SYNTAX_TIMETICKS */
	SmiCNTR64 hNumber;    /* SNMP_SYNTAX_CNTR64 */
	SmiOCTETS string;     /* SNMP_SYNTAX_OCTETS
						  SNMP_SYNTAX_BITS
						  SNMP_SYNTAX_OPAQUE
						  SNMP_SYNTAX_IPADDR
						  SNMP_SYNTAX_NSAPADDR */
	SmiOID    oid;        /* SNMP_SYNTAX_OID */
	SmiBYTE   empty;      /* SNMP_SYNTAX_NULL
						  SNMP_SYNTAX_NOSUCHOBJECT
						  SNMP_SYNTAX_NOSUCHINSTANCE
						  SNMP_SYNTAX_ENDOFMIBVIEW */
} VBVALUE, *PVBVALUE;



#endif  // _SMIVALUE
