import React, { useEffect, useRef } from "react";
import { ContextMenu } from "primereact/contextmenu";
import useLoading from "../tcpip-layer/useLoading";
import { activateComponent } from "../tcpip-layer/activateComponent";
import Loader from "./LoaderComponent";

type Props = {
  cm: any;
  requiredSatList: {
    componentId: string;
    displayType: string;
    displayName: string;
  }[];
  optionalSatList: {
    componentId: string;
    displayType: string;
    displayName: string;
  }[];
};
const parentGroupName = [
  "APPLICATION LAYER",
  "PRESENTATION LAYER",
  "TRANSPORT LAYER",
  "NETWORK LAYER",
  "DATA LINK LAYER",
  "BASIC CONFIGURATION",
  "PHY Layer",
  "MAC Layer",
  "TCP/IP Library"
];
const OverlayPanelComponent = ({
  cm,
  requiredSatList,
  optionalSatList,
}: Props) => {
  const { loading, showLoader, lodingText } = useLoading();
  const requiredItem = requiredSatList.map((item) => {
    return {
      label: `${item.displayName}(${item.componentId})`,
      icon: (
        <i
          className="pi pi-circle-fill"
          style={{ color: "rgb(244, 91, 91)", marginRight: "5px" }}
        ></i>
      ),
      ...item,
      command: () => {
        activate(item.displayType, item.componentId);
      },
    };
  });
  const optionalItem = optionalSatList.map((item) => {
    return {
      label: `${item.displayName}(${item.componentId})`,
      icon: (
        <i
          className="pi pi-circle-fill"
          style={{ color: "#FA9F0A", marginRight: "5px" }}
        ></i>
      ),
      ...item,
      command: () => {
        activate(item.displayType, item.componentId);
      },
    };
  });
  const items = [
    ...requiredItem,
    {
      separator: true,
    },
    ...optionalItem,
  ];

  const handleScroll = () => {
    cm.current?.hide();
  };
  const handleContextMenu = () => {
    cm.current?.hide();
  };
  useEffect(() => {
    window.addEventListener("scroll", handleScroll, true);
    window.addEventListener("contextmenu", handleContextMenu, true);
    return () => {
      window.removeEventListener("scroll", handleScroll, true);
      window.removeEventListener("contextmenu", handleContextMenu, true);
    };
  }, []);
  const activate = async (groupName: string, componentId: string) => {
    const dispType =
      groupName === "PHY Layer" || groupName === "MAC Layer"||groupName==="TCP/IP Library"
        ? "DATA LINK LAYER"
        : groupName.toUpperCase();
       if(parentGroupName.includes(dispType)){
        showLoader([componentId], true);
        await activateComponent(dispType, [componentId]).then(() => {});
       }
       else{
        showLoader([componentId], true);
        await activateComponent('__ROOTVIEW', [componentId]).then(() => {});
       }
       
    
  };
  const emptyMenu = [{ label: "No Satisfiers Available!" }];
  return (
    <>
      <div>
        <ContextMenu
          model={items.length > 1 ? items : emptyMenu}
          ref={cm}
          breakpoint="767px"
          style={{ width: "auto" }}
        />
      </div>
      <Loader visible={loading} text={lodingText} />
    </>
  );
};

export default OverlayPanelComponent;
