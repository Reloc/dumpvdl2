/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "PMCPDLCMessageSetVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_Speed_H_
#define	_Speed_H_


#include <asn_application.h>

/* Including external dependencies */
#include "SpeedIndicated.h"
#include "SpeedIndicatedMetric.h"
#include "SpeedTrue.h"
#include "SpeedTrueMetric.h"
#include "SpeedGround.h"
#include "SpeedGroundMetric.h"
#include "SpeedMach.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Speed_PR {
	Speed_PR_NOTHING,	/* No components present */
	Speed_PR_speedIndicated,
	Speed_PR_speedIndicatedMetric,
	Speed_PR_speedTrue,
	Speed_PR_speedTrueMetric,
	Speed_PR_speedGround,
	Speed_PR_speedGroundMetric,
	Speed_PR_speedMach
} Speed_PR;

/* Speed */
typedef struct Speed {
	Speed_PR present;
	union Speed_u {
		SpeedIndicated_t	 speedIndicated;
		SpeedIndicatedMetric_t	 speedIndicatedMetric;
		SpeedTrue_t	 speedTrue;
		SpeedTrueMetric_t	 speedTrueMetric;
		SpeedGround_t	 speedGround;
		SpeedGroundMetric_t	 speedGroundMetric;
		SpeedMach_t	 speedMach;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Speed_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Speed;

#ifdef __cplusplus
}
#endif

#endif	/* _Speed_H_ */
#include <asn_internal.h>