import { Button } from "primereact/button";
import { HTMLAttributes, useEffect, useRef, useState } from "react";
import {
  ComponentHook,
  componentUtilApi,
} from "@mplab_harmony/harmony-plugin-client-lib";
import "./shape.css";
import { Tooltip } from "primereact/tooltip";
import CustomeTooltip from "./CustomeTooltip";
import OverlayPanelComponent from "./OverlayPanelComponent";
import { ContextMenu } from "primereact/contextmenu";

const TCPIPComponent = (
  props: HTMLAttributes<HTMLButtonElement> & {
    componentHook: ComponentHook;
    handleDragStart: any;
    handleCardClick: any;
    selectedComponent: any;
    backgroundColor: string;
    draggable: boolean;
  }
) => {
  const {
    componentId,
    displayName,
    displayType,
    isActive,
    isSelected,
    componentType,
  } = props.componentHook;
  const {
    componentHook,
    className,
    handleDragStart,
    handleCardClick,
    selectedComponent,
    backgroundColor,
    draggable,
    ...onlyHTMLAttributes
  } = props;
  const cm = useRef<ContextMenu>(null);
  const [requiredDependency, setRequiredDependency] = useState<boolean>(false);
  const [optionalDependency, setOptionalDependency] = useState<boolean>(false);
  const [requiredDepList, setReqDepList] = useState<
    { name: string; displayType: string }[]
  >([]);
  const [optionalDepList, setOptDepList] = useState<
    { name: string; displayType: string }[]
  >([]);
  const [requiredSatList, setReqSatList] = useState<
    { componentId: string; displayType: string; displayName: string }[]
  >([]);
  const [optionalSatList, setOptSatList] = useState<
    { componentId: string; displayType: string; displayName: string }[]
  >([]);
  const required: any = [];
  const optional: any = [];
  const requiredSatisfierList: any = [];
  const optionalSatisfierList: any = [];
  const fetchFunction = async (attachment: any) => {
    const dispType =
      attachment?.satisfiableComponents[0]?.displayType === "PHY Layer" ||
      attachment?.satisfiableComponents[0]?.displayType === "MAC Layer"
        ? "DATA LINK LAYER"
        : attachment?.satisfiableComponents[0]?.displayType;
    if (
      attachment.required === true &&
      attachment.connected === false &&
      attachment.enabled === true
    ) {
      setRequiredDependency(true);
      if (attachment.satisfiableComponents.length > 0) {
        required.push({
          name: attachment.displayType,
          displayType: dispType,
        });
        attachment.satisfiableComponents.map((satisfier: any) => {
          requiredSatisfierList.push(satisfier);
        });
      }
    }

    if (
      attachment.required === false &&
      attachment.connected === false &&
      attachment.enabled
    ) {
      setOptionalDependency(true);
      if (attachment.satisfiableComponents.length > 0) {
        optional.push({
          name: attachment.displayType,
          displayType: dispType,
        });
        attachment.satisfiableComponents.map((satisfier: any) => {
          optionalSatisfierList.push(satisfier);
        });
      }
    }
  };
  const handleClick = (event: any, componentId: any) => {
    if (
      isActive &&
      (componentType === "UniqueComponent" ||
        componentType === "InstanceComponent")
    ) {
      if (event.ctrlKey) {
        if (isSelected) {
          componentUtilApi.removeFromSelectedComponents(componentId);
        } else {
          componentUtilApi.addToSelectedComponents(componentId);
        }
      } else {
        componentUtilApi.setSelectedComponent(componentId);
      }
    } else {
      handleCardClick(event, componentId);
    }
  };
  let componentName = displayName;
  if (!isNaN(parseInt(componentId.split("_")[1]))) {
    componentName = `${displayName}-${componentId.split("_")[1]}`;
  }
  
  
  useEffect(() => {
    if (componentHook.isActive === true) {
      setOptionalDependency(false);
      setRequiredDependency(false);
      componentHook.attachments.map((attachment) => {
        if (attachment.potential === "DEPENDENCY") {
          fetchFunction(attachment);
          setReqDepList(required);
          setOptDepList(optional);
          setReqSatList(requiredSatisfierList);
          setOptSatList(optionalSatisfierList);
        }
      });
    }
  }, [componentHook]);
  

  return (
    <div
      key={componentId}
      className="card-wrapper"
      draggable={draggable}
      onDragStart={(e) =>
        handleDragStart(
          e,
          componentId,
          isActive ? "active" : "available",
          isSelected,
          componentType
        )
      }
    >
      {((requiredDependency &&
        isActive &&
        (componentType === "InstanceComponent" ||
          componentType === "UniqueComponent")) ||
        (optionalDependency &&
          isActive &&
          (componentType === "InstanceComponent" ||
            componentType === "UniqueComponent"))) && (
        <>
          <CustomeTooltip
            componentId={componentId}
            optionalDepList={optionalDepList}
            optionalDependency={optionalDependency}
            requiredDepList={requiredDepList}
            requiredDependency={requiredDependency}
          />
          <i
            className={`pi pi-sort-down-fill custom-tooltip-required-${componentId}`}
            data-pr-position="bottom"
            data-pr-at="bottom+90 right"
            style={{
              color: `${requiredDependency ? "rgb(244, 91, 91)" : "#FA9F0A"}`,
              position: "absolute",
              zIndex: "999",
              marginTop: "-8px",
              marginLeft: "-8px",
              fontSize: "24px",
              transform: "rotate(135deg)",
            }}
          ></i>
        </>
      )}

      <Button
        key={componentId}
        raised
        text
        className={`${isSelected ? "selected-drag-card" : "drag-card"} ${
          selectedComponent?.includes(componentId)
            ? "selected-drag-card"
            : "drag-card"
        }`}
        onClickCapture={(e) => handleClick(e, componentId)}
        label={componentName}
        style={{
          backgroundColor: "#5bbcff",
          color: "white",
          fontWeight: "600",
          fontSize: "16px",
          height: "40px",
          gap: "1rem",
        }}
        onContextMenu={(e) => {
          e.preventDefault();
          if (isActive) {
            if (cm.current) {
              cm.current.show(e);
            }
          }
        }}
      />
      <div>
        {((requiredDependency &&
          isActive &&
          (componentType === "InstanceComponent" ||
            componentType === "UniqueComponent")) ||
          (optionalDependency &&
            isActive &&
            (componentType === "InstanceComponent" ||
              componentType === "UniqueComponent"))) && (
          <OverlayPanelComponent
            cm={cm}
            optionalSatList={optionalSatList}
            requiredSatList={requiredSatList}
          />
        )}
      </div>
    </div>
  );
};
export default TCPIPComponent;
