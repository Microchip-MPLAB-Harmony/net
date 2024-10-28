import { Button } from "primereact/button";
import { HTMLAttributes, useContext, useEffect, useRef, useState } from "react";
import {
  ComponentHook,
  componentUtilApi,
  attachmentApi,
  Component,
} from "@mplab_harmony/harmony-plugin-client-lib";
import "./shape.css";
import MyContext from "./Mycontext";
import AttachmentIcon from "./AttachmentIcon";
import { OverlayPanel } from "primereact/overlaypanel";
import CustomeTooltip from "./CustomeTooltip";
import OverlayPanelComponent from "./OverlayPanelComponent";
import { ContextMenu } from "primereact/contextmenu";
const ActiveDataLinkComponent = (
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
    attachments,
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
  const { show, contextType, contextComponentId, contextDisplayType } =
    useContext(MyContext);
  const op: any = useRef(null);
  const ox: any = useRef(null);
  const cm = useRef<ContextMenu>(null);
  const phyAttachment = attachments.filter((e) => e.displayType === "PHY");
  const macAttachment = attachments.filter((e) => e.displayType === "MAC");
  const macMIMAttachment = attachments.filter((e) => e.displayType === "MIIM");
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
      attachment.enabled
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
  const handleClick = (event: any, componentId: any) => {
    if (isActive) {
      if (event.ctrlKey) {
        if (isSelected) {
          componentUtilApi.removeFromSelectedComponents(componentId);
        } else {
          componentUtilApi.addToSelectedComponents(componentId);
        }
      } else {
        if (!isSelected) {
          componentUtilApi.setSelectedComponent(componentId);
        }
      }
    } else {
      handleCardClick(event, componentId);
    }
  };
  let componentName = displayName;
  if (!isNaN(parseInt(componentId.split("_")[1]))) {
    componentName = `${displayName}-${componentId.split("_")[1]}`;
  }

  const attachmentIcon = (
    display_Type: string,
    type: string,
    left: string,
    capability?: string
  ) => {
    const colorConnected = "#37C306";
    const colorReadyToConnect = "#FA9F0A";
    const colorRequiredConnect = "#F66E60";
    const disconnectAttachment = (
      e: React.MouseEvent,
      componentId: string,
      attachmentId: string
    ) => {
      attachmentApi.disconnectAttachment(componentId, attachmentId);
    };
    if (displayType === display_Type && macAttachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (macAttachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (macAttachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (macAttachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (macAttachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (
        macAttachment[0].connected &&
        macAttachment[0].cardinality === "MULTI"
      ) {
        color = colorConnected;
      }
      if (macAttachment[0].attachable && macAttachment[0].required) {
        color = colorRequiredConnect;
      }
      let showIcon = true;
      if (!show) {
        if (
          contextType === macAttachment[0].type &&
          !macAttachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType !== displayType &&
          contextType !== macAttachment[0].type
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&
          macAttachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&
          contextType !== macAttachment[0].type
        ) {
          showIcon = false;
        }
      }

      return (
        showIcon && (
          <>
            <div
              className={`${className}  ${
                macAttachment[0].attachable ? " dot" : ""
              }`}
              style={{ backgroundColor: color, marginLeft: left }}
              data-srccomponentid={componentId}
              data-attachmetid={macAttachment[0].attachmentId}
              data-attachable={macAttachment[0].attachable}
              data-componenttype={macAttachment[0].type}
              data-componentdisplaytype={displayType}
              onContextMenu={(e) => {
                if (macAttachment[0].connected) {
                  op && op.current && op.current.toggle && op.current.toggle(e);
                }
              }}
            ></div>
            <OverlayPanel ref={op} key={macAttachment[0].attachmentId}>
              <button
                onClick={(e) => {
                  disconnectAttachment(
                    e,
                    componentId,
                    macAttachment[0].attachmentId
                  );
                  op.current.toggle(e);
                }}
              >
                {macAttachment[0].connected &&
                macAttachment[0].cardinality === "MULTI"
                  ? "Disconnect All"
                  : "Disconnect"}
              </button>
            </OverlayPanel>
          </>
        )
      );
    }

    if (displayType === display_Type && phyAttachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (phyAttachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (phyAttachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (phyAttachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (phyAttachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (phyAttachment[0].attachable && phyAttachment[0].required) {
        color = colorRequiredConnect;
      }
      let showIcon = true;
      if (!show) {
        if (
          contextType === phyAttachment[0].type &&
          !phyAttachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType !== displayType &&
          contextType !== phyAttachment[0].type
        ) {
          showIcon = false;
        }

        if (
          contextDisplayType === displayType &&
          phyAttachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&
          contextType !== phyAttachment[0].type
        ) {
          showIcon = false;
        }
      }
      return (
        showIcon && (
          <>
            <div
              className={`${className}  ${
                phyAttachment[0].attachable ? " dot" : ""
              }`}
              style={{
                backgroundColor: color,
                marginLeft: left,
                position: "relative",
              }}
              data-srccomponentid={componentId}
              data-attachmetid={phyAttachment[0].attachmentId}
              data-attachable={phyAttachment[0].attachable}
              data-componenttype={phyAttachment[0].type}
              data-componentdisplaytype={displayType}
              onContextMenu={(e) => {
                if (phyAttachment[0].connected) {
                  ox && ox.current && ox.current.toggle && ox.current.toggle(e);
                }
              }}
            ></div>
            <OverlayPanel ref={ox} key={phyAttachment[0].attachmentId}>
              <button
                onClick={(e) => {
                  disconnectAttachment(
                    e,
                    componentId,
                    phyAttachment[0].attachmentId
                  );
                  ox.current.toggle(e);
                }}
              >
                {phyAttachment[0].connected &&
                phyAttachment[0].cardinality === "MULTI"
                  ? "Disconnect All"
                  : "Disconnect"}
              </button>
            </OverlayPanel>
          </>
        )
      );
    }

    if (displayType === display_Type && macMIMAttachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (macMIMAttachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (macMIMAttachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (macMIMAttachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (macMIMAttachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (macMIMAttachment[0].connected) {
        color = colorConnected;
      } else {
        color = colorRequiredConnect;
      }
      if (macMIMAttachment[0].attachable && macMIMAttachment[0].required) {
        color = colorRequiredConnect;
      }
      if (capability === macMIMAttachment[0].potential)
        return (
          show && (
            <i
              className={className}
              style={{ backgroundColor: color, marginLeft: left }}
              data-srccomponentid={componentId}
              data-attachmetid={macMIMAttachment[0].attachmentId}
              data-attachable={macMIMAttachment[0].attachable}
              data-componenttype={macMIMAttachment[0].type}
            ></i>
          )
        );
    }
  };
  return (
    <div
      // id={componentId}
      key={componentId}
      className={"card-wrapper " + componentId}
      draggable={draggable}
      onDragStart={(e) =>
        handleDragStart(e, componentId, isActive ? "active" : "available")
      }
    >
      <div
        key={componentId}
        style={{ display: "flex", position: "relative", minWidth: "300px" }}
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
                color: `${
                  requiredDependency ? "rgb(244, 91, 91)" : "#FA9F0A"
                }`,
                position: "absolute",
                zIndex: "999",
                marginTop: "-7px",
                marginLeft: "7px",
                fontSize: "20px",
                transform: "rotate(135deg)",
              }}
            ></i>
          </>
        )}
        <div style={{ width: "15px" }}>
          {displayType === "MAC Layer" &&
            attachmentIcon("MAC Layer", "MAC", "7px")}
          {displayType === "PHY Layer" &&
            attachmentIcon("PHY Layer", "PHY", "7px")}
          {displayType === "PHY Layer" &&
            attachmentIcon("PHY Layer", "MIIM", "7px", "CAPABILITY")}
        </div>
        <Button
          id={componentId}
          key={componentId}
          raised
          text
          className={`${isSelected ? "selected-drag-card" : "drag-card"} ${
            selectedComponent?.includes(componentId)
              ? "selected-drag-card"
              : "drag-card"
          }`}
          onClick={(e) => handleClick(e, componentId)}
          label={componentName}
          style={{
            width: "200px",
            minHeight: "40px",
            backgroundColor: backgroundColor,
            color: "white",
            fontWeight: "600",
            fontSize: "16px",
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
        <div style={{ width: "15px", position: "relative" }}>
          {displayType === "MAC Layer" &&
            attachmentIcon("MAC Layer", "PHY", "-7px")}
          {displayType === "TCP/IP Library" &&
            attachmentIcon("TCP/IP Library", "MAC", "-7px")}
          {displayType === "PHY Layer" &&
            attachmentIcon("PHY Layer", "MIIM", "-7px", "DEPENDENCY")}
        </div>
      </div>
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
export default ActiveDataLinkComponent;
