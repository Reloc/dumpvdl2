/*
 *  This file is a part of dumpvdl2
 *
 *  Copyright (c) 2017-2018 Tomasz Lemiech <szpajder@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asn1/FANSATCDownlinkMessage.h"	// FANSATCDownlinkMessage_t and dependencies
#include "asn1/FANSATCUplinkMessage.h"		// FANSATCUplinkMessage_t and dependencies
#include "tlv.h"				// dict_search()
#include "dumpvdl2.h"				// XCALLOC
#include "asn1-util.h"				// asn_formatter_t, asn1_output()
#include "asn1-format-common.h"			// common formatters and helper functions

// forward declaration
void asn1_output_cpdlc(FILE *stream, asn_TYPE_descriptor_t *td, const void *sptr, int indent);

static dict const FANSATCUplinkMsgElementId_labels[] = {
	{ FANSATCUplinkMsgElementId_PR_uM0NULL, "UNABLE" },
	{ FANSATCUplinkMsgElementId_PR_uM1NULL, "STANDBY" },
	{ FANSATCUplinkMsgElementId_PR_uM2NULL, "REQUEST DEFERRED" },
	{ FANSATCUplinkMsgElementId_PR_uM3NULL, "ROGER" },
	{ FANSATCUplinkMsgElementId_PR_uM4NULL, "AFFIRM" },
	{ FANSATCUplinkMsgElementId_PR_uM5NULL, "NEGATIVE" },
	{ FANSATCUplinkMsgElementId_PR_uM6Altitude, "EXPECT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM7Time, "EXPECT CLIMB AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM8Position, "EXPECT CLIMB AT [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM9Time, "EXPECT DESCENT AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM10Position, "EXPECT DESCENT AT [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM11Time, "EXPECT CRUISE CLIMB AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM12Position, "EXPECT CRUISE CLIMB AT [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM13TimeAltitude, "AT [time] EXPECT CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM14PositionAltitude, "AT [position] EXPECT CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM15TimeAltitude, "AT [time] EXPECT DESCENT TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM16PositionAltitude, "AT [position] EXPECT DESCENT TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM17TimeAltitude, "AT [time] EXPECT CRUISE CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM18PositionAltitude, "AT [position] EXPECT CRUISE CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM19Altitude, "MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM20Altitude, "CLIMB TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM21TimeAltitude, "AT [time] CLIMB TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM22PositionAltitude, "AT [position] CLIMB TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM23Altitude, "DESCEND TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM24TimeAltitude, "AT [time] DESCEND TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM25PositionAltitude, "AT [position] DESCEND TO AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM26AltitudeTime, "CLIMB TO REACH [altitude] BY [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM27AltitudePosition, "CLIMB TO REACH [altitude] BY [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM28AltitudeTime, "DESCEND TO REACH [altitude] BY [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM29AltitudePosition, "DESCEND TO REACH [altitude] BY [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM30AltitudeAltitude, "MAINTAIN BLOCK [altitude] TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM31AltitudeAltitude, "CLIMB TO AND MAINTAIN BLOCK [altitude] TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM32AltitudeAltitude, "DESCEND TO AND MAINTAIN BLOCK [altitude] TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM33Altitude, "CRUISE [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM34Altitude, "CRUISE CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM35Altitude, "CRUISE CLIMB ABOVE [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM36Altitude, "EXPEDITE CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM37Altitude, "EXPEDITE DESCENT TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM38Altitude, "IMMEDIATELY CLIMB TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM39Altitude, "IMMEDIATELY DESCEND TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM40Altitude, "IMMEDIATELY STOP CLIMB AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM41Altitude, "IMMEDIATELY STOP DESCENT AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM42PositionAltitude, "EXPECT TO CROSS [position] AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM43PositionAltitude, "EXPECT TO CROSS [position] AT OR ABOVE [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM44PositionAltitude, "EXPECT TO CROSS [position] AT OR BELOW [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM45PositionAltitude, "EXPECT TO CROSS [position] AT AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM46PositionAltitude, "CROSS [position] AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM47PositionAltitude, "CROSS [position] AT OR ABOVE [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM48PositionAltitude, "CROSS [position] AT OR BELOW [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM49PositionAltitude, "CROSS [position] AT AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM50PositionAltitudeAltitude, "CROSS [position] BETWEEN [altitude] AND [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM51PositionTime, "CROSS [position] AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM52PositionTime, "CROSS [position] AT OR BEFORE [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM53PositionTime, "CROSS [position] AT OR AFTER [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM54PositionTimeTime, "CROSS [position] BETWEEN [time] AND [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM55PositionSpeed, "CROSS [position] AT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM56PositionSpeed, "CROSS [position] AT OR LESS THAN [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM57PositionSpeed, "CROSS [position] AT OR GREATER THAN [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM58PositionTimeAltitude, "CROSS [position] AT [time] AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM59PositionTimeAltitude, "CROSS [position] AT OR BEFORE [time] AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM60PositionTimeAltitude, "CROSS [position] AT OR AFTER [time] AT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM61PositionAltitudeSpeed, "CROSS [position] AT AND MAINTAIN [altitude] AT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM62TimePositionAltitude, "AT [time] CROSS [position] AT AND MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM63TimePositionAltitudeSpeed, "AT [time] CROSS [position] AT AND MAINTAIN [altitude] AT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM64DistanceOffsetDirection, "OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM65PositionDistanceOffsetDirection, "AT [position] OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM66TimeDistanceOffsetDirection, "AT [time] OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM67NULL, "PROCEED BACK ON ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM68Position, "REJOIN ROUTE BY [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM69Time, "REJOIN ROUTE BY [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM70Position, "EXPECT BACK ON ROUTE BY [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM71Time, "EXPECT BACK ON ROUTE BY [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM72NULL, "RESUME OWN NAVIGATION" },
	{ FANSATCUplinkMsgElementId_PR_uM73Predepartureclearance, "[predepartureclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM74Position, "PROCEED DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM75Position, "WHEN ABLE PROCEED DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM76TimePosition, "AT [time] PROCEED DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM77PositionPosition, "AT [position] PROCEED DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM78AltitudePosition, "AT [altitude] PROCEED DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM79PositionRouteClearance, "CLEARED TO [position] VIA [routeclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM80RouteClearance, "CLEARED [routeclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM81ProcedureName, "CLEARED [procedurename]" },
	{ FANSATCUplinkMsgElementId_PR_uM82DistanceOffsetDirection, "CLEARED TO DEVIATE UP TO [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM83PositionRouteClearance, "AT [position] CLEARED [routeclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM84PositionProcedureName, "AT [position] CLEARED [procedurename]" },
	{ FANSATCUplinkMsgElementId_PR_uM85RouteClearance, "EXPECT [routeclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM86PositionRouteClearance, "AT [position] EXPECT [routeclearance]" },
	{ FANSATCUplinkMsgElementId_PR_uM87Position, "EXPECT DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM88PositionPosition, "AT [position] EXPECT DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM89TimePosition, "AT [time] EXPECT DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM90AltitudePosition, "AT [altitude] EXPECT DIRECT TO [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM91HoldClearance, "HOLD AT [position] MAINTAIN [altitude] INBOUND TRACK [degrees] [direction] TURNS [legtype]" },
	{ FANSATCUplinkMsgElementId_PR_uM92PositionAltitude, "HOLD AT [position] AS PUBLISHED MAINTAIN [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM93Time, "EXPECT FURTHER CLEARANCE AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM94DirectionDegrees, "TURN [direction] HEADING [degrees]" },
	{ FANSATCUplinkMsgElementId_PR_uM95DirectionDegrees, "TURN [direction] GROUND TRACK [degrees]" },
	{ FANSATCUplinkMsgElementId_PR_uM96NULL, "FLY PRESENT HEADING" },
	{ FANSATCUplinkMsgElementId_PR_uM97PositionDegrees, "AT [position] FLY HEADING [degrees]" },
	{ FANSATCUplinkMsgElementId_PR_uM98DirectionDegrees, "IMMEDIATELY TURN [direction] HEADING [degrees]" },
	{ FANSATCUplinkMsgElementId_PR_uM99ProcedureName, "EXPECT [procedurename]" },
	{ FANSATCUplinkMsgElementId_PR_uM100TimeSpeed, "AT [time] EXPECT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM101PositionSpeed, "AT [position] EXPECT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM102AltitudeSpeed, "AT [altitude] EXPECT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM103TimeSpeedSpeed, "AT [time] EXPECT [speed] TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM104PositionSpeedSpeed, "AT [position] EXPECT [speed] TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM105AltitudeSpeedSpeed, "AT [altitude] EXPECT [speed] TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM106Speed, "MAINTAIN [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM107NULL, "MAINTAIN PRESENT SPEED" },
	{ FANSATCUplinkMsgElementId_PR_uM108Speed, "MAINTAIN [speed] OR GREATER" },
	{ FANSATCUplinkMsgElementId_PR_uM109Speed, "MAINTAIN [speed] OR LESS" },
	{ FANSATCUplinkMsgElementId_PR_uM110SpeedSpeed, "MAINTAIN [speed] TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM111Speed, "INCREASE SPEED TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM112Speed, "INCREASE SPEED TO [speed] OR GREATER" },
	{ FANSATCUplinkMsgElementId_PR_uM113Speed, "REDUCE SPEED TO [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM114Speed, "REDUCE SPEED TO [speed] OR LESS" },
	{ FANSATCUplinkMsgElementId_PR_uM115Speed, "DO NOT EXCEED [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM116NULL, "RESUME NORMAL SPEED" },
	{ FANSATCUplinkMsgElementId_PR_uM117ICAOunitnameFrequency, "CONTACT [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM118PositionICAOunitnameFrequency, "AT [position] CONTACT [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM119TimeICAOunitnameFrequency, "AT [time] CONTACT [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM120ICAOunitnameFrequency, "MONITOR [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM121PositionICAOunitnameFrequency, "AT [position] MONITOR [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM122TimeICAOunitnameFrequency, "AT [time] MONITOR [icaounitname] [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM123BeaconCode, "SQUAWK [beaconcode]" },
	{ FANSATCUplinkMsgElementId_PR_uM124NULL, "STOP SQUAWK" },
	{ FANSATCUplinkMsgElementId_PR_uM125NULL, "SQUAWK ALTITUDE" },
	{ FANSATCUplinkMsgElementId_PR_uM126NULL, "STOP ALTITUDE SQUAWK" },
	{ FANSATCUplinkMsgElementId_PR_uM127NULL, "REPORT BACK ON ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM128Altitude, "REPORT LEAVING [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM129Altitude, "REPORT LEVEL [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM130Position, "REPORT PASSING [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM131NULL, "REPORT REMAINING FUEL AND SOULS ON BOARD" },
	{ FANSATCUplinkMsgElementId_PR_uM132NULL, "CONFIRM POSITION" },
	{ FANSATCUplinkMsgElementId_PR_uM133NULL, "CONFIRM ALTITUDE" },
	{ FANSATCUplinkMsgElementId_PR_uM134NULL, "CONFIRM SPEED" },
	{ FANSATCUplinkMsgElementId_PR_uM135NULL, "CONFIRM ASSIGNED ALTITUDE" },
	{ FANSATCUplinkMsgElementId_PR_uM136NULL, "CONFIRM ASSIGNED SPEED" },
	{ FANSATCUplinkMsgElementId_PR_uM137NULL, "CONFIRM ASSIGNED ROUTE" },
	{ FANSATCUplinkMsgElementId_PR_uM138NULL, "CONFIRM TIME OVER REPORTED WAYPOINT" },
	{ FANSATCUplinkMsgElementId_PR_uM139NULL, "CONFIRM REPORTED WAYPOINT" },
	{ FANSATCUplinkMsgElementId_PR_uM140NULL, "CONFIRM NEXT WAYPOINT" },
	{ FANSATCUplinkMsgElementId_PR_uM141NULL, "CONFIRM NEXT WAYPOINT ETA" },
	{ FANSATCUplinkMsgElementId_PR_uM142NULL, "CONFIRM ENSUING WAYPOINT" },
	{ FANSATCUplinkMsgElementId_PR_uM143NULL, "CONFIRM REQUEST" },
	{ FANSATCUplinkMsgElementId_PR_uM144NULL, "CONFIRM SQUAWK" },
	{ FANSATCUplinkMsgElementId_PR_uM145NULL, "CONFIRM HEADING" },
	{ FANSATCUplinkMsgElementId_PR_uM146NULL, "CONFIRM GROUND TRACK" },
	{ FANSATCUplinkMsgElementId_PR_uM147NULL, "REQUEST POSITION REPORT" },
	{ FANSATCUplinkMsgElementId_PR_uM148Altitude, "WHEN CAN YOU ACCEPT [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM149AltitudePosition, "CAN YOU ACCEPT [altitude] AT [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM150AltitudeTime, "CAN YOU ACCEPT [altitude] AT [time]" },
	{ FANSATCUplinkMsgElementId_PR_uM151Speed, "WHEN CAN YOU ACCEPT [speed]" },
	{ FANSATCUplinkMsgElementId_PR_uM152DistanceOffsetDirection, "WHEN CAN YOU ACCEPT [distanceoffset] [direction] OFFSET" },
	{ FANSATCUplinkMsgElementId_PR_uM153Altimeter, "ALTIMETER [altimeter]" },
	{ FANSATCUplinkMsgElementId_PR_uM154NULL, "RADAR SERVICES TERMINATED" },
	{ FANSATCUplinkMsgElementId_PR_uM155Position, "RADAR CONTACT [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM156NULL, "RADAR CONTACT LOST" },
	{ FANSATCUplinkMsgElementId_PR_uM157Frequency, "CHECK STUCK MICROPHONE [frequency]" },
	{ FANSATCUplinkMsgElementId_PR_uM158ATISCode, "ATIS [atiscode]" },
	{ FANSATCUplinkMsgElementId_PR_uM159ErrorInformation, "ERROR [errorinformation]" },
	{ FANSATCUplinkMsgElementId_PR_uM160ICAOfacilitydesignation, "NEXT DATA AUTHORITY [icaofacilitydesignation]" },
	{ FANSATCUplinkMsgElementId_PR_uM161NULL, "END SERVICE" },
	{ FANSATCUplinkMsgElementId_PR_uM162NULL, "SERVICE UNAVAILABLE" },
	{ FANSATCUplinkMsgElementId_PR_uM163ICAOfacilitydesignationTp4table, "[icaofacilitydesignation] [tp4table]" },
	{ FANSATCUplinkMsgElementId_PR_uM164NULL, "WHEN READY" },
	{ FANSATCUplinkMsgElementId_PR_uM165NULL, "THEN" },
	{ FANSATCUplinkMsgElementId_PR_uM166NULL, "DUE TO TRAFFIC" },
	{ FANSATCUplinkMsgElementId_PR_uM167NULL, "DUE TO AIRSPACE RESTRICTION" },
	{ FANSATCUplinkMsgElementId_PR_uM168NULL, "DISREGARD" },
	{ FANSATCUplinkMsgElementId_PR_uM169FreeText, "[freetext]" },
	{ FANSATCUplinkMsgElementId_PR_uM170FreeText, "[freetext]" },
	{ FANSATCUplinkMsgElementId_PR_uM171VerticalRate, "CLIMB AT [verticalrate] MINIMUM" },
	{ FANSATCUplinkMsgElementId_PR_uM172VerticalRate, "CLIMB AT [verticalrate] MAXIMUM" },
	{ FANSATCUplinkMsgElementId_PR_uM173VerticalRate, "DESCEND AT [verticalrate] MINIMUM" },
	{ FANSATCUplinkMsgElementId_PR_uM174VerticalRate, "DESCEND AT [verticalrate] MAXIMUM" },
	{ FANSATCUplinkMsgElementId_PR_uM175Altitude, "REPORT REACHING [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM176NULL, "MAINTAIN OWN SEPARATION AND VMC" },
	{ FANSATCUplinkMsgElementId_PR_uM177NULL, "AT PILOTS DISCRETION" },
	{ FANSATCUplinkMsgElementId_PR_uM178NULL, "[trackdetailmsg-deleted]" },
	{ FANSATCUplinkMsgElementId_PR_uM179NULL, "SQUAWK IDENT" },
	{ FANSATCUplinkMsgElementId_PR_uM180AltitudeAltitude, "REPORT REACHING BLOCK [altitude] TO [altitude]" },
	{ FANSATCUplinkMsgElementId_PR_uM181ToFromPosition, "REPORT DISTANCE [tofrom] [position]" },
	{ FANSATCUplinkMsgElementId_PR_uM182NULL, "CONFIRM ATIS CODE" },
	{ 0, NULL }
};

static dict const FANSATCDownlinkMsgElementId_labels[] = {
	{ FANSATCDownlinkMsgElementId_PR_dM0NULL, "WILCO" },
	{ FANSATCDownlinkMsgElementId_PR_dM1NULL, "UNABLE" },
	{ FANSATCDownlinkMsgElementId_PR_dM2NULL, "STANDBY" },
	{ FANSATCDownlinkMsgElementId_PR_dM3NULL, "ROGER" },
	{ FANSATCDownlinkMsgElementId_PR_dM4NULL, "AFFIRM" },
	{ FANSATCDownlinkMsgElementId_PR_dM5NULL, "NEGATIVE" },
	{ FANSATCDownlinkMsgElementId_PR_dM6Altitude, "REQUEST [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM7AltitudeAltitude, "REQUEST BLOCK [altitude] TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM8Altitude, "REQUEST CRUISE CLIMB TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM9Altitude, "REQUEST CLIMB TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM10Altitude, "REQUEST DESCENT TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM11PositionAltitude, "AT [position] REQUEST CLIMB TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM12PositionAltitude, "AT [position] REQUEST DESCENT TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM13TimeAltitude, "AT [time] REQUEST CLIMB TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM14TimeAltitude, "AT [time] REQUEST DESCENT TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM15DistanceOffsetDirection, "REQUEST OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM16PositionDistanceOffsetDirection, "AT [position] REQUEST OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM17TimeDistanceOffsetDirection, "AT [time] REQUEST OFFSET [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM18Speed, "REQUEST [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM19SpeedSpeed, "REQUEST [speed] TO [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM20NULL, "REQUEST VOICE CONTACT" },
	{ FANSATCDownlinkMsgElementId_PR_dM21Frequency, "REQUEST VOICE CONTACT [frequency]" },
	{ FANSATCDownlinkMsgElementId_PR_dM22Position, "REQUEST DIRECT TO [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM23ProcedureName, "REQUEST [procedurename]" },
	{ FANSATCDownlinkMsgElementId_PR_dM24RouteClearance, "REQUEST [routeclearance]" },
	{ FANSATCDownlinkMsgElementId_PR_dM25NULL, "REQUEST CLEARANCE" },
	{ FANSATCDownlinkMsgElementId_PR_dM26PositionRouteClearance, "REQUEST WEATHER DEVIATION TO [position] VIA [routeclearance]" },
	{ FANSATCDownlinkMsgElementId_PR_dM27DistanceOffsetDirection, "REQUEST WEATHER DEVIATION UP TO [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM28Altitude, "LEAVING [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM29Altitude, "CLIMBING TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM30Altitude, "DESCENDING TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM31Position, "PASSING [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM32Altitude, "PRESENT ALTITUDE [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM33Position, "PRESENT POSITION [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM34Speed, "PRESENT SPEED [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM35Degrees, "PRESENT HEADING [degrees]" },
	{ FANSATCDownlinkMsgElementId_PR_dM36Degrees, "PRESENT GROUND TRACK [degrees]" },
	{ FANSATCDownlinkMsgElementId_PR_dM37Altitude, "LEVEL [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM38Altitude, "ASSIGNED ALTITUDE [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM39Speed, "ASSIGNED SPEED [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM40RouteClearance, "ASSIGNED ROUTE [routeclearance]" },
	{ FANSATCDownlinkMsgElementId_PR_dM41NULL, "BACK ON ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM42Position, "NEXT WAYPOINT [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM43Time, "NEXT WAYPOINT ETA [time]" },
	{ FANSATCDownlinkMsgElementId_PR_dM44Position, "ENSUING WAYPOINT [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM45Position, "REPORTED WAYPOINT [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM46Time, "REPORTED WAYPOINT [time]" },
	{ FANSATCDownlinkMsgElementId_PR_dM47BeaconCode, "SQUAWKING [beaconcode]" },
	{ FANSATCDownlinkMsgElementId_PR_dM48PositionReport, "POSITION REPORT [positionreport]" },
	{ FANSATCDownlinkMsgElementId_PR_dM49Speed, "WHEN CAN WE EXPECT [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM50SpeedSpeed, "WHEN CAN WE EXPECT [speed] TO [speed]" },
	{ FANSATCDownlinkMsgElementId_PR_dM51NULL, "WHEN CAN WE EXPECT BACK ON ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM52NULL, "WHEN CAN WE EXPECT LOWER ALTITUDE" },
	{ FANSATCDownlinkMsgElementId_PR_dM53NULL, "WHEN CAN WE EXPECT HIGHER ALTITUDE" },
	{ FANSATCDownlinkMsgElementId_PR_dM54Altitude, "WHEN CAN WE EXPECT CRUISE CLIMB TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM55NULL, "PAN PAN PAN" },
	{ FANSATCDownlinkMsgElementId_PR_dM56NULL, "MAYDAY MAYDAY MAYDAY" },
	{ FANSATCDownlinkMsgElementId_PR_dM57RemainingFuelRemainingSouls, "[remainingfuel] OF FUEL REMAINING AND [remainingsouls] SOULS ON BOARD" },
	{ FANSATCDownlinkMsgElementId_PR_dM58NULL, "CANCEL EMERGENCY" },
	{ FANSATCDownlinkMsgElementId_PR_dM59PositionRouteClearance, "DIVERTING TO [position] VIA [routeclearance]" },
	{ FANSATCDownlinkMsgElementId_PR_dM60DistanceOffsetDirection, "OFFSETTING [distanceoffset] [direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM61Altitude, "DESCENDING TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM62ErrorInformation, "ERROR [errorinformation]" },
	{ FANSATCDownlinkMsgElementId_PR_dM63NULL, "NOT CURRENT DATA AUTHORITY" },
	{ FANSATCDownlinkMsgElementId_PR_dM64ICAOfacilitydesignation, "[icaofacilitydesignation]" },
	{ FANSATCDownlinkMsgElementId_PR_dM65NULL, "DUE TO WEATHER" },
	{ FANSATCDownlinkMsgElementId_PR_dM66NULL, "DUE TO AIRCRAFT PERFORMANCE" },
	{ FANSATCDownlinkMsgElementId_PR_dM67FreeText, "[freetext]" },
	{ FANSATCDownlinkMsgElementId_PR_dM68FreeText, "[freetext]" },
	{ FANSATCDownlinkMsgElementId_PR_dM69NULL, "REQUEST VMC DESCENT" },
	{ FANSATCDownlinkMsgElementId_PR_dM70Degrees, "REQUEST HEADING [degrees]" },
	{ FANSATCDownlinkMsgElementId_PR_dM71Degrees, "REQUEST GROUND TRACK [degrees]" },
	{ FANSATCDownlinkMsgElementId_PR_dM72Altitude, "REACHING [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM73VersionNumber, "[versionnumber]" },
	{ FANSATCDownlinkMsgElementId_PR_dM74NULL, "MAINTAIN OWN SEPARATION AND VMC" },
	{ FANSATCDownlinkMsgElementId_PR_dM75NULL, "AT PILOTS DISCRETION" },
	{ FANSATCDownlinkMsgElementId_PR_dM76AltitudeAltitude, "REACHING BLOCK [altitude] TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM77AltitudeAltitude, "ASSIGNED BLOCK [altitude] TO [altitude]" },
	{ FANSATCDownlinkMsgElementId_PR_dM78TimeDistanceToFromPosition, "AT [time] [distance] [tofrom] [position]" },
	{ FANSATCDownlinkMsgElementId_PR_dM79ATISCode, "ATIS [atiscode]" },
	{ FANSATCDownlinkMsgElementId_PR_dM80DistanceOffsetDirection, "DEVIATING [distanceoffset][direction] OF ROUTE" },
	{ FANSATCDownlinkMsgElementId_PR_dM81NULL, "(reserved: dM81NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM82NULL, "(reserved: dM82NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM83NULL, "(reserved: dM83NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM84NULL, "(reserved: dM84NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM85NULL, "(reserved: dM85NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM86NULL, "(reserved: dM86NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM87NULL, "(reserved: dM87NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM88NULL, "(reserved: dM88NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM89NULL, "(reserved: dM89NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM90NULL, "(reserved: dM90NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM91NULL, "(reserved: dM91NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM92NULL, "(reserved: dM92NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM93NULL, "(reserved: dM93NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM94NULL, "(reserved: dM94NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM95NULL, "(reserved: dM95NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM96NULL, "(reserved: dM96NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM97NULL, "(reserved: dM97NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM98NULL, "(reserved: dM98NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM99NULL, "(reserved: dM99NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM100NULL, "(reserved: dM100NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM101NULL, "(reserved: dM101NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM102NULL, "(reserved: dM102NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM103NULL, "(reserved: dM103NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM104NULL, "(reserved: dM104NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM105NULL, "(reserved: dM105NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM106NULL, "(reserved: dM106NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM107NULL, "(reserved: dM107NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM108NULL, "(reserved: dM108NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM109NULL, "(reserved: dM109NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM110NULL, "(reserved: dM110NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM111NULL, "(reserved: dM111NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM112NULL, "(reserved: dM112NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM113NULL, "(reserved: dM113NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM114NULL, "(reserved: dM114NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM115NULL, "(reserved: dM115NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM116NULL, "(reserved: dM116NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM117NULL, "(reserved: dM117NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM118NULL, "(reserved: dM118NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM119NULL, "(reserved: dM119NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM120NULL, "(reserved: dM120NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM121NULL, "(reserved: dM121NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM122NULL, "(reserved: dM122NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM123NULL, "(reserved: dM123NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM124NULL, "(reserved: dM124NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM125NULL, "(reserved: dM125NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM126NULL, "(reserved: dM126NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM127NULL, "(reserved: dM127NULL)" },
	{ FANSATCDownlinkMsgElementId_PR_dM128NULL, "(reserved: dM128NULL)" },
	{ 0, NULL }
};

/************************
 * ASN.1 type formatters
 ************************/

static ASN1_FORMATTER_PROTOTYPE(asn1_format_CHOICE_cpdlc) {
	_format_CHOICE(stream, label, NULL, &asn1_output_cpdlc, td, sptr, indent);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_SEQUENCE_cpdlc) {
	_format_SEQUENCE(stream, label, &asn1_output_cpdlc, td, sptr, indent);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_SEQUENCE_OF_cpdlc) {
	_format_SEQUENCE_OF(stream, label, &asn1_output_cpdlc, td, sptr, indent);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSAltimeterEnglish) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " inHg", 0.01, 2);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSAltimeterMetric) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " hPa", 0.1, 1);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSAltitudeGNSSFeet) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " ft", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSAltitudeFlightLevelMetric) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " m", 10, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSDistanceOffsetNm) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " nm", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSDistanceMetric) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " km", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSFeetX10) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " ft", 10, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSFrequencyhf) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " kHz", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSFrequencykHzToMHz) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " MHz", 0.001, 3);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSDistanceEnglish) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " nm", 0.1, 1);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSLegTime) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " min", 0.1, 1);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSMeters) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " m", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSTemperatureC) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " C", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSTemperatureF) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " F", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSWindSpeedEnglish) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " kts", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSWindSpeedMetric) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " km/h", 1, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSRTATolerance) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " min", 0.1, 1);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSSpeedEnglishX10) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " kts", 10, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSSpeedMetricX10) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " km/h", 10, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSSpeedMach) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, "", 0.01, 2);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSVerticalRateEnglish) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " ft/min", 100, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSVerticalRateMetric) {
	_format_INTEGER_with_unit(stream, label, td, sptr, indent, " m/min", 10, 0);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSBeaconCode) {
	CAST_PTR(code, FANSBeaconCode_t *, sptr);
	long **cptr = code->list.array;
	IFPRINTF(stream, indent, "%s: %ld%ld%ld%ld\n",
		label,
		*cptr[0],
		*cptr[1],
		*cptr[2],
		*cptr[3]
	);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSTime) {
	CAST_PTR(t, FANSTime_t *, sptr);
	IFPRINTF(stream, indent, "%s: %02ld:%02ld\n", label, t->hours, t->minutes);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSTimestamp) {
	CAST_PTR(t, FANSTimestamp_t *, sptr);
	IFPRINTF(stream, indent, "%s: %02ld:%02ld:%02ld\n", label, t->hours, t->minutes, t->seconds);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSLatitude) {
	CAST_PTR(lat, FANSLatitude_t *, sptr);
	long const ldir = lat->latitudeDirection;
	char const *ldir_name = value2enum(&asn_DEF_FANSLatitudeDirection, ldir);
	if(lat->minutesLatLon != NULL) {
		IFPRINTF(stream, indent, "%s:   %02ld %04.1f' %s\n",
			label,
			lat->latitudeDegrees,
			*(long const *)(lat->minutesLatLon) / 10.0,
			ldir_name
		);
	} else {
		IFPRINTF(stream, indent, "%s:   %02ld deg %s\n",
			label,
			lat->latitudeDegrees,
			ldir_name
		);
	}
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSLongitude) {
	CAST_PTR(lat, FANSLongitude_t *, sptr);
	long const ldir = lat->longitudeDirection;
	char const *ldir_name = value2enum(&asn_DEF_FANSLongitudeDirection, ldir);
	if(lat->minutesLatLon != NULL) {
		IFPRINTF(stream, indent, "%s: %03ld %04.1f' %s\n",
			label,
			lat->longitudeDegrees,
			*(long const *)(lat->minutesLatLon) / 10.0,
			ldir_name
		);
	} else {
		IFPRINTF(stream, indent, "%s: %03ld deg %s\n",
			label,
			lat->longitudeDegrees,
			ldir_name
		);
	}
}

// Can't replace this with asn1_format_SEQUENCE_cpdlc, because the first msg element
// is not a part of a SEQ-OF, hence we don't have any data type which we could
// associate "Message data:" label with (nor we can't use FANSATCUplinkMsgElementId
// for that because the same type is used inside the SEQ-OF which would cause
// the label to be printed for each element in the sequence). The same applies
// to asn1_format_FANSATCDownlinkMessage.
static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSATCUplinkMessage) {
	CAST_PTR(msg, FANSATCUplinkMessage_t *, sptr);
	if(label != NULL) {
		IFPRINTF(stream, indent, "%s:\n", label);
		indent++;
	}
	asn1_output_cpdlc(stream, &asn_DEF_FANSATCMessageHeader, &msg->aTCMessageheader, indent);
	IFPRINTF(stream, indent, "%s:\n", "Message data");
	indent++;
	asn1_output_cpdlc(stream, &asn_DEF_FANSATCUplinkMsgElementId, &msg->aTCuplinkmsgelementId, indent);
	if(msg->aTCuplinkmsgelementid_seqOf != NULL) {
		asn1_output_cpdlc(stream, &asn_DEF_FANSATCUplinkMsgElementIdSequence, msg->aTCuplinkmsgelementid_seqOf, indent);
	}
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSATCDownlinkMessage) {
	CAST_PTR(msg, FANSATCDownlinkMessage_t *, sptr);
	if(label != NULL) {
		IFPRINTF(stream, indent, "%s:\n", label);
		indent++;
	}
	asn1_output_cpdlc(stream, &asn_DEF_FANSATCMessageHeader, &msg->aTCMessageheader, indent);
	IFPRINTF(stream, indent, "%s:\n", "Message data");
	indent++;
	asn1_output_cpdlc(stream, &asn_DEF_FANSATCDownlinkMsgElementId, &msg->aTCDownlinkmsgelementid, indent);
	if(msg->aTCdownlinkmsgelementid_seqOf != NULL) {
		asn1_output_cpdlc(stream, &asn_DEF_FANSATCDownlinkMsgElementIdSequence, msg->aTCdownlinkmsgelementid_seqOf, indent);
	}
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSATCDownlinkMsgElementId) {
	_format_CHOICE(stream, label, FANSATCDownlinkMsgElementId_labels, &asn1_output_cpdlc, td, sptr, indent);
}

static ASN1_FORMATTER_PROTOTYPE(asn1_format_FANSATCUplinkMsgElementId) {
	_format_CHOICE(stream, label, FANSATCUplinkMsgElementId_labels, &asn1_output_cpdlc, td, sptr, indent);
}

static asn_formatter_t const asn1_cpdlc_formatter_table[] = {
	{ .type = &asn_DEF_FANSAircraftEquipmentCode, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAircraftFlightIdentification, .format = &asn1_format_any, .label = "Flight ID" },
	{ .type = &asn_DEF_FANSAircraftType, .format = &asn1_format_any, .label = "Aircraft type" },
	{ .type = &asn_DEF_FANSAirport, .format = &asn1_format_any, .label = "Airport" },
	{ .type = &asn_DEF_FANSAirportDeparture, .format = asn1_format_any, .label = "Departure airport" },
	{ .type = &asn_DEF_FANSAirportDestination, .format = asn1_format_any, .label = "Destination airport" },
	{ .type = &asn_DEF_FANSAirwayIdentifier, .format = &asn1_format_any, .label = "Airway ID" },
	{ .type = &asn_DEF_FANSAirwayIntercept, .format = &asn1_format_any, .label = "Airway intercept" },
	{ .type = &asn_DEF_FANSAltimeter, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltimeterEnglish, .format = &asn1_format_FANSAltimeterEnglish, .label = "Altimeter" },
	{ .type = &asn_DEF_FANSAltimeterMetric, .format = &asn1_format_FANSAltimeterMetric, .label = "Altimeter" },
	{ .type = &asn_DEF_FANSAltitude, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltitudeAltitude, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltitudeFlightLevel, .format = &asn1_format_any, .label = "Flight level" },
	{ .type = &asn_DEF_FANSAltitudeFlightLevelMetric, .format = &asn1_format_FANSAltitudeFlightLevelMetric, .label = "Flight level" },
	{ .type = &asn_DEF_FANSAltitudeGNSSFeet, .format = &asn1_format_FANSAltitudeGNSSFeet, .label = "Altitude (GNSS)" },
	{ .type = &asn_DEF_FANSAltitudeGNSSMeters, .format = &asn1_format_FANSMeters, .label = "Altitude (GNSS)" },
	{ .type = &asn_DEF_FANSAltitudePosition, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltitudeQFE, .format = &asn1_format_FANSFeetX10, .label = "Altitude (QFE)" },
	{ .type = &asn_DEF_FANSAltitudeQFEMeters, .format = &asn1_format_FANSMeters, .label = "Altitude (QFE)" },
	{ .type = &asn_DEF_FANSAltitudeQNH, .format = &asn1_format_FANSFeetX10, .label = "Altitude (QNH)" },
	{ .type = &asn_DEF_FANSAltitudeQNHMeters, .format = &asn1_format_FANSMeters, .label = "Altitude (QNH)" },
	{ .type = &asn_DEF_FANSAltitudeRestriction, .format = asn1_format_CHOICE_cpdlc, .label = "Altitude restriction" },
	{ .type = &asn_DEF_FANSAltitudeSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltitudeSpeedSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSAltitudeTime, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATCDownlinkMessage, .format = &asn1_format_FANSATCDownlinkMessage, .label = "CPDLC Downlink Message" },
	{ .type = &asn_DEF_FANSATCDownlinkMsgElementId, .format = &asn1_format_FANSATCDownlinkMsgElementId, .label = NULL },
	{ .type = &asn_DEF_FANSATCDownlinkMsgElementIdSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATCMessageHeader, .format = &asn1_format_SEQUENCE_cpdlc, .label = "Header" },
	{ .type = &asn_DEF_FANSATCUplinkMessage, .format = &asn1_format_FANSATCUplinkMessage, .label = "CPDLC Uplink Message" },
	{ .type = &asn_DEF_FANSATCUplinkMsgElementId, .format = &asn1_format_FANSATCUplinkMsgElementId, .label = NULL },
	{ .type = &asn_DEF_FANSATCUplinkMsgElementIdSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATISCode, .format = &asn1_format_any, .label = "ATIS code" },
	{ .type = &asn_DEF_FANSATWAlongTrackWaypoint, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATWAlongTrackWaypointSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Along-track waypoints" },
	{ .type = &asn_DEF_FANSATWAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATWAltitudeSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATWAltitudeTolerance, .format = &asn1_format_ENUM, .label = "ATW altitude tolerance" },
	{ .type = &asn_DEF_FANSATWDistance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSATWDistanceTolerance, .format = &asn1_format_ENUM, .label = "ATW distance tolerance" },
	{ .type = &asn_DEF_FANSBeaconCode, .format = &asn1_format_FANSBeaconCode, .label = "Code" },
	{ .type = &asn_DEF_FANSCOMNAVApproachEquipmentAvailable, .format = &asn1_format_any, .label = "COMM/NAV/Approach equipment available" },
	{ .type = &asn_DEF_FANSCOMNAVEquipmentStatus, .format = &asn1_format_ENUM, .label = "COMM/NAV equipment status" },
	{ .type = &asn_DEF_FANSCOMNAVEquipmentStatusSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "COMM/NAV Equipment status list" },
	{ .type = &asn_DEF_FANSDegreeIncrement, .format = &asn1_format_Deg, .label = "Degree increment" },
	{ .type = &asn_DEF_FANSDegrees, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSDegreesMagnetic, .format = &asn1_format_Deg, .label = "Degrees (magnetic)" },
	{ .type = &asn_DEF_FANSDegreesTrue, .format = &asn1_format_Deg, .label = "Degrees (true)" },
	{ .type = &asn_DEF_FANSDirection, .format = asn1_format_ENUM, .label = "Direction" },
	{ .type = &asn_DEF_FANSDirectionDegrees, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSDistance, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSDistanceKm, .format = &asn1_format_FANSDistanceMetric, .label = "Distance" },
	{ .type = &asn_DEF_FANSDistanceNm, .format = &asn1_format_FANSDistanceEnglish, .label = "Distance" },
	{ .type = &asn_DEF_FANSDistanceOffset, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSDistanceOffsetDirection, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSDistanceOffsetKm, .format = &asn1_format_FANSDistanceMetric, .label = "Offset" },
	{ .type = &asn_DEF_FANSDistanceOffsetNm, .format = &asn1_format_FANSDistanceOffsetNm, .label = "Offset" },
	{ .type = &asn_DEF_FANSEFCtime, .format = asn1_format_FANSTime, .label = "Expect further clearance at" },
	{ .type = &asn_DEF_FANSErrorInformation, .format = &asn1_format_ENUM, .label = "Error information" },
	{ .type = &asn_DEF_FANSFixName, .format = &asn1_format_any, .label = "Fix" },
	{ .type = &asn_DEF_FANSFixNext, .format = asn1_format_CHOICE_cpdlc, .label = "Next fix" },
	{ .type = &asn_DEF_FANSFixNextPlusOne, .format = asn1_format_CHOICE_cpdlc, .label = "Next+1 fix" },
	{ .type = &asn_DEF_FANSFreeText, .format = &asn1_format_any, .label = NULL },
	{ .type = &asn_DEF_FANSFrequency, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSFrequencyDeparture, .format = &asn1_format_FANSFrequencykHzToMHz, .label = "Departure frequency" },
	{ .type = &asn_DEF_FANSFrequencyhf, .format = &asn1_format_FANSFrequencyhf, .label = "HF" },
	{ .type = &asn_DEF_FANSFrequencysatchannel, .format = &asn1_format_any, .label = "Satcom channel" },
	{ .type = &asn_DEF_FANSFrequencyuhf, .format = &asn1_format_FANSFrequencykHzToMHz, .label = "UHF" },
	{ .type = &asn_DEF_FANSFrequencyvhf, .format = &asn1_format_FANSFrequencykHzToMHz, .label = "VHF" },
	{ .type = &asn_DEF_FANSHoldatwaypoint, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSHoldatwaypointSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Holding points" },
	{ .type = &asn_DEF_FANSHoldatwaypointSpeedHigh, .format = asn1_format_CHOICE_cpdlc, .label = "Holding speed (max)" },
	{ .type = &asn_DEF_FANSHoldatwaypointSpeedLow, .format = asn1_format_CHOICE_cpdlc, .label = "Holding speed (min)" },
	{ .type = &asn_DEF_FANSHoldClearance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSICAOfacilityDesignation, .format = &asn1_format_any, .label = "Facility designation" },
	{ .type = &asn_DEF_FANSICAOFacilityDesignationTp4Table, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSICAOFacilityFunction, .format = &asn1_format_ENUM, .label = "Facility function" },
	{ .type = &asn_DEF_FANSICAOFacilityIdentification, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSICAOFacilityName, .format = &asn1_format_any, .label = "Facility Name" },
	{ .type = &asn_DEF_FANSICAOUnitName, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSICAOUnitNameFrequency, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSIcing, .format = &asn1_format_ENUM, .label = "Icing" },
	{ .type = &asn_DEF_FANSInterceptCourseFrom, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSInterceptCourseFromSelection, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSInterceptCourseFromSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Intercept courses" },
	{ .type = &asn_DEF_FANSLatitude, .format = &asn1_format_FANSLatitude, .label = "Latitude" },
	{ .type = &asn_DEF_FANSLatitudeDegrees, .format = &asn1_format_Deg, .label = "Latitude" },
	{ .type = &asn_DEF_FANSLatitudeDirection, .format = &asn1_format_ENUM, .label = "Direction" },
	{ .type = &asn_DEF_FANSLatitudeLongitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSLatitudeLongitudeSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Coordinate list" },
	{ .type = &asn_DEF_FANSLatitudeReportingPoints, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSLatLonReportingPoints, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSLegDistance, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSLegDistanceEnglish, .format = &asn1_format_FANSDistanceEnglish, .label = "Leg distance" },
	{ .type = &asn_DEF_FANSLegDistanceMetric, .format = &asn1_format_FANSDistanceMetric, .label = "Leg distance" },
	{ .type = &asn_DEF_FANSLegTime, .format = &asn1_format_FANSLegTime, .label = "Leg time" },
	{ .type = &asn_DEF_FANSLegType, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSLongitude, .format = &asn1_format_FANSLongitude, .label = "Longitude" },
	{ .type = &asn_DEF_FANSLongitudeDegrees, .format = &asn1_format_Deg, .label = "Longitude" },
	{ .type = &asn_DEF_FANSLongitudeDirection, .format = &asn1_format_ENUM, .label = "Direction" },
	{ .type = &asn_DEF_FANSLongitudeReportingPoints, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSMsgIdentificationNumber, .format = &asn1_format_any, .label = "Msg ID" },
	{ .type = &asn_DEF_FANSMsgReferenceNumber, .format = &asn1_format_any, .label = "Msg Ref" },
	{ .type = &asn_DEF_FANSNavaid, .format = &asn1_format_any, .label = "Navaid" },
	{ .type = &asn_DEF_FANSPDCrevision, .format = &asn1_format_any, .label = "Revision number" },
	{ .type = &asn_DEF_FANSPlaceBearing, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPlaceBearingDistance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPlaceBearingPlaceBearing, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPosition, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionAltitudeAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionAltitudeSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionCurrent, .format = asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionDegrees, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionDistanceOffsetDirection, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionICAOUnitNameFrequency, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionPosition, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionProcedureName, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionReport, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionRouteClearance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionSpeedSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionTime, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionTimeAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPositionTimeTime, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSPredepartureClearance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSProcedure, .format = &asn1_format_any, .label = "Procedure name" },
	{ .type = &asn_DEF_FANSProcedureApproach, .format = asn1_format_SEQUENCE_cpdlc, .label = "Approach procedure" },
	{ .type = &asn_DEF_FANSProcedureArrival, .format = asn1_format_SEQUENCE_cpdlc, .label = "Arrival procedure" },
	{ .type = &asn_DEF_FANSProcedureDeparture, .format = asn1_format_SEQUENCE_cpdlc, .label = "Departure procedure" },
	{ .type = &asn_DEF_FANSProcedureName, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSProcedureTransition, .format = &asn1_format_any, .label = "Procedure transition" },
	{ .type = &asn_DEF_FANSProcedureType, .format = &asn1_format_ENUM, .label = "Procedure type" },
	{ .type = &asn_DEF_FANSPublishedIdentifier, .format = &asn1_format_SEQUENCE_cpdlc, .label = "Published identifier" },
	{ .type = &asn_DEF_FANSRemainingFuel, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRemainingFuelRemainingSouls, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRemainingSouls, .format = &asn1_format_any, .label = "Persons on board" },
	{ .type = &asn_DEF_FANSReportedWaypointAltitude, .format = asn1_format_CHOICE_cpdlc, .label = "Reported waypoint altitude" },
	{ .type = &asn_DEF_FANSReportedWaypointPosition, .format = asn1_format_CHOICE_cpdlc, .label = "Reported waypoint position" },
	{ .type = &asn_DEF_FANSReportedWaypointTime, .format = asn1_format_FANSTime, .label = "Reported waypoint time" },
	{ .type = &asn_DEF_FANSReportingPoints, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRouteClearance, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRouteInformation, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRouteInformationAdditional, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRouteInformationSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Route" },
	{ .type = &asn_DEF_FANSRTARequiredTimeArrival, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRTARequiredTimeArrivalSequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Required arrival times" },
	{ .type = &asn_DEF_FANSRTATime, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRTATolerance, .format = &asn1_format_FANSRTATolerance, .label = "RTA tolerance" },
	{ .type = &asn_DEF_FANSRunway, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSRunwayArrival, .format = asn1_format_SEQUENCE_cpdlc, .label = "Arrival runway" },
	{ .type = &asn_DEF_FANSRunwayConfiguration, .format = &asn1_format_ENUM, .label = "Runway configuration" },
	{ .type = &asn_DEF_FANSRunwayDeparture, .format = asn1_format_SEQUENCE_cpdlc, .label = "Departure runway" },
	{ .type = &asn_DEF_FANSRunwayDirection, .format = &asn1_format_any, .label = "Runway direction" },
	{ .type = &asn_DEF_FANSSpeed, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSSpeedGround, .format = &asn1_format_FANSSpeedEnglishX10, .label = "Ground speed" },
	{ .type = &asn_DEF_FANSSpeedGroundMetric, .format = &asn1_format_FANSSpeedMetricX10, .label = "Ground speed" },
	{ .type = &asn_DEF_FANSSpeedIndicated, .format = &asn1_format_FANSSpeedEnglishX10, .label = "Indicated airspeed" },
	{ .type = &asn_DEF_FANSSpeedIndicatedMetric, .format = &asn1_format_FANSSpeedMetricX10, .label = "Indicated airspeed" },
	{ .type = &asn_DEF_FANSSpeedMach, .format = &asn1_format_FANSSpeedMach, .label = "Mach number" },
	{ .type = &asn_DEF_FANSSpeedMachLarge, .format = &asn1_format_FANSSpeedMach, .label = "Mach number" },
	{ .type = &asn_DEF_FANSSpeedSpeed, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSSpeedTrue, .format = &asn1_format_FANSSpeedEnglishX10, .label = "True airspeed" },
	{ .type = &asn_DEF_FANSSpeedTrueMetric, .format = &asn1_format_FANSSpeedMetricX10, .label = "True airspeed" },
	{ .type = &asn_DEF_FANSSSREquipmentAvailable, .format = &asn1_format_ENUM, .label = "SSR equipment available" },
	{ .type = &asn_DEF_FANSSupplementaryInformation, .format = &asn1_format_any, .label = "Supplementary information" },
	{ .type = &asn_DEF_FANSTemperature, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTemperatureC, .format = &asn1_format_FANSTemperatureC, .label = "Temperature" },
	{ .type = &asn_DEF_FANSTemperatureF, .format = &asn1_format_FANSTemperatureF, .label = "Temperature" },
	{ .type = &asn_DEF_FANSTime, .format = &asn1_format_FANSTime, .label = "Time" },
	{ .type = &asn_DEF_FANSTimeAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeAtPositionCurrent, .format = asn1_format_FANSTime, .label = "Time at current position" },
	{ .type = &asn_DEF_FANSTimeDepartureEdct, .format = asn1_format_FANSTime, .label = "Estimated departure time" },
	{ .type = &asn_DEF_FANSTimeDistanceOffsetDirection, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeDistanceToFromPosition, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeEtaAtFixNext, .format = asn1_format_FANSTime, .label = "ETA at next fix" },
	{ .type = &asn_DEF_FANSTimeEtaDestination, .format = asn1_format_FANSTime, .label = "ETA at destination" },
	{ .type = &asn_DEF_FANSTimeICAOunitnameFrequency, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimePosition, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimePositionAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimePositionAltitudeSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeSpeedSpeed, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimestamp, .format = &asn1_format_FANSTimestamp, .label = "Timestamp" },
	{ .type = &asn_DEF_FANSTimeTime, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTimeTolerance, .format = &asn1_format_ENUM, .label = "Time tolerance" },
	{ .type = &asn_DEF_FANSToFrom, .format = asn1_format_ENUM, .label = "To/From" },
	{ .type = &asn_DEF_FANSToFromPosition, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTp4table, .format = &asn1_format_ENUM, .label = "TP4 table" },
	{ .type = &asn_DEF_FANSTrackAngle, .format = asn1_format_CHOICE_cpdlc, .label = "Track angle" },
	{ .type = &asn_DEF_FANSTrackDetail, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSTrackName, .format = &asn1_format_any, .label = "Track name" },
	{ .type = &asn_DEF_FANSTrueheading, .format = asn1_format_CHOICE_cpdlc, .label = "True heading" },
	{ .type = &asn_DEF_FANSTurbulence, .format = &asn1_format_ENUM, .label = "Turbulence" },
	{ .type = &asn_DEF_FANSVersionNumber, .format = &asn1_format_any, .label = "Version number" },
	{ .type = &asn_DEF_FANSVerticalChange, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSVerticalDirection, .format = &asn1_format_ENUM, .label = "Vertical direction" },
	{ .type = &asn_DEF_FANSVerticalRate, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSVerticalRateEnglish, .format = &asn1_format_FANSVerticalRateEnglish, .label = "Vertical rate" },
	{ .type = &asn_DEF_FANSVerticalRateMetric, .format = &asn1_format_FANSVerticalRateMetric, .label = "Vertical rate" },
	{ .type = &asn_DEF_FANSWaypointSpeedAltitude, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSWaypointSpeedAltitudesequence, .format = &asn1_format_SEQUENCE_OF_cpdlc, .label = "Waypoints, speeds and altitudes" },
	{ .type = &asn_DEF_FANSWindDirection, .format = &asn1_format_Deg, .label = "Wind direction" },
	{ .type = &asn_DEF_FANSWinds, .format = &asn1_format_SEQUENCE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSWindSpeed, .format = &asn1_format_CHOICE_cpdlc, .label = NULL },
	{ .type = &asn_DEF_FANSWindSpeedEnglish, .format = &asn1_format_FANSWindSpeedEnglish, .label = "Wind speed" },
	{ .type = &asn_DEF_FANSWindSpeedMetric, .format = &asn1_format_FANSWindSpeedMetric, .label = "Wind speed" },
	{ .type = &asn_DEF_NULL, .format = &asn1_format_NULL, .label = NULL }
};

static size_t asn1_cpdlc_formatter_table_len = sizeof(asn1_cpdlc_formatter_table) / sizeof(asn_formatter_t);

void asn1_output_cpdlc(FILE *stream, asn_TYPE_descriptor_t *td, const void *sptr, int indent) {
	asn1_output(stream, asn1_cpdlc_formatter_table, asn1_cpdlc_formatter_table_len, td, sptr, indent);
}
