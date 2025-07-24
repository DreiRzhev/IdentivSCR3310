#ifndef _MCARDAPI_H
#define _MCARDAPI_H

#include "winscard.h"

//
// MCARD APIs Data Types
//

// 1. MCARDCONTEXT

typedef SCARDHANDLE			MCARDCONTEXT;
typedef MCARDCONTEXT*		PMCARDCONTEXT;


// 2. MCARDHANDLE 

typedef SCARDHANDLE MCARDHANDLE;
typedef MCARDHANDLE* PMCARDHANDLE;


// 3. MCARD_FEATURES

typedef struct _MCARD_FEATURES
{
	DWORD	dwFlags;
	BYTE	byMemoryZones;
	BYTE	byPINs;
	BYTE	byCounters;
	BYTE	byCRs;
} MCARD_FEATURES, *PMCARD_FEATURES;


// 4. MCARD_MEMORY

typedef struct _MCARD_MEMORY 
{
	DWORD	dwFLags;
	DWORD	dwSize;
} MCARD_MEMORY, *PMCARD_MEMORY;


// 5. MCARD_PIN

typedef struct _MCARD_PIN 
{
	DWORD	dwFlags;
	BYTE	bySize;
	BYTE	byRetries;
} MCARD_PIN, *PMCARD_PIN;


// 6. MCARD_CR

typedef struct _MCARD_CR 
{
	DWORD	dwFlags;
	DWORD	dwChallengeLen;
	DWORD	dwResponseLen;
	BYTE	byRetries;
} MCARD_CR, *PMCARD_CR;


// 7. MCARD_COUNTER

typedef struct _MCARD_COUNTER 
{
	DWORD	dwFLags;
	BYTE	dwSize;
	DWORD	dwUnits;
} MCARD_COUNTER, *PMCARD_COUNTER;

// Memory card connect modes
#define		FORCED_MODE						0x00
#define		INTELLIGENT_MODE				0x01
#define		KVK_MODE						0x02

// Memory card types
#define		MCARDTYPE_UNKNOWN				0x00
#define		MCARDTYPE_SLE4406				0x01
#define		MCARDTYPE_SLE4418				0x02
#define		MCARDTYPE_SLE4428				0x03
#define		MCARDTYPE_SLE4432				0x04
#define		MCARDTYPE_SLE4436				0x05
#define		MCARDTYPE_SLE4442				0x06
#define		MCARDTYPE_SLE5536				0x07
#define		MCARDTYPE_AT24C01ASC			0x08
#define		MCARDTYPE_AT24C02SC				0x09
#define		MCARDTYPE_AT24C04SC				0x0A
#define		MCARDTYPE_AT24C08SC				0x0B
#define		MCARDTYPE_AT24C16SC				0x0C
#define		MCARDTYPE_AT24C32SC				0x0D
#define		MCARDTYPE_AT24C64SC				0x0E
#define		MCARDTYPE_AT24C128SC			0x0F
#define		MCARDTYPE_AT24C256SC			0x10
#define		MCARDTYPE_AT24C512SC			0x11
#define		MCARDTYPE_AT88SC153				0x12
#define		MCARDTYPE_AT88SC1608			0x13
#define		MCARDTYPE_SLE4404				0x14
#define		MCARDTYPE_AT24C1024SC			0x15
#define		MCARDTYPE_AT88SC1003			0x16
#define		MCARDTYPE_ST1331				0x17
#define		MCARDTYPE_ST14C02C				0x18
#define	 	MCARDTYPE_SLE6636				0x19
#define 	MCARDTYPE_MCM2814				0x1A
#define		MCARDTYPE_ST1336				0x1B

#define		NUM_CARDS_SUPPORTED				(MCARDTYPE_ST1336 + 1)

//
// Memory card protocols 
//
#define		PROTOCOL_UNKNOWN				0x00
#define		PROTOCOL_2_WIRE					0x01
#define		PROTOCOL_3_WIRE					0x02
#define		PROTOCOL_IIC					0x03
#define		PROTOCOL_EXT_IIC				0x04
#define		PROTOCOL_AT88_IIC				0x05
#define		PROTOCOL_BIT			  		0x06
 
#define		PROTOCOL_GENERIC				0xFF


// Attribute IDs
#define		MCARD_ATTR_TYPE					0x00
#define		MCARD_ATTR_PROTOCOL				0x01
#define		MCARD_ATTR_FEATURES				0x02
#define		MCARD_ATTR_MEMORY				0x03
#define		MCARD_ATTR_PIN					0x04
#define		MCARD_ATTR_CR					0x05
#define		MCARD_ATTR_COUNTERS				0x06
#define		MCARD_ATTR_CLOCK				0x07
#define		MCARD_ATTR_BIT_ORDER			0x08
#define		MCARD_ATTR_CONFIGURATION		0x09

// Bit order
#define		LSB								0x00
#define		MSB								0x01
#define		DEFAULT							0xFF

// Memory card specific error codes
#define		SCARD_CODE_BASE					0x80100000
#define		COSTOMER_CODE_FLAG				0x20000000
#define		MCARD_OFFSET					0x800

// Offset for error codes		
#define		MCARD_E_OFFSET					0x0

// Offset for warning codes [SCARD_W_xx uses same]
#define		MCARD_W_OFFSET					0x65 

#define		MCARD_CODE_BASE     ((SCARD_CODE_BASE | COSTOMER_CODE_FLAG) + MCARD_OFFSET)
#define		MCARD_E_CODE_BASE   (MCARD_CODE_BASE + MCARD_E_OFFSET) // errors start here
#define		MCARD_W_CODE_BASE   (MCARD_CODE_BASE + MCARD_W_OFFSET) // warning start here


// MCARD_E_xxx error codes *************************************

// These codes indicate the "bad" errors
#define		MCARD_S_SUCCESS						 0x00

// 0xA0100801: an internal error has occured
#define		MCARD_E_INTERNAL_ERROR               MCARD_E_CODE_BASE + 0x1

// 0xA0100802: function not implemented
#define		MCARD_E_NOT_IMPLEMENTED              MCARD_E_CODE_BASE + 0x2

// 0xA0100803: MCardInitialize not called
#define		MCARD_E_NOT_INITIALIZED              MCARD_E_CODE_BASE + 0x3

// 0xA0100804: this DLL does not work with the specified reader
#define		MCARD_E_INCOMPATIBLE_READER          MCARD_E_CODE_BASE + 0x4

// 0xA0100805: could not identify card
#define		MCARD_E_UNKNOWN_CARD                 MCARD_E_CODE_BASE + 0x05

// 0xA0100811: the buffer for return daa is too small
#define		MCARD_E_BUFFER_TOO_SMALL             MCARD_E_CODE_BASE + 0x11

// 0xA0100812: one or more parameters are invalid
#define     MCARD_E_INVALID_PARAMETER            MCARD_E_CODE_BASE + 0x12

// 0xA0100821: protocoll error while connecting to card
#define		MCARD_E_PROTO_MISMATCH               MCARD_E_CODE_BASE + 0x21

// 0xA0100822: protocol error during card access
#define		MCARD_E_PROTOCOL_ERROR               MCARD_E_CODE_BASE + 0x22

// 0xA0100823: Challenge Response Failed
#define		MCARD_E_CHAL_RESP_FAILED             MCARD_E_CODE_BASE + 0x23

// 0xA0100824: Invalid memory range
#define		MCARD_E_INVALID_MEMORY_RANGE         MCARD_E_CODE_BASE + 0x24

// 0xa0100831: specified memory zone ID is invalid for current card
#define		MCARD_E_INVALID_MEMORY_ZONE_ID       MCARD_E_CODE_BASE + 0x31

// 0A0100832: specified PIN ID is invalid for current card
#define		MCARD_E_INVALID_PIN_ID               MCARD_E_CODE_BASE + 0x32

// 0xA0100833: spezcified challenge/response ID is invalid for current card
#define		MCARD_E_INVALID_CHAL_RESP_ID         MCARD_E_CODE_BASE + 0x33

//0xA0100834:Write not possible erasure to be done first
#define		MCARD_E_ERASURE_NEEDED				 MCARD_E_CODE_BASE + 0x34

//0xA0100835:Bit order changed from default
#define		MCARD_E_BITORDER_CHANGED			 MCARD_E_CODE_BASE + 0x35

// MCARD_W_xxx warning codes *************************************************
// These codes indicate that a problem occured, but it's up to the
//   application to decide how bad it is

// 0xA0100866: could not read all data from card
#define		MCARD_W_NOT_ALL_DATA_READ            MCARD_W_CODE_BASE + 0x1

// 0xA0100867: could not write all data to card
#define		MCARD_W_NOT_ALL_DATA_WRITTEN         MCARD_W_CODE_BASE + 0x2

// 0xA0100876: PIN must be verified before access is possible
#define		MCARD_W_PIN_VERIFY_NEEDED            MCARD_W_CODE_BASE + 0x11

// 0xA0100877: PIN verification failed
#define		MCARD_W_PIN_VERIFY_FAILED            MCARD_W_CODE_BASE + 0x12

// 0xA0100878: no PIN verification attempts left, card probably locked
#define		MCARD_W_NO_PIN_ATTEMPTS_LEFT         MCARD_W_CODE_BASE + 0x13

// 0xA0100879: no units left in the card to decrement
#define		MCARD_W_NO_UNITS_TO_DECREMENT        MCARD_W_CODE_BASE + 0x14

// 0xA0100886: The card has been removed
#define		MCARD_W_REMOVED_CARD				 MCARD_W_CODE_BASE + 0x21

// 0xA0100896: Timeout occurred
#define		MCARD_W_TIMEOUT						 MCARD_W_CODE_BASE + 0x31

// 0xA0100897: Command Aborted
#define		MCARD_W_ABORTED						 MCARD_W_CODE_BASE + 0x32

#define		MCARD_W_PROTECTED_AREA				 MCARD_W_CODE_BASE + 0x33

#define 	MCARD_W_FUSE_BLOWN					 MCARD_W_CODE_BASE + 0x34


#ifdef MCSCM_EXPORTS
#define MCSCM_API __declspec(dllexport)
#else
#define MCSCM_API __declspec(dllimport)
#endif


#define OUT
#define IN



// Export functions for the Memory card application

MCSCM_API LONG WINAPI MCardInitialize(
	IN	SCARDCONTEXT  hSCardContext,
	IN  LPCTSTR szReaderName,
    OUT PMCARDCONTEXT phMCardContext,
    OUT PDWORD pdwDllVersion
    );


MCSCM_API LONG WINAPI MCardShutdown(
    IN MCARDCONTEXT hMCardContext
    );


MCSCM_API LONG WINAPI MCardConnect(
    IN MCARDCONTEXT hMCardContext,
    IN DWORD dwConnectMode,
	IN BYTE byCardType,
    OUT PMCARDHANDLE phMCard
    );


MCSCM_API LONG WINAPI MCardDisconnect(
    IN MCARDHANDLE hMCard,
    IN DWORD dwDisposition
    );

MCSCM_API LONG WINAPI MCardWaitForCardState(
    IN  MCARDCONTEXT hMCardContext,
    IN  DWORD        dwExpectedCardState,
    OUT DWORD*       pdwCardState,
    IN  DWORD        dwTimeOut
    );

MCSCM_API LONG WINAPI MCardGetAttrib(
    IN MCARDHANDLE hMCard,
    IN DWORD dwAttrId,
    OUT LPBYTE pbAttr,
    IN OUT LPDWORD pcbAttrLen
    );


MCSCM_API LONG WINAPI MCardSetAttrib(
    IN MCARDHANDLE hMCard,
    IN DWORD dwAttrId,
    IN LPBYTE pbAttr,
    IN DWORD cbAttrLen
    );

MCSCM_API LONG WINAPI MCardReadMemory(
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN LPBYTE pbReadBuffer,
    IN OUT LPDWORD pbReadLen
    );


MCSCM_API LONG WINAPI MCardWriteMemory(
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN LPBYTE pbWriteBuffer,
    IN OUT LPDWORD pcbWriteLen
    );


MCSCM_API LONG WINAPI MCardSetMemoryWriteProtection(
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN OUT LPDWORD pbcLen
    );


MCSCM_API LONG WINAPI MCardSetMemoryReadProtection(
    MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN OUT LPDWORD pbLen
    );


MCSCM_API LONG WINAPI MCardReadMemoryStatus(
    MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    OUT PBYTE pbBuffer,
    IN OUT LPDWORD pcbLen
    );


 
MCSCM_API LONG WINAPI MCardVerifyPIN(
    IN MCARDHANDLE hMCard,
    IN BYTE bPinNumber,
    IN PBYTE pbBufferWithPIN,
    IN BYTE pbcLen
    );

MCSCM_API LONG WINAPI MCardChangePIN(
    IN MCARDHANDLE hMCard,
    IN BYTE bPinNumber,
    IN PBYTE pbBufferWithOldPIN,
    IN BYTE cbOldLen,
    IN PBYTE pbBufferWithNewPIN,
    IN BYTE cbNewLen
    );



MCSCM_API LONG WINAPI MCardChallengeResponse(
    IN MCARDHANDLE hMCard,
    IN BYTE bChallengeID,
    IN PBYTE pbChallengeBuffer,
    IN BYTE cbChallengeLen,
    OUT PBYTE pbResponseBuffer,
    OUT PBYTE cbResponseLen
    );


MCSCM_API LONG WINAPI MCardDeductCounter(
    IN MCARDHANDLE hMCard,
	IN BYTE bCounterID,
    IN DWORD dwUnits
    );

MCSCM_API LONG WINAPI MCardSetCounter(
    IN MCARDHANDLE hMCard,
    IN BYTE bCounterID,
    IN PBYTE pbCounter,	
    IN BYTE cbCounterLen
    );

MCSCM_API LONG WINAPI MCardVerifyOnOff(
	IN BOOL bVerify
	);

MCSCM_API LONG WINAPI MCardReadFuse(
    IN MCARDHANDLE hMCard,
	IN LPBYTE pbyReadBuffer
	);

MCSCM_API LONG WINAPI MCardBlowFuse(
    IN MCARDHANDLE	hMCard,
	IN BYTE			byFuseIndex
	);


MCSCM_API LONG WINAPI MCard4436BlowFuseOnOff(
	IN BOOL bBlow4436Fuse
	);


MCSCM_API LONG WINAPI MCARD4404SetTestMode ( IN MCARDHANDLE	hMCard,  IN BOOL bOn);

#endif
