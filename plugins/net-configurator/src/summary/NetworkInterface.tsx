import React, { useState } from "react";
import {
  useBooleanSymbol,
  useComponent,
  useIntegerSymbol,
  useStringSymbol,
} from "@mplab_harmony/harmony-plugin-client-lib";
import { Divider } from "primereact/divider";
import { Badge } from "primereact/badge";
import "./summary.css";
import { Fieldset } from "primereact/fieldset";
const NetworkInterface = () => {
  const numberOfInterface = useIntegerSymbol({
    componentId: "tcpipStack",
    symbolId: `TCPIP_STACK_NET_INTERFACE_NUM`,
  });
  const instantiateComponent = useComponent("tcpipNetConfig");
  const InterfaceList = (props: { interfaceNumber: number }) => {
    const intrfaceName = useStringSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_NET_INTERFACE_NAME_IDX` + props.interfaceNumber,
    });
    const internalMAc = useBooleanSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_INT_MAC_IDX` + props.interfaceNumber,
    });
    const phyInterface = useStringSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_MII_MODE_IDX` + props.interfaceNumber,
    });

    const extrenalInterface = useStringSymbol({
      componentId: "tcpipStack",
      symbolId: `TCPIP_STACK_OTH_INT_IDX` + props.interfaceNumber,
    });

    return (
      <>
        <div className="network-list">
          <div className="network-list-item">Interface name </div>
          <div className="network-list-item"> {intrfaceName.value}</div>
        </div>
        <div className="network-list">
          <div className="network-list-item">Internal Mac</div>
          <div className="network-list-item">
            {" "}
            {internalMAc.value ? "YES" : "NO"}
          </div>
        </div>
        {!internalMAc.value && (
          <div className="network-list">
            <div className="network-list-item">External Interface </div>
            <div className="network-list-item"> {extrenalInterface.value}</div>
          </div>
        )}
        {internalMAc.value && (
          <div className="network-list">
            <div className="network-list-item">PHY Interface</div>
            <div className="network-list-item">{phyInterface.value}</div>
          </div>
        )}
      </>
    );
  };
  return (
    <>
      <Divider align="left">
        <Badge
          value={`Network Interface : ${numberOfInterface.value}`}
          size="large"
        ></Badge>
      </Divider>
      <div className="network-container">
        {instantiateComponent.instances.map((instance) => {
          const componentNumber: any = instance.componentId.split("_");
          return (
            <>
              <Fieldset
                legend={`Interface ${componentNumber[1]}`}
                style={{ width: "30%", textAlign: "left" }}
              >
                <InterfaceList interfaceNumber={componentNumber[1]} />
              </Fieldset>
            </>
          );
        })}
      </div>
    </>
  );
};

export default NetworkInterface;
