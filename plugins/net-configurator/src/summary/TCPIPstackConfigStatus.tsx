import React from "react";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import { Divider } from "primereact/divider";
import { Badge } from "primereact/badge";
import "./summary.css";
import { compare } from "../util/sorting";
import DependencyStatus from "./DependencyStatus";
type Props = {
  componentIds: string[];
};
const TCPIPstackConfigStatus = ({ componentIds }: Props) => {
  const components = useComponents(componentIds).sort(compare);
  const applicationComponents = components.filter(
    (c) => c.displayType === "Application Layer" && c.isActive
  );
  const networkComponents = components.filter(
    (c) => c.displayType === "Network Layer" && c.isActive
  );
  const basicConfigComponents = components.filter(
    (c) =>
      c.displayType === "TCP/IP Library" &&
      c.displayName !== "NETCONFIG" &&
      c.isActive
  );
  const presentationComponents = components.filter(
    (c) => c.displayType === "Presentation Layer" && c.isActive
  );
  const transportComponents = components.filter(
    (c) => c.displayType === "Transport Layer" && c.isActive
  );
  const dataLinkComponents = components.filter(
    (c) =>
      (c.displayType === "PHY Layer" ||
        c.displayType === "MAC Layer" ||
        (c.displayType === "TCP/IP Library" &&
          c.displayName === "NETCONFIG")) &&
      c.isActive
  );
  return (
    <>
      <Divider align="left">
        <Badge value="TCP/IP Stack Configuration Status" size="large"></Badge>
      </Divider>
      <div className="ip-layer-main-container">
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-stack-header">TCP/IP Layers</div>
          <div className="ip-layer-stack-header">Enabled TCP/IP Module</div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Application</div>
          <div className="ip-layer-list">
            {applicationComponents.length > 0 ? (
              applicationComponents.map((c) => {
                return (
                  <Badge
                    key={c.displayName}
                    value={c.displayName}
                    size="large"
                    severity="success"
                  ></Badge>
                );
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Presentation</div>
          <div className="ip-layer-list">
            {presentationComponents.length > 0 ? (
              presentationComponents.map((c) => {
                return (
                  <Badge
                    key={c.displayName}
                    value={c.displayName}
                    size="large"
                    severity="success"
                  ></Badge>
                );
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Transport</div>
          <div className="ip-layer-list">
            {transportComponents.length > 0 ? (
              transportComponents.map((c) => {
                return (
                  <Badge
                    key={c.displayName}
                    value={c.displayName}
                    size="large"
                    severity="success"
                  ></Badge>
                );
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Network</div>
          <div className="ip-layer-list">
            {networkComponents.length > 0 ? (
              networkComponents.map((c) => {
                return (
                  <Badge
                    key={c.displayName}
                    value={c.displayName}
                    size="large"
                    severity="success"
                  ></Badge>
                );
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Data Link & Physical</div>
          <div className="ip-layer-list">
            {dataLinkComponents.length >= 2 &&
            (dataLinkComponents[0]?.instances?.length !== 0 ||
              dataLinkComponents[1]?.instances?.length !== 0) ? (
              dataLinkComponents.map((c) => {
                if (c?.instances?.length >= 0) {
                  return c?.instances?.map((a) => {
                    let componentName = a.displayName;
                    if (!isNaN(parseInt(a.componentId.split("_")[1]))) {
                      componentName = `${a.displayName}-${
                        a.componentId.split("_")[1]
                      }`;
                    }
                    return (
                      <Badge
                        key={componentName}
                        value={componentName}
                        size="large"
                        severity="success"
                      ></Badge>
                    );
                  });
                } else {
                  return (
                    <Badge
                      key={c.displayName}
                      value={c.displayName}
                      size="large"
                      severity="success"
                    ></Badge>
                  );
                }
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
        <div className="ip-layer-ipstack-container">
          <div className="ip-layer-header">Basic Configuration</div>
          <div className="ip-layer-list">
            {basicConfigComponents.length > 0 ? (
              basicConfigComponents.map((c) => {
                return (
                  <Badge
                    key={c.displayName}
                    value={c.displayName}
                    size="large"
                    severity="success"
                  ></Badge>
                );
              })
            ) : (
              <div>None</div>
            )}
          </div>
        </div>
      </div>
      <DependencyStatus componentIds={componentIds} />
    </>
  );
};

export default TCPIPstackConfigStatus;
