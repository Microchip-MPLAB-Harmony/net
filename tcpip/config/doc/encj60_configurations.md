# ENC28J60 MAC (ENC28J60)  Configurations

**ENC28J60 Specific User Configurations**


- **ENC28J60 Instance Number:**
    - The current ENCx24J600 hardware instance
    - Note:
        - This value is currently read-only
    - Default value: 0
    - Symbol INDEX

- **Number of Client Instances:**
    - This definition selects the Number of Client instances.
    - Default value: 1
    - Symbol DRV_ENC28J60_CLIENT_INSTANCES_IDX

- **Number of the TX Descriptors:**
    - Number of the TX descriptors to be created.
    - Default value: 2
    - Symbol DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX

- **Number of the RX Descriptors:**
    - Number of the RX descriptors to be created.
    - Default value: 2
    - Symbol DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX


- **RX Descriptor Buffer Size:**
    - Size of a RX packet buffer.
    - This is the size of all receive packet buffers processed by the ETH Controller.
    - The size should be enough to accommodate any network received packet.
    - Default value: 1536
    - Symbol DRV_ENC28J60_MAX_RX_BUFFER_IDX


- **RX Buffer Size (should be even):**
    - The total size of the RX space
    - Note: should be an even value  
    - Default value: 16384
    - Symbol DRV_ENC28J60_RX_BUFFER_SIZE_IDX


- **SPI Chip Select Pin:**
    - the I/O pin to be used for the SPI Chip Select
    - Default value: platform dependent
    - Symbol DRV_ENC28J60_SPI_CS_IDX

- **Maximum Frame Length:**
    - Maximum MAC supported RX/TX frame size.
    - The default value is 1536 (allows for VLAN tagged frames, although the VLAN tagged frames are discarded on RX).
    - On RX: any incoming ETH frame that is longer than this size will be discarded.
    - On TX: any frame that is longer than this size will be aborted by the MAC.
    - Default value: 1536
    - Symbol DRV_ENC28J60_MAX_FRAME_SIZE_IDX


- **Enable Automatic Flow Control:**
    - Enable MAC Auto Flow Control
    - When the auto flow control is enabled, the MAC will send pause frames whenever the number of pending RX packets reached the full watermark.
    - Once this number drops to the empty watermark, a pause frame with pause value of 0 is transmitted, resuming the normal traffic
    - Default value: true
    - Symbol DRV_ENC28J60_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX

