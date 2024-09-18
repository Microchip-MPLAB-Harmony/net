import React, { useState } from "react";
import {
  symbolUtilApi,
  useBooleanSymbol,
  useComboSymbol,
  useHexSymbol,
  useIntegerSymbol,
  useKeyValueSetSymbol,
  useStringSymbol,
} from "@mplab_harmony/harmony-plugin-client-lib";
import { Divider } from "primereact/divider";
import { Badge } from "primereact/badge";
import "./summary.css";
import useInternalMacConfig from "./useInternalMacConfig";

const SystemConfig = () => {
  const [isInternalMac, setIsInternalMac] = useState(false);
  const clockFrequency = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: `TCPIP_STACK_MAC_CLOCK`,
  });
  const numberOfInterrupts = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: `TCPIP_STACK_MAC_INTERRUPT_NUM`,
  });
  const interruptArray: string[] = new Array(numberOfInterrupts.value).fill(
    "0"
  );

  const compilerChoice = useKeyValueSetSymbol({
    componentId: "core",
    symbolId: `COMPILER_CHOICE`,
  });
  const xc32 = useIntegerSymbol({
    componentId: "core",
    symbolId: `XC32_HEAP_SIZE`,
  });
  const iar = useIntegerSymbol({
    componentId: "core",
    symbolId: `IAR_HEAP_SIZE`,
  });
  const keil = useIntegerSymbol({
    componentId: "core",
    symbolId: `KEIL_HEAP_SIZE`,
  });

  const InterruptsList = (props: { interruptIndex: number }) => {
    const gmac = useStringSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_INTERRUPT_NAME_IDX${props.interruptIndex}`,
    });
    const vector = useIntegerSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_INTERRUPT_VECTOR_IDX${props.interruptIndex}`,
    });

    const interrupt = useBooleanSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_INTERRUPT_EN_IDX${props.interruptIndex}`,
    });

    return (
      <div className="interrupt-list ">
        <div className="interrupt-list-header">
          {`${gmac.value} (Vector ${vector.value})`}
        </div>
        <div className="interrupt-list-value">
          {interrupt.value ? "Enabled" : "Disabled"}
        </div>
      </div>
    );
  };

  const dramSize = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_DRAM_SIZE",
  });
  const poolExSize = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_POOL_EXPANSION_SIZE",
  });

  const recommendedSize = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_HEAP_SIZE_RECOMMEND",
  });

  const nonCachebleMemory = useBooleanSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_NO_CACHE_CONFIG",
  });
  const startAddress = useHexSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_NOCACHE_MEM_ADDRESS",
  });
  const memorySize = useStringSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_NOCACHE_SIZE",
  });

  const heapCong = useComboSymbol({
    componentId: "tcpipStack",
    symbolId: "TCPIP_STACK_USE_HEAP_CONFIG",
  });

  const internalMAcList = useInternalMacConfig();
  const checkForInterlaMacActive = async () => {
    const symbols = await symbolUtilApi.getSymbols(
      "tcpipStack",
      internalMAcList.internalMac
    );
    symbols.map((mac) => {
      if (mac.value) {
        setIsInternalMac(true);
      }
    });
  };
  checkForInterlaMacActive();
  return (
    <>
      <Divider align="left">
        <Badge value={`System Configuration`} size="large"></Badge>
      </Divider>
      <div style={{marginLeft:'30px'}}>
      {isInternalMac && (
        <div >
          <div className="interrupt-header">
            <div className="interrupt-list-header">{clockFrequency.label}</div>
            <div className="interrupt-list-value">
              {`${clockFrequency.value}  Hz`}
            </div>
          </div>
          <div style={{ textAlign: "left" }}>
            <h2>MAC Peripheral Interrupts</h2>
          </div>
          <div className="interrupt-header">
            <div className="interrupt-list-header">
              {numberOfInterrupts.label}
            </div>
            <div className="interrupt-list-value">
              {numberOfInterrupts.value}
            </div>
          </div>

          {interruptArray.map((j, index) => {
            return <InterruptsList interruptIndex={index} />;
          })}
        </div>
      )}

      <div style={{ textAlign: "left" }}>
        <h2>Heap Size</h2>
      </div>

      <div className="interrupt-header">
        <div className="interrupt-list-header">System Heap Size</div>
        <div className="interrupt-list-value">
          {compilerChoice.selectedOption === "XC32"
            ? xc32.value + " Bytes"
            : ""}
          {compilerChoice.selectedOption === "IAR" ? iar.value + " Bytes" : ""}
          {compilerChoice.selectedOption === "KEIL"
            ? keil.value + " Bytes"
            : ""}
        </div>
      </div>
      <div className="interrupt-header">
        <div className="interrupt-list-header">Configured TCP/IP Heap Size</div>
        {heapCong.value === "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP" && (
          <div className="interrupt-list-value">{dramSize.value} Bytes</div>
        )}
        {heapCong.value === "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" && (
          <div className="interrupt-list-value">{poolExSize.value} Bytes</div>
        )}
      </div>
      <div className="interrupt-header">
        <div className="interrupt-list-header">
          Recommended TCP/IP Heap Size
        </div>
        <div className="interrupt-list-value">
          {recommendedSize.value} Kbytes
        </div>
      </div>

      {nonCachebleMemory.value && (
        <>
          <div style={{ textAlign: "left" }}>
            <h2>Non-Cacheable Memory</h2>
          </div>

          <div className="interrupt-header">
            <div className="interrupt-list-header">Start Address</div>
            <div className="interrupt-list-value">0x{startAddress.value}</div>
          </div>
          <div className="interrupt-header">
            <div className="interrupt-list-header">Memory Size</div>
            <div className="interrupt-list-value">{memorySize.value} Bytes</div>
          </div>
        </>
      )}
      </div>
    </>
  );
};

export default SystemConfig;
