import React, { useEffect } from "react";
import OverViewLayer from "./OverViewLayer";
import "./overview.css";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import { initBasicConfigLayer } from "../tcpip-layer/createGroupComponet";
import { compare } from "../util/sorting";
type Props = {
  componentIds: string[];
};
const OverViewLayerList = ({ componentIds }: Props) => {
  const components = useComponents(componentIds).sort(compare);
  useEffect(() => {
    // initBasicConfigLayer()
    
   }, []);
  return (
    <div className="main-container">
      <div className="overview-container">
        <OverViewLayer
          label="Application Layer"
          activatedComponents={components.filter(
            (c) => c.displayType === "Application Layer"
          )}
          labelColor="#dd5746"
        />
        <OverViewLayer
          label="Presentation Layer"
          activatedComponents={components.filter(
            (c) => c.displayType === "Presentation Layer"
          )}
          labelColor="#ffc470"
        />
        <OverViewLayer
          label="Transport Layer"
          activatedComponents={components.filter(
            (c) => c.displayType === "Transport Layer"
          )}
          labelColor="#4ccd99"
        />
        <OverViewLayer
          label="Network Layer"
          activatedComponents={components.filter(
            (c) => c.displayType === "Network Layer"
          )}
          labelColor="#374259"
        />
        <OverViewLayer
          label="Data Link Layer"
          activatedComponents={components
            .filter(
              (c) =>
                c.displayType === "PHY Layer" ||
                c.displayType === "MAC Layer" ||
                (c.displayType === "TCP/IP Library" &&
                  c.displayName === "NETCONFIG")
            )
           }
          labelColor="#5bbcff"
        />
        <OverViewLayer
          label="Basic Configuration"
          activatedComponents={components.filter(
            (c) =>
              c.displayType === "TCP/IP Library" &&
              c.displayName !== "NETCONFIG"
          )}
          labelColor="#7f9f80"
        />
      </div>
    </div>
  );
};

export default OverViewLayerList;
