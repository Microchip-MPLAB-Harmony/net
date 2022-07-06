# TCPIP\_TFTPS\_IsEnabled Function

**Parent topic:**[TFTP Server Module](GUID-D76DC993-4CD3-4C65-92DB-14DEAB57BB27.md)

## C

```
bool TCPIP_TFTPS_IsEnabled(); 
```

## Returns

true : if connection is established false: if server open fails.

## Description

This function determines if a TFTP server socket connection is established and ready to receive packet from the different client.

## Remarks

None.

## Preconditions

The TFTPS module should have been initialized.

