/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "FANSACTwoWayDataLinkCommunications"
 * 	found in "fans-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_FANSPositionSpeed_H_
#define	_FANSPositionSpeed_H_


#include <asn_application.h>

/* Including external dependencies */
#include "FANSPosition.h"
#include "FANSSpeed.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FANSPositionSpeed */
typedef struct FANSPositionSpeed {
	FANSPosition_t	 position;
	FANSSpeed_t	 speed;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} FANSPositionSpeed_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_FANSPositionSpeed;

#ifdef __cplusplus
}
#endif

#endif	/* _FANSPositionSpeed_H_ */
#include <asn_internal.h>
