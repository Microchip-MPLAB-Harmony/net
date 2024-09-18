import React from "react";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
import { Divider } from "primereact/divider";
import { Badge } from "primereact/badge";
import "./summary.css";
import { compare } from "../util/sorting";
import UnsatisfiedDependnency from "../common-component/UnsatisfiedDependency";
type Props = {
  componentIds: string[];
};
const DependencyStatus = ({ componentIds }: Props) => {
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
        <Badge value="Unsatisfied Dependency Status" size="large"></Badge>
      </Divider>
      <div style={{ textAlign: "left" }}>
        <h2>Required Dependency Status</h2>
      </div>
      <div>
        {applicationComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="required"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {presentationComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="required"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {transportComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="required"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {networkComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="required"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {dataLinkComponents.map((c) => {
          if (c?.instances?.length >= 0) {
            return c?.instances?.map((a) => {
              let componentName = a.displayName;
              if (!isNaN(parseInt(a.componentId.split("_")[1]))) {
                componentName = `${a.displayName}-${
                  a.componentId.split("_")[1]
                }`;
              }
              return (
                <UnsatisfiedDependnency
                  dependency="required"
                  attachments={a.attachments}
                  displayName={componentName}
                />
              );
            });
          } else {
            return (
              <UnsatisfiedDependnency
                dependency="required"
                attachments={c.attachments}
                displayName={c.displayName}
              />
            );
          }
        })}
      </div>
      <div>
        {basicConfigComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="required"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>

      <div style={{ textAlign: "left" }}>
        <h2>Optional Dependency Status</h2>
      </div>
      <div>
        {applicationComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="optional"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {presentationComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="optional"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {transportComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="optional"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {networkComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="optional"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
      <div>
        {dataLinkComponents.map((c) => {
          if (c?.instances?.length >= 0) {
            return c?.instances?.map((a) => {
              let componentName = a.displayName;
              if (!isNaN(parseInt(a.componentId.split("_")[1]))) {
                componentName = `${a.displayName}-${
                  a.componentId.split("_")[1]
                }`;
              }
              return (
                <UnsatisfiedDependnency
                  dependency="optional"
                  attachments={a.attachments}
                  displayName={componentName}
                />
              );
            });
          } else {
            return (
              <UnsatisfiedDependnency
                dependency="optional"
                attachments={c.attachments}
                displayName={c.displayName}
              />
            );
          }
        })}
      </div>
      <div>
        {basicConfigComponents.map((c) => {
          return (
            <UnsatisfiedDependnency
              dependency="optional"
              attachments={c.attachments}
              displayName={c.displayName}
            />
          );
        })}
      </div>
    </>
  );
};

export default DependencyStatus;
