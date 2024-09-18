import React, {
  HTMLAttributes,
  useEffect,
  useRef,
  useState,
  createContext,
  useContext,
} from "react";
import { attachmentApi } from "@mplab_harmony/harmony-plugin-client-lib";
import "./shape.css";
import MyContext from "./Mycontext";
interface Dot {
  x: number;
  y: number;
}
type Props = {
  display_Type: string | undefined;
  displayType: string;
  type: string;
  left: string | undefined;
  capability?: string | undefined;
  attachment: any;
  componentId: string;
};
const AttachmentIcon = ({
  display_Type,
  type,
  left,
  capability,
  attachment,
  displayType,
  componentId,
}: Props) => {
  const { show, contextType, contextComponentId, contextDisplayType } =
    useContext(MyContext);
  const colorConnected = "#37C306";
  const colorReadyToConnect = "#FA9F0A";
  const colorRequiredConnect = "#F66E60";
    if (displayType === display_Type && attachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (attachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (attachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (attachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (attachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (
        attachment[0].connected &&
        attachment[0].cardinality === "MULTI"
      ) {
        color = colorConnected;
      }
      if (attachment[0].attachable && attachment[0].required) {
        color = colorRequiredConnect;
      }
      let showIcon = true;
      if (!show) {
        if (
          contextType === attachment[0].type &&
          !attachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType !== displayType &&
          contextType !== attachment[0].type
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&
          attachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&contextType !== attachment[0].type
        ) {
          showIcon = false;
        }
      }

      return (
        showIcon && (
          <div
            className={`${className}  ${
                attachment[0].attachable ? " dot" : ""
            }`}
            style={{ backgroundColor: color, left: left }}
            data-srccomponentid={componentId}
            data-attachmetid={attachment[0].attachmentId}
            data-attachable={attachment[0].attachable}
            data-componenttype={attachment[0].type}
            data-componentdisplaytype={displayType}
          ></div>
        )
      );
    }

    if (displayType === display_Type && attachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (attachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (attachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (attachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (attachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (attachment[0].attachable && attachment[0].required) {
        color = colorRequiredConnect;
      }
      let showIcon = true;
      if (!show) {
        if (
          contextType === attachment[0].type &&
          !attachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType !== displayType &&
          contextType !== attachment[0].type
        ) {
          showIcon = false;
        }

        if (
          contextDisplayType === displayType &&
          attachment[0].attachable &&
          contextComponentId !== componentId
        ) {
          showIcon = false;
        }
        if (
          contextDisplayType === displayType &&contextType !== attachment[0].type
        ) {
          showIcon = false;
        }
      }
      return (
        showIcon && (
          <i
            className={`${className}  ${
                attachment[0].attachable ? " dot" : ""
            }`}
            style={{ backgroundColor: color, left: left }}
            data-srccomponentid={componentId}
            data-attachmetid={attachment[0].attachmentId}
            data-attachable={attachment[0].attachable}
            data-componenttype={attachment[0].type}
            data-componentdisplaytype={displayType}
          ></i>
        )
      );
    }
    
    if (displayType === display_Type && attachment[0]?.type === type) {
      let color = colorReadyToConnect;
      let className = "square1";
      if (attachment[0].cardinality === "DIRECT") {
        className = "triangle";
      }
      if (attachment[0].cardinality === "MULTI") {
        className = "square1";
      }
      if (attachment[0].cardinality === "GENERIC") {
        className = "circle";
      }
      if (attachment[0].attachable) {
        color = colorReadyToConnect;
      } else {
        color = colorConnected;
      }
      if (attachment[0].connected) {
        color = colorConnected;
      }
      if (attachment[0].attachable && attachment[0].required) {
        color = colorRequiredConnect;
      }
      if (capability === attachment[0].potential)
        return (
          show&&<i
            className={className}
            style={{ backgroundColor: color, left: left }}
            data-srccomponentid={componentId}
            data-attachmetid={attachment[0].attachmentId}
            data-attachable={attachment[0].attachable}
            data-componenttype={attachment[0].type}
          ></i>
        );
    }
};
export default AttachmentIcon;
