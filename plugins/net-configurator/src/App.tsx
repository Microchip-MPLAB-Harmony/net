import { useCallback, useEffect, useState } from "react";
import SidebarMenu from "./sidebar-component/sidebarMenu";
import { IMenuInterface } from "./interfaces";
import OverViewLayerList from "./overview/OverViewLayerList";
import ConfigSummary from "./summary/ConfigSummary";
import ApplicationLayer from "./tcpip-layer/ApplicationLayer";
import PresentationLayer from "./tcpip-layer/PresentationLayer";
import NetworkLayer from "./tcpip-layer/NetworkLayer";
import TransportLayer from "./tcpip-layer/TransportLayer";
import BasicConfigLayer from "./tcpip-layer/BasicConfig";
import DataLinkLayer from "./tcpip-layer/DataLinkLayer";
import { Component, componentUtilApi, useComponent, useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import { initGroupComponent } from "./tcpip-layer/createGroupComponet";
const arr = ["tcpipNetConfig"]
function App() {
  const [components, setComponents] = useState<Component[]>([]);
  const [componentIds,setComponentIds]=useState<string[]>([])
  const [selectedSection, setSelectedSection] = useState<IMenuInterface>();
  const [networkComponentIds,setNetworkComponentIds] = useState<string[]>([])
  const [applicationComponentIds,setApplicationComponentIds] = useState<string[]>([])
  const [dataLinkComponentIds,setdataLinkComponentIds] = useState<string[]>([])
  const [presentationComponentIds,setPresentationComponentIds] = useState<string[]>([])
  const [transportComponentIds,setTransportComponentIds] = useState<string[]>([])
  const [basicConfigComponentIds,setBasicConfigComponentIds] = useState<string[]>([])
  const [macComponentIds, setMacComponentIds] = useState<string[]>([]);
  const [phyComponentIds, setPhyComponentIds] = useState<string[]>([]);
  const [instantiateComponentId, setInstantiateComponentId] = useState<string[]>([]);
  const [reload,setReload]=useState()
  useEffect(() => {
    initGroupComponent()
    componentUtilApi
      .getAllComponents()
      .then((e) => setComponents(e))
  }, []);

  useEffect(()=>{
    setComponentIds(components.map((c=>c.componentId)))
   setApplicationComponentIds(components.filter((c)=>c.displayType==="Application Layer").map((c)=>c.componentId))
   setPresentationComponentIds(components.filter((c)=>c.displayType==="Presentation Layer").map((c)=>c.componentId))
   setTransportComponentIds(components.filter((c)=>c.displayType==="Transport Layer").map((c)=>c.componentId))
   setNetworkComponentIds(components.filter((c)=>c.displayType==="Network Layer").map((c)=>c.componentId))
   setBasicConfigComponentIds(components.filter((c)=>c.displayType==="TCP/IP Library" && c.displayName !== "NETCONFIG").map((c)=>c.componentId))
   setdataLinkComponentIds(components.filter((c)=>c.displayType==="PHY Layer" || "MAC Layer").map((c)=>c.componentId))
   setMacComponentIds(components.filter((c)=>c.displayType==="MAC Layer").map((c)=>c.componentId))
   setPhyComponentIds(components.filter((c)=>c.displayType==="PHY Layer").map((c)=>c.componentId))
   setInstantiateComponentId(components.filter((c)=>c.displayType === "TCP/IP Library" && c.displayName === "NETCONFIG").map((c)=>c.componentId))
  },[components])

  const onHandleMenuClick = useCallback((menuItem: any) => {
    setSelectedSection(menuItem);
  }, []);

  return (
    <div className="app-container">
      <div className="app-sidebar">
        <SidebarMenu onHandleMenuClick={onHandleMenuClick} />
      </div>
      {selectedSection?.name === "Config Summary" && <ConfigSummary componentIds={componentIds} />}
      {selectedSection?.name === "Application" && (
        <ApplicationLayer componentIds={applicationComponentIds}  />
      )}
      {selectedSection?.name === "Presentation" && (
        <PresentationLayer componentIds={presentationComponentIds} />
      )}
      {selectedSection?.name === "Network" && (
        <NetworkLayer componentIds={networkComponentIds} />
      )}
      {selectedSection?.name === "Transport" && (
        <TransportLayer componentIds={transportComponentIds} />
      )}
      {selectedSection?.name === "Basic Config" && (
        <BasicConfigLayer componentIds={basicConfigComponentIds} />
      )}
      {selectedSection?.name === "Data Link" && <DataLinkLayer shouldReload={{reload,setReload}} macComponentIds={macComponentIds} phyComponentIds={phyComponentIds} instantiateComponentId={instantiateComponentId} />}
      {(selectedSection?.name === "Overview" ||
        selectedSection === undefined) && <OverViewLayerList componentIds={componentIds}  />}
    </div>
  );
}

export default App;
