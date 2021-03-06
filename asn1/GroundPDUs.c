/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "CPDLCAPDUsVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#include "GroundPDUs.h"

static asn_per_constraints_t asn_PER_type_GroundPDUs_constr_1 GCC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  3,  3,  0,  5 }	/* (0..5,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_GroundPDUs_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.abortUser),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CPDLCUserAbortReason,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"abortUser"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.abortProvider),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CPDLCProviderAbortReason,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"abortProvider"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.startup),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_UplinkMessage,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"startup"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.send),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ATCUplinkMessage,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"send"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.forward),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ATCForwardMessage,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"forward"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GroundPDUs, choice.forwardresponse),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ATCForwardResponse,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"forwardresponse"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_GroundPDUs_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* abortUser */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* abortProvider */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* startup */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* send */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* forward */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* forwardresponse */
};
static asn_CHOICE_specifics_t asn_SPC_GroundPDUs_specs_1 = {
	sizeof(struct GroundPDUs),
	offsetof(struct GroundPDUs, _asn_ctx),
	offsetof(struct GroundPDUs, present),
	sizeof(((struct GroundPDUs *)0)->present),
	asn_MAP_GroundPDUs_tag2el_1,
	6,	/* Count of tags in the map */
	0,
	6	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_GroundPDUs = {
	"GroundPDUs",
	"GroundPDUs",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	CHOICE_decode_uper,
	CHOICE_encode_uper,
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_type_GroundPDUs_constr_1,
	asn_MBR_GroundPDUs_1,
	6,	/* Elements count */
	&asn_SPC_GroundPDUs_specs_1	/* Additional specs */
};

