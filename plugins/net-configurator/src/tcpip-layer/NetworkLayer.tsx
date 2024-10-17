import React, {
  LegacyRef,
  MouseEvent,
  RefObject,
  useEffect,
  useState,
} from "react";
import { Panel } from "primereact/panel";
import { Logger } from "@mplab_harmony/harmony-plugin-client-lib";
import { componentUtilApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import TCPIPComponentDefault from "../common-component/TCPIPComponentDefault";
import AvailableHeader from "../common-component/AvailableHeader";
import ActiveHeader from "../common-component/ActiveHeader";
import { retrieveUnique, setUnsetDraggingColor } from "../util/retrieveUnique";
import { activateComponent } from "./activateComponent";
import { initBasicConfigLayer, initNetworkLayer } from "./createGroupComponet";
import WarningComponent from "../common-component/WarningComponent";
import useLoading from "./useLoading";
import Loader from "../common-component/LoaderComponent";
import { compare } from "../util/sorting";
const log = new Logger("NetworkLayer");
type Props = {
  componentIds: string[];
};
const NetworkLayer = ({ componentIds }: Props) => {
  const components = useComponents(componentIds).sort(compare);
  const [sourceContainer, setSourceContainer] = useState<
    "active" | "available"
  >();
  const [selectedComponent, setSelectedComponent] = useState<string[]>([]);
  const panelOneRef = React.useRef<LegacyRef<Panel>>();
  const panelTwoRef = React.useRef<LegacyRef<Panel>>();
  const [visible, setVisible] = useState<boolean>(false);
  const [deactivateVisible, setDeactivateVisible] = useState<boolean>(false);
  const { loading, showLoader,lodingText } = useLoading();
  const [selectedComponentDeactivate, setSelectedComponentDeactivate] =
    useState<string[]>([]);
  useEffect(() => {
    initNetworkLayer();
    initBasicConfigLayer();
  }, []);
  useEffect(()=>{
    const deselectedComponent = components
    .filter((e) => e.isActive)
    .filter((e) => e.isSelected)
    .map((e) => e.componentId);
    setSelectedComponentDeactivate(deselectedComponent)
  },[components])
  const activateComponents = async (event: MouseEvent<HTMLButtonElement>) => {
    if (selectedComponent.length > 1) {
      setVisible(true);
    } else {
      activate();
    }
  };

  function deactivateComponents(event: MouseEvent<HTMLButtonElement>): void {
    setDeactivateVisible(true);
    const toBeDeactivated = components
      .filter((e) => e.isActive)
      .filter((e) => e.isSelected)
      .map((e) => e.componentId);
    setSelectedComponentDeactivate(toBeDeactivated);
  }
  const activate = async () => {
    showLoader(selectedComponent,true);
    await activateComponent("NETWORK LAYER", selectedComponent).then(() => {});
    setVisible(false);
    setSelectedComponent([]);
  };

  const deactivate = async () => {
    const toBeDeactivated = components
      .filter((e) => e.isActive)
      .filter((e) => e.isSelected)
      .map((e) => e.componentId);
      showLoader(toBeDeactivated,false);
    componentUtilApi.deactivateComponents(toBeDeactivated).then(() => {});
    setDeactivateVisible(false);
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
    container: "available" | "active",
    isSelected: boolean,
    componentType: "UniqueComponent" | "InstanceComponent"
  ) => {
    event.dataTransfer.effectAllowed = "move";
    const drag_icon = document.createElement("div");
    drag_icon.style.position='absolute';
    drag_icon.style.top='-100px';
    drag_icon.style.right='0px';
    drag_icon.className = "pi pi-clone";
    drag_icon.style.fontSize = "60px";
    document.body.appendChild(drag_icon);
    event.dataTransfer.setDragImage(drag_icon, 30, 20);
    setSourceContainer(container);
    event.dataTransfer.setData(
      "text/plain",
      JSON.stringify(
        selectedComponent.length > 0 ? selectedComponent : [componentId]
      )
    );
    if (
      container === "active" &&
      (componentType === "UniqueComponent" ||
        componentType === "InstanceComponent")
    ) {
      if (event.ctrlKey) {
        if (isSelected) {
          // componentUtilApi.removeFromSelectedComponents(componentId);
        } else {
          componentUtilApi.addToSelectedComponents(componentId);
        }
      } else {
        componentUtilApi.setSelectedComponent(componentId);
      }
    } else {
      if (event.ctrlKey || event.metaKey) {
        if (selectedComponent.includes(componentId)) {
          // setSelectedComponent(
          //   selectedComponent.filter((c) => c !== componentId)
          // );
        } else {
          setSelectedComponent(
            retrieveUnique([...selectedComponent, componentId])
          );
        }
      } else {
        setSelectedComponent([componentId]);
      }
    }
  };

  const handleDragOver = (
    event: React.DragEvent<HTMLDivElement>,
    container: "available" | "active"
  ) => {
    event.preventDefault();
    if (sourceContainer === "available") {
      setUnsetDraggingColor("panel-container-2_content", "set");
    } else {
      setUnsetDraggingColor("panel-container-1_content", "set");
    }
  };

  const handleDrop = (
    event: React.DragEvent<HTMLDivElement>,
    targetContainer: "available" | "active"
  ) => {
    event.preventDefault();

    if (targetContainer === "available" && sourceContainer === "active") {
      deactivateComponents(event as any);
    }
    if (targetContainer === "active" && sourceContainer === "available") {
      activateComponents(event as any);
    }
    setUnsetDraggingColor("panel-container-1_content", "unset");
    setUnsetDraggingColor("panel-container-2_content", "unset");
    // setSelectedComponent([]);
  };
  const handleOnDragEnd = ()=>{
    setUnsetDraggingColor("panel-container-1_content", "unset");
    setUnsetDraggingColor("panel-container-2_content", "unset");
  }
  return (
    <div className="main-container" onDragEnd={handleOnDragEnd}>
      <div className="app-content-top">
        <Panel
          ref={panelOneRef as RefObject<Panel>}
          id="panel-container-1"
          headerTemplate={
            <AvailableHeader {...{ selectedComponent, activateComponents }} />
          }
          className={`panel-container ${
            selectedComponent.length > 0 ? "highlighted" : ""
          }`}
          onDragOver={(e) => handleDragOver(e, "available")}
        >
          <div
            className={"panel-content-outer-wrapper"}
            onDrop={(e) => handleDrop(e, "available")}
          >
            <div className="panel-content-wrapper">
              {components
                .filter((e) => !e.isActive)
                .map((e) => e.componentId)
                .map((componentId) => (
                  <TCPIPComponentDefault
                    draggable={true}
                    backgroundColor="#374259"
                    handleDragStart={handleDragStart}
                    key={componentId}
                    componentId={componentId}
                    handleCardClick={handleCardClick}
                    selectedComponent={selectedComponent}
                  ></TCPIPComponentDefault>
                ))}
            </div>
          </div>
        </Panel>
      </div>
      <div className="app-content-bottom">
        <Panel
          ref={panelTwoRef as RefObject<Panel>}
          id="panel-container-2"
          headerTemplate={
            <ActiveHeader {...{ selectedComponentDeactivate, deactivateComponents }} />
          }
          className={`panel-container ${
            selectedComponent.length > 0 ? "highlighted" : ""
          }`}
          onDragOver={(e) => handleDragOver(e, "active")}
        >
          <div
            className={"panel-content-outer-wrapper"}
            onDrop={(e) => handleDrop(e, "active")}
          >
            <div className="panel-content-wrapper">
              {components
                .filter((e) => e.isActive)
                .map((e) => e.componentId)
                .map((componentId) => (
                  <TCPIPComponentDefault
                    draggable={true}
                    backgroundColor="#374259"
                    selectedComponent={selectedComponent}
                    handleDragStart={handleDragStart}
                    key={componentId}
                    componentId={componentId}
                    handleCardClick={handleCardClick}
                  ></TCPIPComponentDefault>
                ))}
            </div>
          </div>
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
  );
};

export default NetworkLayer;
