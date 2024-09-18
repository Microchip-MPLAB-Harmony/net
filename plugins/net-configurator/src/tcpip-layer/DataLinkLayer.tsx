import React, {
  LegacyRef,
  MouseEvent,
  RefObject,
  useEffect,
  useRef,
  useState,
} from "react";
import { Panel } from "primereact/panel";
import {
  Component,
  Logger,
  useComponent,
} from "@mplab_harmony/harmony-plugin-client-lib";
import { componentUtilApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import TCPIPComponentDefault from "../common-component/TCPIPComponentDefault";
import AvailableHeader from "../common-component/AvailableHeader";
import ActiveHeader from "../common-component/ActiveHeader";
import { retrieveUnique, setUnsetDraggingColor } from "../util/retrieveUnique";
import { Card } from "primereact/card";
import { Button } from "primereact/button";
import ActiveDataLinkComponentDefault from "../common-component/ActiveDataLinkComponentDefault";
import { activateComponent } from "./activateComponent";
import { initBasicConfigLayer, initDataLinkLayer } from "./createGroupComponet";
import { useXarrow, Xwrapper } from "react-xarrows-updated";
import "./dataLinkLayer.css";
import DataLinkAttachment from "./DataLinkAttachmet";
import { useRenderCounter } from "../util/useCounter";
import AttachmentCardWrapper from "../common-component/ConnectableCard";
import WarningComponent from "../common-component/WarningComponent";
import Loader from "../common-component/LoaderComponent";
import useLoading from "./useLoading";
import {compare} from '../util/sorting'
const log = new Logger("DataLinkLayer");
type Props = {
  macComponentIds: string[];
  phyComponentIds: string[];
  instantiateComponentId: string[];
  shouldReload: any;
};
const DataLinkLayer = ({ macComponentIds, phyComponentIds }: Props) => {
  const macComponents = useComponents(macComponentIds).sort(compare);
  const phyComponents = useComponents(phyComponentIds).sort(compare);
  const instantiateComponent = useComponent("tcpipNetConfig");
  const [sourceContainer, setSourceContainer] = useState<
    "macAvailable" | "phyAvailable"
  >();
  const [targetContainer, setTargetContainer] = useState<
    "macActive" | "phyActive"
  >();
  const [selectedComponent, setSelectedComponent] = useState<string[]>([]);
  const [visible, setVisible] = useState<boolean>(false);
  const [deactivateVisible, setDeactivateVisible] = useState<boolean>(false);
  const { loading, showLoader, lodingText } = useLoading();
  const [selectedComponentDeactivate, setSelectedComponentDeactivate] =
    useState<string[]>([]);
  const updateArrow = useXarrow();
  const { executeCallback } = useRenderCounter();
  useEffect(() => {
    initDataLinkLayer();
    executeCallback(updateArrow);
    initBasicConfigLayer();
  }, []);
  useEffect(() => {
    const newMacComponents: Component[] = [];
    const macCompo = macComponents.map((mac) => {
      if (mac?.instances?.length > 0) {
        mac.instances.map((m) => {
          newMacComponents.push(m);
        });
      } else {
        newMacComponents.push(mac);
      }
    });
    const toBeDeactivated = [
      ...newMacComponents,
      ...phyComponents,
      ...instantiateComponent.instances,
    ]
      .filter((e) => e.isActive)
      .filter((e) => e.isSelected)
      .map((e) => e.componentId);
    setSelectedComponentDeactivate(toBeDeactivated);
  }, [macComponents, phyComponents, instantiateComponent.instances]);
  const activateComponents = async (event: MouseEvent<HTMLButtonElement>) => {
    if (selectedComponent.length > 1) {
      setVisible(true);
    } else {
      activate();
    }
  };

  function deactivateComponents(event: MouseEvent<HTMLButtonElement>): void {
    setDeactivateVisible(true);
    const newMacComponents: Component[] = [];
    const macCompo = macComponents.map((mac) => {
      if (mac?.instances?.length > 0) {
        mac.instances.map((m) => {
          newMacComponents.push(m);
        });
      } else {
        newMacComponents.push(mac);
      }
    });
    const toBeDeactivated = [
      ...newMacComponents,
      ...phyComponents,
      ...instantiateComponent.instances,
    ]
      .filter((e) => e.isActive)
      .filter((e) => e.isSelected)
      .map((e) => e.componentId);
    setSelectedComponentDeactivate(toBeDeactivated);
  }
  const activate = async () => {
    showLoader(selectedComponent, true);
    await activateComponent("DATA LINK LAYER", selectedComponent).then(
      (e) => {}
    );
    setVisible(false);
    setSelectedComponent([]);
    executeCallback(updateArrow);
  };

  const deactivate = async () => {
    const newMacComponents: Component[] = [];
    const macCompo = macComponents.map((mac) => {
      if (mac?.instances?.length > 0) {
        mac.instances.map((m) => {
          newMacComponents.push(m);
        });
      } else {
        newMacComponents.push(mac);
      }
    });
    const toBeDeactivated = [
      ...newMacComponents,
      ...phyComponents,
      ...instantiateComponent.instances,
    ]
      .filter((e) => e.isActive)
      .filter((e) => e.isSelected)
      .map((e) => e.componentId);
    showLoader(toBeDeactivated, false);
    componentUtilApi.deactivateComponents(toBeDeactivated).then(() => {});
    setDeactivateVisible(false);
    executeCallback(updateArrow);
  };

  const handleCardClick = (
    e: MouseEvent<HTMLButtonElement> | any,
    componentId: string
  ) => {
    if (e.ctrlKey || e.metaKey) {
      if (selectedComponent.includes(componentId)) {
        setSelectedComponent(
          selectedComponent.filter((c) => c !== componentId)
        );
      } else {
        setSelectedComponent(
          retrieveUnique([...selectedComponent, componentId])
        );
      }
    } else {
      setSelectedComponent([componentId]);
    }
  };

  const handleDragStart = (
    event: React.DragEvent<HTMLDivElement>,
    componentId: string,
    container: "macAvailable" | "phyAvailable"
  ) => {
    event.dataTransfer.effectAllowed = "move";
    const drag_icon = document.createElement("div");
    drag_icon.style.position = "absolute";
    drag_icon.style.top = "-100px";
    drag_icon.style.right = "0px";
    drag_icon.className = "pi pi-clone";
    drag_icon.style.fontSize = "60px";
    document.body.appendChild(drag_icon);
    event.dataTransfer.setDragImage(drag_icon, 40, 30);
    setSourceContainer(container);
    event.dataTransfer.setData(
      "text/plain",
      JSON.stringify(
        selectedComponent.length > 0 ? selectedComponent : [componentId]
      )
    );
  };

  const handleDragOver = (
    event: React.DragEvent<HTMLDivElement>,
    container: "macActive" | "phyActive"
  ) => {
    event.preventDefault();
    setTargetContainer(container);
  };

  const handleDrop = (
    event: React.DragEvent<HTMLDivElement>,
    targetContainer: "macActive" | "phyActive"
  ) => {
    event.preventDefault();
    // console.log(event.target)
    if (targetContainer === "macActive" && sourceContainer === "macAvailable") {
      activateComponents(event as any);
    }
    if (targetContainer === "phyActive" && sourceContainer === "phyAvailable") {
      activateComponents(event as any);
    }
    setSourceContainer(undefined);
    setTargetContainer(undefined);
    // setSelectedComponent([]);

    executeCallback(updateArrow);
  };
  const activateGeneratorComponents = () => {
    showLoader(["tcpipNetConfig"], true);
    activateComponent("DATA LINK LAYER", ["tcpipNetConfig"]);
    executeCallback(updateArrow);
  };

  const networkTitle = () => {
    return (
      <div
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <div className="mac-title">Network Interface </div>
        <Button
          style={{ marginRight: "35px" }}
          rounded
          severity="success"
          icon={"pi pi-plus"}
          onClick={() => activateGeneratorComponents()}
        />
      </div>
    );
  };
  const handleOnDragEnd = ()=>{
    setSourceContainer(undefined)
    setTargetContainer(undefined)
  }
  return (
    <>
    <div className="main-container" onDragEnd={handleOnDragEnd}>
        <div className="app-content-top">
          <Panel
            style={{}}
            headerTemplate={
              <AvailableHeader {...{ selectedComponent, activateComponents }} />
            }
            className="panel-container"
          >
            <div className="card-container-top">
              <Card
                title={
                  <div className="card-container-top-title">MAC Components</div>
                }
                className="mac-card-container "
              >
                <div className="panel-content-wrapper">
                  {macComponents
                    .filter(
                      (e) =>
                        (e.componentType === "UniqueComponent" &&
                          e.isActive === false) ||
                        e.componentType === "GeneratorComponent"
                    )
                    .map((e) => e.componentId)
                    .map((componentId) => (
                      <TCPIPComponentDefault
                        draggable={true}
                        backgroundColor="#5bbcff"
                        handleDragStart={(e: any) =>
                          handleDragStart(e, componentId, "macAvailable")
                        }
                        key={componentId}
                        componentId={componentId}
                        handleCardClick={handleCardClick}
                        selectedComponent={selectedComponent}
                      ></TCPIPComponentDefault>
                    ))}
                </div>
              </Card>
              <Card
                title={
                  <div className="card-container-top-title">PHY Components</div>
                }
                className="phy-card-container"
              >
                <div className="panel-content-wrapper">
                  {phyComponents
                    .filter((e) => !e.isActive)
                    .map((e) => e.componentId)
                    .map((componentId) => (
                      <TCPIPComponentDefault
                        draggable={true}
                        backgroundColor="#5bbcff"
                        handleDragStart={(e: any) =>
                          handleDragStart(e, componentId, "phyAvailable")
                        }
                        key={componentId}
                        componentId={componentId}
                        handleCardClick={handleCardClick}
                        selectedComponent={selectedComponent}
                      ></TCPIPComponentDefault>
                    ))}
                </div>
              </Card>
            </div>
          </Panel>
        </div>
        <div className="app-content-bottom">
          <Panel
            headerTemplate={
              <ActiveHeader
                {...{ selectedComponentDeactivate, deactivateComponents }}
              />
            }
            className="panel-container"
          >
            <AttachmentCardWrapper>
              <div className="card-container">
                <Xwrapper>
                  <Card
                    title={networkTitle}
                    className="card-wraper"
                    style={{
                      backgroundColor: "#CDF5FD",
                    }}
                  >
                    <div className="data-link-active-component-wrapper">
                      {instantiateComponent.instances.length > 0 &&
                        instantiateComponent.instances
                          .sort(compare)
                          .map((a) => (
                            <ActiveDataLinkComponentDefault
                              id={a.componentId}
                              draggable={false}
                              backgroundColor="#5bbcff"
                              handleDragStart={handleDragStart}
                              key={a.componentId}
                              componentId={a.componentId}
                              handleCardClick={handleCardClick}
                              selectedComponent={selectedComponent}
                            ></ActiveDataLinkComponentDefault>
                          ))}
                    </div>
                  </Card>
                  <Card
                    onDragOver={(e) => handleDragOver(e, "macActive")}
                    onDrop={(e) => handleDrop(e, "macActive")}
                    title={<div className="mac-phy-title">MAC Components</div>}
                    className="card-wraper"
                    style={{
                      backgroundColor:
                        sourceContainer === "macAvailable" &&
                        targetContainer === "macActive"
                          ? "lightGreen"
                          : "#CDF5FD",
                    }}
                  >
                    <div className="data-link-active-component-wrapper">
                      {macComponents
                        .filter((e) => e.isActive === true)
                        .map((e) => {
                          if (e?.instances?.length >= 0) {
                            return e?.instances?.map((a) => (
                              <ActiveDataLinkComponentDefault
                                id={a.componentId}
                                draggable={false}
                                backgroundColor="#5bbcff"
                                handleDragStart={handleDragStart}
                                key={a.componentId}
                                componentId={a.componentId}
                                handleCardClick={handleCardClick}
                                selectedComponent={selectedComponent}
                              ></ActiveDataLinkComponentDefault>
                            ));
                          } else {
                            return (
                              <ActiveDataLinkComponentDefault
                                id={e.componentId}
                                draggable={false}
                                backgroundColor="#5bbcff"
                                handleDragStart={handleDragStart}
                                key={e.componentId}
                                componentId={e.componentId}
                                handleCardClick={handleCardClick}
                                selectedComponent={selectedComponent}
                              ></ActiveDataLinkComponentDefault>
                            );
                          }
                        })}
                    </div>
                  </Card>
                  <Card
                    onDragOver={(e) => handleDragOver(e, "phyActive")}
                    onDrop={(e) => handleDrop(e, "phyActive")}
                    title={<div className="mac-phy-title">PHY Components</div>}
                    className="card-wraper"
                    style={{
                      // overflow:'auto',
                      backgroundColor:
                        sourceContainer === "phyAvailable" &&
                        targetContainer === "phyActive"
                          ? "lightGreen"
                          : "#CDF5FD",
                    }}
                  >
                    <div className="data-link-active-component-wrapper">
                      {/* <AttachmentCardWrapper> */}
                      {phyComponents
                        .filter((e) => e.isActive)
                        .map((e) => e.componentId)
                        .map((componentId) => (
                          <ActiveDataLinkComponentDefault
                            id={componentId}
                            draggable={false}
                            backgroundColor="#5bbcff"
                            handleDragStart={handleDragStart}
                            key={componentId}
                            componentId={componentId}
                            handleCardClick={handleCardClick}
                            selectedComponent={selectedComponent}
                          ></ActiveDataLinkComponentDefault>
                        ))}
                      {/* </AttachmentCardWrapper> */}
                    </div>
                  </Card>
                  <DataLinkAttachment
                    components={instantiateComponent.instances}
                  />
                  <DataLinkAttachment components={phyComponents} />
                </Xwrapper>
              </div>
            </AttachmentCardWrapper>
          </Panel>
        </div>
        <Loader visible={loading} text={lodingText} />
        <WarningComponent
          visible={visible}
          activate={activate}
          setVisible={setVisible}
          selectedComponent={selectedComponent}
          action="activate"
        />
        <WarningComponent
          visible={deactivateVisible}
          activate={deactivate}
          setVisible={setDeactivateVisible}
          selectedComponent={selectedComponentDeactivate}
          action="deactivate"
        />
      </div>
    </>
  );
};

export default DataLinkLayer;
