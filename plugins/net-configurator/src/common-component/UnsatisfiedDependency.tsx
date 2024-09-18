import React, { useEffect, useState } from "react";
import { Attachment } from "@mplab_harmony/harmony-plugin-client-lib";
import "./unsatisfiedDependency.css";
type Props = {
  displayName: string;
  attachments: Attachment[];
  dependency: "required" | "optional";
};
export default function UnsatisfiedDependnency({
  displayName,
  attachments,
  dependency,
}: Props) {
  const [requiredDepList, setReqDepList] = useState<
    { name: string; displayType: string }[]
  >([]);
  const [optionalDepList, setOptDepList] = useState<
    { name: string; displayType: string }[]
  >([]);
  const required: any = [];
  const optional: any = [];
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
      if (attachment.satisfiableComponents.length > 0) {
        required.push({
          name: attachment.displayType,
          displayType: dispType,
        });
      }
    }
    if (
      attachment.required === false &&
      attachment.connected === false &&
      attachment.enabled
    ) {
      if (attachment.satisfiableComponents.length > 0) {
        optional.push({
          name: attachment.displayType,
          displayType: dispType,
        });
      }
    }
  };
  useEffect(() => {
    attachments.map((attachment) => {
      if (attachment.potential === "DEPENDENCY") {
        fetchFunction(attachment);
        setReqDepList(required);
        setOptDepList(optional);
      }
    });
  }, [attachments]);
  return dependency === "required" && requiredDepList.length > 0 ? (
    <div style={{ marginLeft: "30px" }}>
      <div
        className="unsatisfied"
        style={{
          color: "red",
        }}
      >
        <div style={{ width: "200px" ,textAlign:'left'}}>{displayName}</div>
        <div>:</div>
        <div>
          {requiredDepList.map(
            (item, index) =>
              `${item.name} (${item.displayType})${
                index < requiredDepList.length - 1 ? ", " : ""
              }`
          )}
        </div>
      </div>
    </div>
  ) : dependency === "optional" && optionalDepList.length > 0 ? (
    <div style={{ marginLeft: "30px" }}>
      <div
        className="unsatisfied"
        style={{
          color: "#FA9F0A",
        }}
      >
        <div style={{ width: "200px" ,textAlign:'left'}}>{displayName}</div>
        <div>:</div>
        <div>
          {optionalDepList.map(
            (item, index) =>
              `${item.name} (${item.displayType})${
                index < optionalDepList.length - 1 ? ", " : ""
              }`
          )}
        </div>
      </div>
    </div>
  ) : null;
}
