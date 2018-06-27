/*******************************************************************************
  PTP private file for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ptp_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _PTP_PRIVATE_H_
#define _PTP_PRIVATE_H_

#include <stdint.h>
#include <stdbool.h>

// @@@@@
// Configuration
//
// # of PTP ports on this clock
// 1 for an ordinary clock
// > 1 for boundary clocks
#define TCPIP_PTP_PORTS         1

// number of foreign master records to store
// should be at least 5 as recommended by the standard
#define TCPIP_PTP_FOREIGN_MASTER_RECORDS       5

// estimation of the timestamps for the TX messages could
// if 0, then no estimation is done and the timestamp is 0
// otherwise, the proper time stamp is used, as read from the local clock
#define TCPIP_PTP_TX_TIMESTAMP_ESTIMATE         1

// @@@@@
// End Configuration

// debugging
#define TCPIP_PTP_DEBUG_MASK_BASIC           (0x0001)

// enable IPV4 debugging levels
#define TCPIP_PTP_DEBUG_LEVEL  (TCPIP_PTP_DEBUG_MASK_BASIC)


// current/supported version of the protocol
#define PTP_VERSION_NUMBER          2

// Section: PTP data types
// All PTP data is in network byte order!

// PTP Device type
typedef enum
{
    PTP_DEVICE_TYPE_ORDINARY_CLK,           // ordinary clock
    PTP_DEVICE_TYPE_BOUNDARY_CLK,           // boundary clock
    PTP_DEVICE_TYPE_E2E_TRANSPARENT_CLK,    // end to end transparent clock
    PTP_DEVICE_TYPE_P2P_TRANSPARENT_CLK,    // peer to peer transparent clock
    PTP_DEVICE_TYPE_MANAGEMENT_NODE,        // management node
}PTP_DEVICE_TYPE;



// PTP Network transport protocol types
// Note : 16 bits!
typedef enum
{
    PTP_NET_PROTO_INVALID       = 0,    // reserved, invalid
    PTP_NET_PROTO_UDP_IPV4      = 1,
    PTP_NET_PROTO_UDP_IPV6      = 2,
    PTP_NET_PROTO_ETH           = 3,    // IEEE 802.3
    PTP_NET_PROTO_DEVICE_NET    = 4, 
    PTP_NET_PROTO_CONTROL_NET   = 5,
    PTP_NET_PROTO_PROFINET      = 6,

    PTP_NET_PROTO_UNKNOWN       = 0xfffe,   // unknown protocol
    PTP_NET_PROTO_RESERVED      = 0xffff,   // reserved

    // any other value is reserved/invalid
}PTP_NETWORK_PROTOCOL;

// PTP TimeInterval
typedef struct
{
    int64_t     scaledNanoseconds;      // the time interval expressed in units of nanoseconds and multiplied by 2+16.  
                                        // Positive or negative time intervals outside the maximum range of this data type
                                        // shall be encoded as the largest positive and negative values of the data type, respectively. 
                                        // For example, 2.5 ns is expressed as hex 0000 0000 0002 8000 (0x28000 / 2^16 = 2.5) 
}PTP_TIME_INTERVAL;

// PTP timestamp
// The Timestamp type represents a positive time with respect to the epoch. 
typedef struct __attribute__((packed))
{ 
    uint16_t secondsField_16;       // 48 bit field: integer portion of the timestamp in seconds 
    uint32_t secondsField_32;       // 
    uint32_t nanosecondsField;      // fractional portion of the timestamp in nanoseconds 
                                    // always < 10 ^ 9 = 0x3B9A_CA00; ie 30 bits used
}PTP_TIMESTAMP; 

 

// The ClockIdentity type identifies a clock. 
typedef struct
{
    uint8_t     id[8];      // the 3 MSBytes shall be an OUI
                            //      the 2 LSbits of the MSB shall be 0
                            // Shall be an EUI-64 or an EUI-64 constructed from EUI-48
                            //      EUI-48 shall be a ETH MAC addressowned by the organization
                            //      mapping rules shall be those specified by IEEE
                            //      The first 3 bytes, the OUI, shall be the first of the clock identity.
                            //      Ex: if the MAC address is 0xacde48234567, th egenerated EUI-64 will be:
                            //      0xacde48-FFFE-234567
                            //
                            // Reserved value: 0xffffffffffffffff - all clocks in the domain!     
}PTP_CLOCK_IDENTITY;                            
 

// PTP PortIdentity  
// The PortIdentity type identifies a PTP port. 
typedef struct
{ 
    PTP_CLOCK_IDENTITY  clockIdentity; 
    uint16_t            portNumber;     // For a port on a PTP node supporting a single PTP port the value shall be 1.
                                        // The values of the port numbers for the N ports on a PTP node supporting
                                        //      N PTP ports shall be 1, 2, ... N, respectively.
                                        // 0xffff - reserved "all ports"
                                        // 0 - reserved; represents NULL port number (uninitialized/invalid port)
                                        //
}PTP_PORT_IDENTITY; 


// PTP PortAddress 
// The PortAddress type represents the protocol address of a PTP port. 
typedef struct
{ 
    uint16_t       networkProtocol;     // a PTP_NETWORK_PROTOCOL value     
    uint16_t       addressLength;       // The addressLength is the length in octets of the address.
                                        // The range shall be 1 to 16 octets. 
    uint8_t         addressField[0];        // size is addressLength bytes
}PTP_PORT_ADDRESS; 

// The PortAddress type for UDP/IPv4
typedef struct
{ 
    uint16_t       networkProtocol;     // = PTP_NET_PROTO_UDP_IPV4     
    uint16_t       addressLength;       // = 4; 
    uint8_t     addressField[4];        // an IPV4 address 
}PTP_PORT_ADDRESS_IPV4; 
 
// The PortAddress type for UDP/IPv6
typedef struct
{ 
    uint16_t       networkProtocol;     // = PTP_NET_PROTO_UDP_IPV6     
    uint16_t       addressLength;       // = 16; 
    uint8_t    addressField[16];        // an IPV6 address 
}PTP_PORT_ADDRESS_IPV6; 
 
// The PortAddress type for ETH/IEEE 802.3
typedef struct
{ 
    uint16_t       networkProtocol;     // = PTP_NET_PROTO_ETH     
    uint16_t       addressLength;       // = 6; 
    uint8_t     addressField[6];        // a MAC address 
}PTP_PORT_ADDRESS_ETH; 
 
 
// PTP TLV 
// The TLV type represents TLV extension fields. 
typedef struct
{ 
    uint16_t    tlvType; 
    uint16_t    lengthField;    // length of all TLVs shall be an even number of octets!
    uint8_t  valueField[0];     // lengthField bytes
}PTP_TLV; 



// PTP Text
// The PTPText data type is used to represent textual material in PTP messages. 
typedef struct
{ 
    uint8_t     lengthField; 
    uint8_t  textField[0];  // lengthField bytes
}PTP_TEXT; 

// PTP Fault Record
// The FaultRecord type is used to construct fault logs. 
typedef struct
{ 
    uint16_t        faultRecordLength;      // the number of octets in the FaultRecord
                                            // not including the 2 octets of the faultRecordLength member. 
    PTP_TIMESTAMP   faultTime; 
    uint8_t         severityCode; 
    PTP_TEXT        faultName; 
    PTP_TEXT        faultValue; 
    PTP_TEXT        faultDescription; 
}PTP_FAULT_RECORD; 


// PTP domain enumeration
// Note: 8 bits!
typedef enum
{
    PTP_DOMAIN_DEFAULT      = 0,    // default domain
    PTP_DOMAIN_ALTERNATE_1,         // alternate domain 1
    PTP_DOMAIN_ALTERNATE_2,         // alternate domain 2
    PTP_DOMAIN_ALTERNATE_3,         // alternate domain 3
    //
    PTP_DOMAIN_USER_START   = 4,    // user domain start
    PTP_DOMAIN_USER_END     = 127,  // user domain end
    //
    PTP_DOMAIN_RESERVED     = 128,  // 128 - 255: reserved domain
}PTP_DOMAIN_NUMBER;

// PTP timescales
typedef enum
{
    PTP_TIMESCALE_PTP   = 0,        // default PTP scale;
                                    // the epoch is the PTP epoch: 1 Jan 1970 00:00:00 TAI (31 Dec 1969 23:59:51.999918 UTC)
                                    // same as POSIX epoch!
                                    // timePropertiesDS.currentUtcOffset = TAI - UTC;
                                    // timescale is continuous
    PTP_TIMESCALE_ARB               // arbitrary timescale
                                    // set by administrative procedure
                                    // epoch could be reset during normal operation
                                    // time scale is continuous between invocations of the admin procedure
                                    //
}PTP_TIMESCALE;



// PTP Clock Class
// denotes the traceability of the time or frequency distributed by the grandmaster clock.
// Note: 8 bits!
typedef enum
{
    PTP_CLOCK_CLASS_PRIMARY_SYNC            = 6,    // Clock that is synchronized to a primary reference time source.
                                                    // The timescale distributed shall be PTP.
                                                    // A clockClass 6 clock shall not be a slave to another clock in the domain!
    PTP_CLOCK_CLASS_PRIMARY_SYNC_HOLD       = 7,    // Previously clockClass 6 but that has lost the ability to synchronize
                                                    // to a primary reference time source and is in holdover mode 
                                                    // The timescale distributed shall be PTP.
                                                    // A clockClass 7 clock shall not be a slave to another clock in the domain!
    PTP_CLOCK_CLASS_APP_SYNC                = 13,   // Shall designate a clock that is synchronized to an application-specific source of time.
                                                    // The timescale distributed shall be ARB.
                                                    // A clockClass 13 clock shall not be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_APP_SYNC_HOLD           = 14,   // Previously a clockClass 13 but that has lost the ability to synchronize
                                                    // to an application-specific source of time and is in holdover
                                                    // The timescale distributed shall be ARB.
                                                    // A clockClass 14 clock shall not be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_PRIMARY_SYNC_DEGRADE_A  = 52,   // Degradation alternative A for a clock of clockClass 7
                                                    // that is not within holdover specification.
                                                    // A clock of clockClass 52 shall not be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_APP_SYNC_DEGRADE_A      = 58,   // Degradation alternative A for a clock of clockClass 14
                                                    // that is not within holdover specification.
                                                    // A clock of clockClass 58 shall not be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_ALTERNATE_PASSIVE_BEGIN = 68,   // Start of use by alternate PTP profiles, passive mode
    PTP_CLOCK_CLASS_ALTERNATE_PASSIVE_END   = 122,  // End of use by alternate PTP profiles, passive mode

    PTP_CLOCK_CLASS_128_BARRIER             = 128,  // ! 128 barrier
                                                    // In a given domain, clocks with clockClass values less than 128 should be inherently
                                                    // at least as stable (low  variance) as any clock with a clock class value
                                                    // greater than its own. 

    PTP_CLOCK_CLASS_ALTERNATE_SLAVE_BEGIN   = 133,  // Start of use by alternate PTP profiles, slave mode
    PTP_CLOCK_CLASS_ALTERNATE_SLAVE_END     = 170,  // End of use by alternate PTP profiles, slave mode

    PTP_CLOCK_CLASS_PRIMARY_SYNC_DEGRADE_B  = 187,  // Degradation alternative B for a clock of clockClass 7
                                                    // that is not within holdover specification.
                                                    // A clock of clockClass 187 may be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_APP_SYNC_DEGRADE_B      = 193,  // Degradation alternative B for a clock of clockClass 14
                                                    // that is not within holdover specification.
                                                    // A clock of clockClass 193 may be a slave to another clock in the domain. 
    PTP_CLOCK_CLASS_ALTERNATE_APP_BEGIN     = 216,  // Start of alternate PTP profiles, application specific precedence
    PTP_CLOCK_CLASS_ALTERNATE_APP_END       = 232,  // End of alternate PTP profiles, application specific precedence


    PTP_CLOCK_CLASS_DEFAULT                 = 248,  // This clock Class shall be used if none of the other clockClass definitions apply.

    PTP_CLOCK_CLASS_RESERVED_VER1           = 251,  // Reserved for version 1 compatibility; see Clause 18.

    PTP_CLOCK_CLASS_SLAVE_ONLY              = 255   // Shall be the clock Class of a slave-only clock;

    // any other value is reserved for various reasons
    // see the standard
}PTP_CLOCK_CLASS;


// PTP Clock Accuracy
// The clockAccuracy indicates the expected accuracy of a clock when it is the grandmaster,
// or in the event it becomes the grandmaster.
// Note: 8 bits!
typedef enum
{
    PTP_CLOCK_ACCURACY_25NS         = 0x20,     // The time is accurate to within 25 ns
    PTP_CLOCK_ACCURACY_100NS        = 0x21,     // The time is accurate to within 100 ns
    PTP_CLOCK_ACCURACY_250NS        = 0x22,     // The time is accurate to within 250 ns
    PTP_CLOCK_ACCURACY_1US          = 0x23,     // The time is accurate to within 1 microsecond
    PTP_CLOCK_ACCURACY_2PT5US       = 0x24,     // The time is accurate to within 2.5 microsecond
    PTP_CLOCK_ACCURACY_10US         = 0x25,     // The time is accurate to within 10 microsecond
    PTP_CLOCK_ACCURACY_25US         = 0x26,     // The time is accurate to within 25 microsecond
    PTP_CLOCK_ACCURACY_100US        = 0x27,     // The time is accurate to within 100 microsecond
    PTP_CLOCK_ACCURACY_250US        = 0x28,     // The time is accurate to within 250 microsecond
    PTP_CLOCK_ACCURACY_1MS          = 0x29,     // The time is accurate to within 1 millisecond
    PTP_CLOCK_ACCURACY_2PT5MS       = 0x2a,     // The time is accurate to within 2.5 millisecond
    PTP_CLOCK_ACCURACY_10MS         = 0x2b,     // The time is accurate to within 10 millisecond
    PTP_CLOCK_ACCURACY_25MS         = 0x2c,     // The time is accurate to within 25 millisecond
    PTP_CLOCK_ACCURACY_100MS        = 0x2d,     // The time is accurate to within 100 millisecond
    PTP_CLOCK_ACCURACY_250MS        = 0x2e,     // The time is accurate to within 250 millisecond
    PTP_CLOCK_ACCURACY_1S           = 0x2f,     // The time is accurate to within 1 second
    PTP_CLOCK_ACCURACY_10S          = 0x30,     // The time is accurate to within 10 second
    PTP_CLOCK_ACCURACY_11S          = 0x31,     // The time is accurate to > 10 second

    PTP_CLOCK_ACCURACY_ALT_BEGIN    = 0x80,     // start of alternate PTP profile accuracy
    PTP_CLOCK_ACCURACY_ALT_END      = 0xfd,     // end of alternate PTP profile accuracy

    PTP_CLOCK_ACCURACY_UNKNOWN      = 0xfe,     // end of alternate PTP profile accuracy

    // any other value is reserved for various reasons
    // see the standard
}PTP_CLOCK_ACCURACY;

// PTP ClockQuality  
// The ClockQuality represents the quality of a clock. 
typedef struct 
{ 
    uint8_t     clockClass;             // PTP_CLOCK_CLASS value
    uint8_t     clockAccuracy;          // PTP_CLOCK_ACCURACY value
    uint16_t    offsetScaledLogVariance; // Estimate of its inherent precision:
                                        // 2 ^ 8 x Log2(estimation)
                                        // The estimation is the precision of the timestamps included in messages issued by the 
                                        // clock when it is not synchronized to another clock using the protocol
                                        // Based on Allen deviation.
                                        // Normally this characterization is done using a reference clock (atomic clock,
                                        // GPS, very stable local oscillator, etc.).
                                        // Can be set as a constant at initialization time!
                                        // The smallest variance that can be represented is 2^-128 = ~3 x 10^-39 s^2 which
                                        // results in value 0f 0x0000.
                                        // The maximum variance that can be represented is 2 ^ 127.99609 which result in 0xffff.
                                        // Value 0xffff indicates that the variance is either too large to be represented 
                                        // or it has not been computed.
                                        // A hysteresis value == 2 ^ 7 is also used!
    
}PTP_CLOCK_QUALITY; 


// PTP Time Source
// This information-only attribute indicates the source of time
// used by the grandmaster clock.
// The value is not used in the selection of the grandmaster clock
// Note: 8 bits!
typedef enum
{
    PTP_TIME_SOURCE_ATOMIC_CLOCK    = 0x10,     // Device  based on atomic resonance for frequency 
                                                // and that has been calibrated against international standards
    PTP_TIME_SOURCE_GPS             = 0x20,     // Device synchronized to a satellite system
    PTP_TIME_SOURCE_RADIO           = 0x30,     // Device synchronized via any of the radio distribution systems
                                                // that distribute time and frequency tied to international standards 
    PTP_TIME_SOURCE_PTP             = 0x40,     // Any device synchronized to a PTP-based source of time external to the domain 
    PTP_TIME_SOURCE_NTP             = 0x50,     // Any device synchronized via NTP/SNTP to time servers
    PTP_TIME_SOURCE_HAND_SET        = 0x60,     // Set by means of a human interface based on observation of an 
                                                // international standards source of time to within the claimed clock accuracy 
    PTP_TIME_SOURCE_OTHER           = 0x90,     // Other source of time and/or frequency not covered by other values 
    PTP_TIME_SOURCE_INTERNAL_OSC    = 0xa0,     // Device whose time is based on a free-running oscillator with epoch determined in an 
                                                // arbitrary or unknown manner 
    PTP_TIME_SOURCE_ALT_BEGIN       = 0xf0,     // Start of time source to be used in alternate PTP profiles 
    PTP_TIME_SOURCE_ALT_END         = 0xfe,     // End of time source to be used in alternate PTP profiles 
    
    PTP_TIME_SOURCE_RESERVED        = 0xff,     // reserved

    // any other value is reserved for various reasons
    // see the standard
}PTP_TIME_SOURCE;


// Data sets for ordinary and boundary clocks

// defaultDS: Attributes describing the ordinary or boundary clock. 
typedef struct
{
    // static members
    uint8_t             twoStepFlag;        // need to use PTP_FLAG_FIELD ???? @@@@
                                            // For a one-step clock, the value of twoStepFlag shall be FALSE. 
                                            // For a two-step clock, the value of twoStepFlag shall be TRUE.
    PTP_CLOCK_IDENTITY  clockIdentity;      // identity of the local clock 
    uint16_t            numberPorts;        // number of PTP ports on device: PTP_PORT_DS. 1 for ordinary clock
    //
    // dynamic members
    PTP_CLOCK_QUALITY   clockQuality;       //  - clockClass initialization value depends on slaveOnly:
                                            //      if slaveOnly == true, should be PTP_CLOCK_CLASS_SLAVE_ONLY (255) 
                                            //      if slaveOnly == false, then use PTP profile, etc. See 8.2.1.3.1.1
                                            //  - clockAccuracy initialization value depends on clockClass which shall be peviously initialized
                                            //      shall represent the clockAccuracy of the clock at the time of initialization
                                            //  - offsetScaledLogVariance: initialization value soffsetScaledLogVariance
                                            //      shall reflect the inherent characteristics of the local clock at the time of initialization
                                            //
                                            //
                                            //    
    uint8_t             priority1;          // configurable in [0-255] range
                                            // lower values take precedence
                                            // specified in a PTP profile
    uint8_t             priority2;          // configurable in [0-255] range
                                            // lower values take precedence
                                            // specified in a PTP profile
    uint8_t             domainNumber;       // PTP_DOMAIN_NUMBER value 
    uint8_t             slaveOnly;          // boolean: shall be TRUE if the clock is a slave-only clock
                                            // shall be FALSE if the clock is a non-slave-only clock

}PTP_DEFAULT_DS;

// currentDS: Attributes related to synchronization. 
typedef struct
{
    // dynamic members (all)
    uint16_t        stepsRemoved;           // the number of communication paths traversed between the local 
                                            // clock and the grandmaster clock.  
                                            // The initialization value shall be 0
    PTP_TIME_INTERVAL offsetFromMaster;     // current value of the time difference between a master and a slave as computed by the slave
                                            // <offsetFromMaster> = <Time on the slave clock> - <Time on the master clock>
                                            // The initialization value shall be either: 
                                            //      - The value in non-volatile read-write storage if implemented 
                                            //      - Implementation-specific 
                    
    PTP_TIME_INTERVAL meanPathDelay;        // Current value of the mean propagation time between a master and slave clock as
                                            // computed by the slave.
                                            // The initialization value shall be either: 
                                            //      - The value in nonvolatile read-write storage if implemented 
                                            //      - Implementation-specific
}PTP_CURRENT_DS;

// parentDS: Attributes describing the parent (the clock to which the ordinary clock synchronizes)
// and the grandmaster (the clock at the root of the master-slave hierarchy). 
// NOTE: The values of the parentDS data set shall be initialized after the values in the defaultDS data set. 
typedef struct
{
    // dynamic members (all)
    PTP_PORT_IDENTITY       parentPortIdentity;     // This is the portIdentity of the port on the master that issues
                                                    // the Sync messages used in synchronizing this clock. 
                                                    // The initialization value shall be as follows: 
                                                    //      -  parentPortIdentity.clockIdentity == defaultDS.clockIdentity field. 
                                                    //      -  parentPortIdentity.portNumber member is 0. 

    bool                     parentStats;           // A slave clock MAY maintain statistics on the observed performance of its parent.
                                                    // Shall be TRUE if all of the following conditions are satisfied: 
                                                    //      - The clock has a port in the SLAVE state. 
                                                    //      - The clock has computed statistically valid estimates of the 
                                                    //        parentDS.obsParentOffsetScaledLogVar and the
                                                    //        parentDS.obsParentClockPhaseChangeRate members. 
                                                    // Otherwise the value shall be FALSE. 
                                                    // The initialization value shall be FALSE. 
    uint16_t    obsParentOffsetScaledLogVar;        // Optional: Estimate of the precision of the clock to which
                                                    // it is synchronized as observed by this clock. 
                                                    // The initialization value shall be 0xffff
    uint32_t    obsParentClockPhaseChangeRate;      // Estimate of the parent clock’s phase change rate
                                                    // as measured by the slave clock
                                                    // Reported as 2 ^ 40 x Fractional Frequency Offset
                                                    // The minimum value expressed is 2 ^ -40 == ~ 1 ps per second.
                                                    // If the estimate exceeds the capacity of its data type
                                                    // this value shall be set to 0x7FFF_FFFF or 0x8000_0000, as appropriate.
                                                    // A positive sign indicates that the parent clock’s phase change rate is greater
                                                    // than the rate of the slave clock. 
                                                    // The initialization value shall be 0x7FFF_FFFF irrespective of whether the
                                                    // computation is implemented in the local clock.  
                                                    // A value equal to 0x7FFF_FFFF indicates that either the value exceeds
                                                    // the capacity of the data type or that the value has not been computed. 
                                                    // NOTE:  This value is dependent on the measurement time interval used.
                                                    //        When this value is critical for an application domain,
                                                    //        the time interval should be specified in the applicable PTP profile. 
                                                    
                            
    PTP_CLOCK_IDENTITY      grandmasterIdentity;    // clockIdentity attribute of the grandmaster clock
                                                    // The initialization value shall be the defaultDS.clockIdentity member
    PTP_CLOCK_QUALITY       grandmasterClockQuality;// clockQuality attribute of the grandmaster clock
                                                    // The initialization value shall be the value of the defaultDS.clockQuality member.

    uint8_t                 grandmasterPriority1;   // priority1 attribute of the grandmaster clock. 
                                                    // The initialization value shall be the value of the defaultDS.priority1 member.

    uint8_t                 grandmasterPriority2;   // priority2 attribute of the grandmaster clock. 
                                                    // The initialization value shall be the value of the defaultDS.priority2 member.
}PTP_PARENT_DS;

// timePropertiesDS: Attributes of the timescale. 
typedef struct
{
    // dynamic members (all)
    int16_t     currentUtcOffset;       // in a PTP system with PTP epoch: this is value of the offset between TAI and UTC
                                        // otherwise there is no meaning
                                        // Initialization:
                                        //      - if PTP epoch,  the value is the value obtained from a 
                                        //        primary reference if the value is known at the time of initialization
                                        //      - else the current number of leap seconds when the node is designed
    bool        currentUtcOffsetValid;  // TRUE if the currentUtcOffset is known to be correct. 
                                        // The initialization value shall be TRUE if the value of currentUtcOffset is known to be 
                                        // correct; otherwise, it shall be FALSE. 

    bool        leap59;                 // in a PTP epoch system a TRUE value shall indicate that the last minute
                                        // of the current UTC day contains 59 seconds. 
                                        // If the epoch is not PTP, the value shall be set to FALSE.
                                        // Initialization:
                                        //      - if ptpTimescale is TRUE, the value shall be the value obtained
                                        //        from a primary reference if known at the time of initialization
                                        //      - else the value shall be FALSE. 
    bool        leap61;                 // in a PTP epoch system a TRUE value shall indicate that the last minute
                                        // of the current UTC day contains 61 seconds. 
                                        // If the epoch is not PTP, the value shall be set to FALSE.
                                        // Initialization:
                                        //      - if ptpTimescale is TRUE, the value shall be the value obtained
                                        //        from a primary reference if known at the time of initialization
                                        //      - else the value shall be FALSE.     
    bool        timeTraceable;          // TRUE if the timescale and the value of currentUtcOffset are traceable to a primary reference;
                                        // Otherwise, the value shall be FALSE. 
                                        // Initialization:
                                        //      - If the ptpTimescale is TRUE and the time and the value of currentUtcOffset are traceable
                                        //          to a primary reference at the time of initialization, 
                                        //          the value shall be TRUE
                                        //       - else the value shall be FALSE. 
    bool        frequencyTraceable;     // TRUE if the frequency determining the timescale is traceable to a primary reference;
                                        // Otherwise, the value shall be FALSE. 
                                        // Initialization:
                                        //      - If the frequency is traceable to a primary reference at the time of initialization the value shall be TRUE
                                        //      - else the value shall be FALSE. 
    bool        ptpTimescale;           // TRUE if the clock timescale of the grandmaster clock is PTP
                                        // FALSE otherwise. 
                                        // Initialization:
                                        //      - ptpTimescale member shall be initialized before the other members of this data set.
                                        //      - If the clock timescale is PTP and this is known at the time of initialization,
                                        //        the value shall be TRUE
                                        //      - else  shall be FALSE, indicating that the timescale is ARB. 
    uint8_t     timeSource;             // PTP_TIME_SOURCE: The source of time used by the grandmaster clock.
                                        // Initialization:
                                        //      - If the timeSource is known at the time of initialization, the value shall be set to that value
                                        //      - else it shall be INTERNAL_OSCILLATOR.
}PTP_TIME_PROPERTIES_DS;

// current state of the protocol engine
// associated with this port
typedef enum
{
    PTP_PORT_STATE_INVALID      = 0,    // not a valid state
    PTP_PORT_STATE_INITIALIZING,        //
    PTP_PORT_STATE_FAULTY,              //
    PTP_PORT_STATE_DISABLED,            //
    PTP_PORT_STATE_LISTENING,           //
    PTP_PORT_STATE_PRE_MASTER,          //
    PTP_PORT_STATE_MASTER,              //
    PTP_PORT_STATE_PASSIVE,             //
    PTP_PORT_STATE_UNCALIBRATED,        //
    PTP_PORT_STATE_SLAVE,               //
    // All other values reserved;
}PTP_PORT_STATE;

// recommended state of the protocol engine
// used for decisions based on the State Decision Event mechanism
// @@@@ is this separation from PTP_PORT_STATE needed?
typedef enum
{
    PTP_PORT_REC_STATE_LISTENING,           //
    PTP_PORT_REC_STATE_MASTER_M1,           //
    PTP_PORT_REC_STATE_MASTER_M2,           //
    PTP_PORT_REC_STATE_MASTER_M3,           //
    PTP_PORT_REC_STATE_PASSIVE_P1,          //
    PTP_PORT_REC_STATE_PASSIVE_P2,          //
    PTP_PORT_REC_STATE_SLAVE_S1,            //
}PTP_PORT_REC_STATE;

typedef enum
{
    PTP_DELAY_MECHANISM_E2E         = 0x01,     // The port is configured to use the delay request-response mechanism. 
    PTP_DELAY_MECHANISM_P2P         = 0x02,     // The port is configured to use the peer delay mechanism. 
    PTP_DELAY_MECHANISM_DISABLED    = 0xfe,     // The port does not implement the delay mechanism;
                                                //  NOTE:   This value shall not be set by a clock except when the applicable
                                                //          PTP profile specifies that the clock syntonize only
                                                //          and that neither path delay mechanism is to be used. 
}PTP_DELAY_MECHANISM;


// Port data sets: portDS
// Is the basis for protocol decisions and for providing values for message fields.
// The number of such data sets shall be the value of PTP_DEFAULT_DS.numberPorts.
// NOTE:
//      all message transmission intervals are represented as log2(interval in seconds);
//      will have values [-128, +127] subject to further profile limitations
//      The intervals are communicated through the logMessageInterval field of PTP messages.
//
typedef struct
{
    // static data members
    PTP_PORT_IDENTITY   portIdentity;           //  port identity of the local port
                                                // Note: only the portNumber could be stored here since the clockIdentity
                                                // is the same for all ports on this clock 

    // dynamic data members 
    PTP_PORT_STATE      portState;              // current state of the protocol engine
                                                // associated with this port
    int8_t              logMinDelayReqInterval; // the logarithm to the base 2 of the minDelayReqInterval
                                                // The minimum permitted mean time interval between successive Delay_Req messages,
                                                // i.e., the minDelayRequestInterval, sent by a slave to a specific port on the master
                                                // This value is determined and advertised by a master clock based on the ability
                                                // of the master clock to process the Delay_Req message traffic.
                                                // The minimum value being portDS.logSyncInterval, i.e., at the same rate as Sync messages,
                                                // and a maximum value of logSyncInterval+5, i.e., 1 Delay_Req message every 32 Sync messages.
    PTP_TIME_INTERVAL   peerMeanPathDelay;      // If the value of the delayMechanism member is peer-to-peer (P2P),
                                                // then the value of peerMeanPathDelay shall be an estimate of the current one-way propagation
                                                // delay on the link, i.e., <meanPathDelay>, attached to this port computed using
                                                // the peer delay mechanism;
                                                // If the value of the portDS.delayMechanism member is end-to-end (E2E),
                                                // this member’s value shall be zero.
                                                // Initialization:
                                                //      The initialization value shall be zero. 

    // configurable data members      
    int8_t              logAnnounceInterval;    // the logarithm to the base 2 of the mean announceInterval
                                                // The announce interval is 2 ^ logAnnounceInterval;
                                                // the mean time interval between successive Announce messages i.e. the announceInterval.  
                                                // Uniform throughout a domain
    uint8_t             announceReceiptTimeout; // an integral multiple of announceInterval
                                                // The value of announceReceiptTimeout shall specify the number of announceInterval
                                                // that has to pass without receipt of an Announce message before
                                                // the occurrence of the event ANNOUNCE_RECEIPT_TIMEOUT_EXPIRES; 
                                                // The range shall be 2 to 255 subject to further restrictions of a PTP profile.
                                                // The minimum value should be 3.
                                                // The product (portDS.announceReceiptTimeout) × 2 ^ portDS.logAnnounceInterval
                                                // should be uniform throughout a domain. 

    int8_t              logSyncInterval;        // the logarithm to the base 2 of the mean SyncInterval for multicast messages
                                                // the mean time interval between successive Sync messages, i.e., 
                                                // the syncInterval, when transmitted as multicast messages.
    uint8_t             delayMechanism;         // PTP_DELAY_MECHANISM: The value of delayMechanism shall indicate the propagation delay measuring option
                                                // used by the port in computing <meanPathDelay>.
                                                // Initialization:
                                                //      The initialization value is implementation-specific unless otherwise stated in a PTP profile.
    
    int8_t              logMinPdelayReqInterval;// the logarithm to the base 2 of the minPdelayReqInterval
                                                // The minimum permitted mean time interval between successive Pdelay_Req messages,
                                                // i.e., the minPdelayReqInterval, sent over a link
    uint8_t             versionNumber;          // The value of versionNumber shall indicate the PTP version in use on the port: PTP_VERSION_NUMBER  
}PTP_PORT_DS;


// Data sets for transparent clocks
// NOTES:
//      1. This version of PTP does not implement transparent clocks!
//      2. Optionally, a transparent clock shall maintain a single copy of each default and port data sets.  
//      3. Unlike ordinary and boundary clocks, a transparent clock does not maintain separate data sets for each domain.
//         With the exception of syntonization, transparent clocks are domain independent. 

// transparentClockDefaultDS data set
typedef struct
{
    // static data members
    PTP_CLOCK_IDENTITY  clockIdentity;      // clockIdentity attribute of the local clock.
    uint16_t            numberPorts;        // the number of PTP ports of the device

    // configurable data members      
    uint8_t             delayMechanism;     // PTP_DELAY_MECHANISM:
                                            // PTP_DELAY_MECHANISM_E2E if the transparent clock is an end-to-end transparent clock;
                                            // PTP_DELAY_MECHANISM_P2P if the transparent clock is a peer-to-peer transparent clock.
    uint8_t             primaryDomain;      // PTP_DOMAIN_NUMBER: the domain number of the primary syntonization domain;
                                            // Initialization:
                                            //      The initialization value shall be 0. 
}PTP_TRANSPARENT_CLOCK_DEFAULT_DS;

// transparentClockPortDS data set 
typedef struct
{
    // static data members
    PTP_PORT_IDENTITY   portIdentity;               // PortIdentity attribute of the local port;

    // dynamic data members
    int8_t              logMinPdelayReqInterval;    // the logarithm to the base 2 of the minPdelayReqInterval
    bool                faultyFlag;                 // TRUE if the port is faulty and FALSE if the port is operating normally.
                                                    // Initialization:

    PTP_TIME_INTERVAL   peerMeanPathDelay;          // If the delay mechanism member is P2P, the value shall be the estimate of the current one-way propagation delay,
                                                    // i.e., <meanPathDelay> on the link attached to this port computed using the peer delay mechanism; 
                                                    // If the delayMechanism member is E2E, the value shall be zero.
                                                    // Initialization:
                                                    //      The initialization value shall be 0. 
}PTP_TRANSPARENT_CLOCK_PORT_DS;


// PTP transition events
// events transitioning the PTP state machine 
typedef enum
{
    PTP_EVENT_POWERUP,                              // power up or implementation-specific reset mechanisms

    PTP_EVENT_INITIALIZE,                           // receipt of the INITIALIZE management message
                                                    // or its equivalent if required by the initializationKey field of this message.  

    PTP_EVENT_DESIGNATED_ENABLED,                   // receipt of the ENABLE_PORT management message

    PTP_EVENT_DESIGNATED_DISABLED,                  // receipt of the DISABLE_PORT management message. 

    PTP_EVENT_FAULT_CLEARED,                        // clearing of the fault condition or conditions that prevents correct operation of the port.  
                                                    // Clearing of all detected fault conditions may result from
                                                    // the management messages or internal procedures. 

    PTP_EVENT_FAULT_DETECTED,                       // occurrence of an internal condition that prevents the correct operation of the port

    PTP_EVENT_STATE_DECISION_EVENT,                 // using the data in received Announce messages to determine which is the best master clock (BMC), 
                                                    // and whether the local clock port or ports needs to change state. 
                                                    // The occurrence of the STATE_DECISION_EVENT shall implement the logic involving BMC (Figure 25).
                                                    // The STATE_DECISION_EVENT shall: 
                                                    //      - Logically occur simultaneously on all ports of a clock 
                                                    //      - Occur at least once per Announce message transmission interval 
                                                    //      - Not occur when any port is in the INITIALIZING state 
                                                    //      For nodes implementing the default BMCA
                                                    //      prior to or as the first action of the STATE_DECISION_EVENT logic,
                                                    //      each port N (not in the DISABLED or FAULTY states)
                                                    //      shall compute an updated value of Erbest reflecting the receipt of Announce messages since the last 
                                                    //      STATE_DECISION_EVENT.
                                                    //      Following the computation of the set of Erbest for all ports, the clock shall compute Ebest. 
                                                    //  For all nodes, the clock shall complete the following tasks, in the order given: 
                                                    //      - Apply the best master clock algorithm 
                                                    //      - Update the appropriate data sets 
                                                    //      - Instantiate the recommended state event in the state machine 
                                                    //      - Make the required state changes in all ports 
                                                    //      These tasks shall be carried out atomically using information: Erbest

    PTP_EVENT_RECOMMENDED_STATE,                    //  this event results from the exercise of the BMCA initiated by a STATE_DECISION_EVENT. 

    PTP_EVENT_QUALIFICATION_TIMEOUT_EXPIRES,        // event triggered by the expiration of the <qualificationTimeoutInterval>
                                                    // This timeout mechanism determines the interval a clock spends in the PRE_MASTER state.
                                                    // The qualificationTimeoutInterval shall be N multiplied by the announceInterval in seconds
                                                    //      - If the recommended state = MASTER event was based on decision points M1/M2 of Figure 26:
                                                    //          N = 0
                                                    //      - If the recommended state = MASTER event was based on decision point M3 
                                                    //          N = currentDS.stepsRemoved + 1. 
                                                    
    PTP_EVENT_ANNOUNCE_RECEIPT_TIMEOUT_EXPIRES,     // Each protocol engine shall support a timeout mechanism defining the
                                                    // announceReceiptTimeoutInterval = portDS.announceReceiptTimeout * announceInterval;
                                                    // The ANNOUNCE_RECEIPT_TIMEOUT_EXPIRES event occurs at the expiration of this timeout plus a 
                                                    // random number uniformly distributed in the range (0,1) announceIntervals.  
                                                    // The mechanism restart procedures explained in 9.2.6.11
    PTP_EVENT_SYNCHRONIZATION_FAULT,                // event instantiation is implementation-specific.
                                                    // Triggered whenever a clock is in the SLAVE state and the implementation detects
                                                    // circumstances that require re-execution of functions that occur in the UNCALIBRATED state to ensure 
                                                    // correct synchronization.  

    PTP_EVENT_MASTER_CLOCK_SELECTED,                //  Implementation-specific.
                                                    // Should be instantiated whenever a clock in the UNCALIBRATED state has satisfied
                                                    // all implementation- and protocol-mandated requirements necessary to ensure synchronization when in the SLAVE state.
}PTP_TRANSITION_EVENT;

   
// Foreign Master DS
// Each port maintains the foreignMasterDS for qualifying the Announce messages
//
typedef struct
{
    PTP_PORT_IDENTITY   foreignMasterPortIdentity;          // the sourcePortIdentity field value of an Announce message
                                                            // received from the foreign master. 
    uint8_t             isValid;                            // boolean validity flag
    uint8_t             padding;                            // not used
    int                 foreignMasterAnnounceMessages;      // the number of Announce messages from this foreign master that have been
                                                            // received within a time window of duration FOREIGN_MASTER_TIME_WINDOW.

}PTP_FOREIGN_MASTER_DS;

// The values of these two attributes determine the criteria for accepting an Announce message from a 
// previously silent master clock for consideration in the operation of the best master clock algorithm.

// the number of a announceInterval
#define PTP_FOREIGN_MASTER_TIME_WINDOW          4

// Minimum # of Announce messages to be received within the FOREIGN_MASTER_TIME_WINDOW. 
#define PTP_FOREIGN_MASTER_THRESHOLD            2


// minimum number of foreign master records to store
#define PTP_FOREIGN_MASTER_DS_MIN_RECORDS       5

#if (TCPIP_PTP_FOREIGN_MASTER_RECORDS < PTP_FOREIGN_MASTER_DS_MIN_RECORDS)
#define PTP_FOREIGN_MASTER_DS_RECORDS PTP_FOREIGN_MASTER_DS_MIN_RECORDS
#else
#define PTP_FOREIGN_MASTER_DS_RECORDS TCPIP_PTP_FOREIGN_MASTER_RECORDS
#endif


// Section: PTP messages
//
// PTP message type
// Note: 4 bits!
typedef enum
{
    // event messages
    PTP_MESSAGE_TYPE_SYNC                   = 0,        // sync message 
    PTP_MESSAGE_TYPE_DELAY_REQ              = 1,        // delay request
    PTP_MESSAGE_TYPE_PDELAY_REQ             = 2,        // peer delay request 
    PTP_MESSAGE_TYPE_PDELAY_RESP            = 3,        // peer delay response 
    // general messages
    PTP_MESSAGE_TYPE_FOLLOW_UP              = 8,        // follow up 
    PTP_MESSAGE_TYPE_DELAY_RESP             = 9,        // delay response 
    PTP_MESSAGE_TYPE_PDELAY_RESP_FOLLOW_UP  = 0x0a,     // peer delay response follow up 
    PTP_MESSAGE_TYPE_ANNOUNCE               = 0x0b,     // announce message 
    PTP_MESSAGE_TYPE_SIGNALING              = 0x0c,     // signaling message
    PTP_MESSAGE_TYPE_MANAGEMENT             = 0x0d,     // management message

    // any other value is reserved!
}PTP_MESSAGE_TYPE;


// PTP control field - deprecated!
// Note: 8 bits!
typedef enum
{
    PTP_CONTROL_SYNC        = 0x00, 
    PTP_CONTROL_DELAY_REQ   = 0x01, 
    PTP_CONTROL_FOLLOW_UP   = 0x02, 
    PTP_CONTROL_DELAY_RESP  = 0x03, 
    PTP_CONTROL_MANAGEMENT  = 0x04, 
    PTP_CONTROL_ALL_OTHERS  = 0x05,

    // any other value is reserved!
}PTP_CONTROL_FIELD;


// PTP flag field
typedef union
{
    struct
    {
        uint16_t    alternateMasterFlag : 1;    // FALSE if the port of the originator is in the MASTER state.
        // Conditions to set the flag to TRUE are specified in 17.3 and 17.4. 
        // For Announce, Sync, Follow_Up, Delay_Resp messages

        uint16_t    twoStepFlag         : 1;    // For a one-step clock, the value of twoStepFlag shall be FALSE. 
        // For a two-step clock, the value of twoStepFlag shall be TRUE. 
        // For 0 1 Sync, Pdelay_Resp 

        uint16_t    unicastFlag         : 1;    // TRUE, if the transport layer protocol address to which
        // this message was sent is a unicast address. 
        // FALSE, if the transport layer protocol address to 
        // which this message was sent is a multicast address.  
        // For ALL messages

        uint16_t    reserved1           : 2;

        uint16_t    profileSpecific1    : 1;    // As defined by an alternate PTP profile;
        // otherwise FALSE. 
        // For ALL messages

        uint16_t    profileSpecific2    : 1;    // As defined by an alternate PTP profile;
        // otherwise FALSE. 
        // For ALL messages

        uint16_t    reserved2           : 1;


        uint16_t    leap61              : 1;    // The value of timePropertiesDS.leap61
        // For Announce messages


        uint16_t    leap59              : 1;    // The value of timePropertiesDS.leap59
        // For Announce messages

        uint16_t    currentUtcOffsetValid   : 1;    // The value of timePropertiesDS.currentUtcOffsetValid
        // For Announce messages

        uint16_t    ptpTimescale        : 1;    // The value of timePropertiesDS.ptpTimescale
        // For Announce messages

        uint16_t    timeTraceable       : 1;    // The value of timePropertiesDS.timeTraceable
        // For Announce messages

        uint16_t    frequencyTraceable  : 1;    // The value of timePropertiesDS.frequencyTraceable
        // For Announce messages

        uint16_t    reserved3           : 2;
    };
    uint16_t    val;
}PTP_FLAG_FIELD;

// PTP message header
//
typedef struct __attribute__((packed))
{
    uint8_t             transportSpecific   : 4;
    uint8_t             messageType         : 4;    // PTP_MESSAGE_TYPE value
    uint8_t             reserved_4          : 4;
    uint8_t             versionPTP          : 4;    // PTP_VERSION_NUMBER
    uint16_t            messageLength;
    uint8_t             domainNumber;       // PTP_DOMAIN_NUMBER value 
                                            // NOTE:  the message is accepted only if == defaultDS.domainNumber!
                                            // @@@@
    uint8_t             reserved_8; 
    PTP_FLAG_FIELD      flagField;          // PTP_FLAG_FIELD value 
    int64_t             correctionField;    // the value of the correction measured in nanoseconds and multiplied by 2^16.
                                            // For example, 2.5 ns is represented as 0x28000.
                                            // PTP_TIME_INTERVAL
                                            // A value of one in all bits, except the most significant, 
                                            // shall indicate that the correction is too big to be represented.
    
    uint8_t             reserved_32[4]; 
    PTP_PORT_IDENTITY   sourcePortIdentity;
    uint16_t            sequenceId;         // id pool: per message type and per destination!
                                            // exceptions for: Pdelay_Resp, Follow_Up, Delay_Resp, Pdelay_Resp_Follow_Up !
    uint8_t             controlField;       // a PTP_CONTROL_FIELD value
    int8_t              logMessageInterval;
}PTP_MESSAGE_HEADER;

// PTP generic message
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;         // PTP message header
    uint32_t            payload[0];     // message payload
}PTP_MESSAGE_GENERIC;

// PTP Announce Message
//
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;                     // PTP message header

    PTP_TIMESTAMP       originTimestamp;            // shall be 0 or an estimate no worse than ±1 s
                                                    // of the local time of the originating clock when
                                                    // the Announce message was transmitted.

    int16_t             currentUtcOffset;           // the value of the timePropertiesDS.currentUtcOffset member of the data set. 

    int8_t              reserved;                   // not used

    uint8_t             grandmasterPriority1;       // the value of the parentDS.grandmasterPriority1 member of the data set.  

    PTP_CLOCK_QUALITY   grandmasterClockQuality;    // the value of the parentDS.grandmasterClockQuality member of the data set.

    uint8_t             grandmasterPriority2;       // the value of the parentDS.grandmasterPriority2 member of the data set. 

    PTP_CLOCK_IDENTITY  grandmasterIdentity;        // the value of the parentDS.grandmasterIdentity member of the data set. 

    uint16_t            stepsRemoved;               // the value of currentDS.stepsRemoved of the data set of the clock issuing this message. 

    uint8_t             timeSource;                 // PTP_TIME_SOURCE: the value of the timePropertiesDS.timeSource member of the data set.
}PTP_MESSAGE_ANNOUNCE;


// flags for an announce message/Erbest/Ebest
typedef enum
{
    TCPIP_ANNOUNCE_FLAG_NONE            = 0x0000,     // no flag set
    TCPIP_ANNOUNCE_FLAG_EBEST_VALID     = 0x0001,     // flag used when the Announce message is an Erbest/Ebest
                                                      // signals Erbest/Ebest message computed and not empty

    // ....

}TCPIP_ANNOUNCE_FLAGS;

// announce message descriptor:
// the announce message + the receiving port number
typedef struct
{
    PTP_MESSAGE_ANNOUNCE    msg;
    PTP_FOREIGN_MASTER_DS*  pForeignDS;             // the foreign masterDS record this announce belongs to
    uint16_t                recvPortNumber;         // port number which received this message
                                                    // TODO aa: could be moved to foreignMasterDS?
    uint16_t                msgFlags;               // TCPIP_ANNOUNCE_FLAGS: associated message flags
}PTP_ANNOUNCE_MESSAGE_DCPT;

// Generic synchronization message (Sync of Follow_Up)
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;     // PTP header message
    PTP_TIMESTAMP       timestamp;  // message timestamp
}PTP_MESSAGE_SYNC_GENERIC;


// PTP Sync Message
//
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;                     // PTP header message
    PTP_TIMESTAMP       originTimestamp;            // one-step clocks:
                                                            // an estimate no worse than ±1 s of the <syncEventEgressTimestamp>
                                                            // excluding any fractional nanoseconds. 
                                                            // The sum of the Sync message correctionField and originTimestamp field shall 
                                                            // be the value of the <syncEventEgressTimestamp> including any fractional nanoseconds. 
                                                    // two-step clocks:
                                                        // shall be 0 or an estimate no worse than ±1 s of the <syncEventEgressTimestamp>. 
                                                        // The correctionField of the Sync message shall be set to 0. 
                                                        // A two-step clock shall transmit both a Sync and a Follow_Up message.
                                                        // The port shall capture the sequenceId value of the Sync message
                                                        // as an input to the sequenceId field of the Follow_Up message. 
                                                     
                                                    
}PTP_MESSAGE_SYNC;


// PTP Follow_Up message
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;                     // PTP header message
    PTP_TIMESTAMP       preciseOriginTimestamp;     // an estimate no worse than ±1 s of the <syncEventEgressTimestamp>
                                                    // of the associated Sync message excluding any fractional nanoseconds.
                                                    // The preciseOriginTimestamp field of the Follow_Up message should be the <syncEventEgressTimestamp> 
                                                    // of the associated Sync message excluding any fractional nanoseconds.
                                                    // The sum of the correctionField in the Follow_Up and associated Sync messages
                                                    // added to the preciseOriginTimestamp field of the Follow_Up message shall be
                                                    // the precise value of the <syncEventEgressTimestamp> of the associated Sync message 
                                                    // including any fractional nanoseconds. 
}PTP_MESSAGE_FOLLOW_UP;

// PTP Delay_Req Message
//
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;                     // PTP header message
    PTP_TIMESTAMP       originTimestamp;           
}PTP_MESSAGE_DELAY_REQ;

// PTP Delay_Resp Message
//
typedef struct __attribute__((packed))
{
    PTP_MESSAGE_HEADER  header;                     // PTP header message
    PTP_TIMESTAMP       receiveTimestamp;           // The receiveTimestamp shall be the <delayReqEventIngressTimestamp>
                                                    // of the associated Delay_Req message. 
    
    PTP_PORT_IDENTITY   requestingPortIdentity;
}PTP_MESSAGE_DELAY_RESP;


// local definitions that are not part of the standard
//


// descriptor of a PTP port on this clock
// each clock has TCPIP_PTP_PORTS ports
typedef struct
{
    struct _tag_TCPIP_PTP_CLOCK_DCPT*   pClock;       // clock this port belongs to
    PTP_PORT_DS                 portDS;     // corresponding port DS
    PTP_ANNOUNCE_MESSAGE_DCPT   Erbest;     // last computed Erbest on this port
    PTP_FOREIGN_MASTER_DS       foreignMasterDS[PTP_FOREIGN_MASTER_DS_RECORDS]; //  array of foreign master records data sets
                                                                            // for the purposes of qualifying Announce messages
    int64_t                     pathAsymmetry;  // the value of the path asymmetry measured in nanoseconds and multiplied by 2^16. 
                                                // @@@@ configuration param/#define but per port
    PTP_PORT_IDENTITY           followUpIdentity;   // the identity to wait for the follow up
    uint16_t                    followUpSequenceId; // the sequence Id to wait for the follow up
    uint16_t                    delayReqSequenceId; // the sequence Id used for Delay_Req and to wait for the Delay_Resp

    // pool of sequence IDs for different message types
    uint16_t                    sequenceId_Sync;     // for Sync messages
    uint16_t                    sequenceId_DelayReq; // for Delay_Req messages
    uint16_t                    sequenceId_PDelayReq; // for PDelay_Req messages : only if PDelay implemented!
    uint16_t                    sequenceId_Announce; // for Announce messages
    uint16_t                    sequenceId_Signaling; // for Signaling messages
    uint16_t                    sequenceId_Management; // for Management messages
}TCPIP_PTP_PORT_DCPT;

// describes this implementation CLOCK descriptor/characterization
typedef struct _tag_TCPIP_PTP_CLOCK_DCPT
{
    // this clock default DS
    // the numberPorts is 1 for an ordinary clock
    PTP_DEFAULT_DS              defaultDS;

    // Attributes related to synchronization. 
    PTP_CURRENT_DS              currentDS;          

    // Attributes describing the parent (the clock to which the ordinary clock synchronizes)
    // and the grandmaster (the clock at the root of the master-slave hierarchy)
    PTP_PARENT_DS               parentDS;

    // Attributes of the timescale for this clock 
    PTP_TIME_PROPERTIES_DS      timePropertiesDS;

    // last computed Ebest on this clock
    PTP_ANNOUNCE_MESSAGE_DCPT   Ebest;      // TODO aa: could it be just a pointer??? 

    // descriptor for each port on this clock
    TCPIP_PTP_PORT_DCPT         portDcpt[TCPIP_PTP_PORTS]; 

    // communication socket
    UDP_SOCKET                  clockSkt;
    
}TCPIP_PTP_CLOCK_DCPT;



#endif  // _PTP_PRIVATE_H_

