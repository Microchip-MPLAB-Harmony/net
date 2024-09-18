import React, { useState } from "react";
import { Button } from "primereact/button";
import { Dialog } from "primereact/dialog";
import { useComponents } from "@mplab_harmony/harmony-plugin-client-lib";
type Props = {
  visible: boolean;
  activate: () => void;
  setVisible: (T: boolean) => void;
  selectedComponent: string[];
  action?: string;
};
export default function WarningComponent({
  visible,
  activate,
  setVisible,
  selectedComponent,
  action,
}: Props) {
  let deactivateString =
    " Selected components are going to be deactivated. Are you sure?";
  let activateString =
    " Selected components are going to be activated. Are you sure?";
  const components = useComponents(selectedComponent);
  const footerContent = (
    <div style={{ display: "flex", flexDirection: "row" }}>
      <div style={{ width: "90%", textAlign: "left", fontWeight: "bold" }}>
        {action === "deactivate" ? deactivateString : activateString}
      </div>
      <Button
        label="No"
        onClick={() => setVisible(false)}
        className="p-button-text"
      />
      <Button label="Yes" onClick={() => activate()} autoFocus />
    </div>
  );

  const headerContent = (
    <div
      style={{
        display: "flex",
        flexDirection: "row",
        gap: "1rem",
        alignItems: "",
      }}
    >
      <div>Warning</div>
      <div style={{ fontSize: "20px" }}>
        <i
          className="pi pi-exclamation-circle"
          style={{ fontSize: "20px" }}
        ></i>
      </div>
    </div>
  );

  return (
    <div className="card flex justify-content-center">
      <Dialog
        header={headerContent}
        visible={visible}
        style={{ width: "50vw" }}
        onHide={() => {
          if (!visible) return;
          setVisible(false);
        }}
        footer={footerContent}
      >
        <div
          style={{ display: "flex", flexDirection: "row", flexWrap: "wrap" }}
        >
          {components.map((component) => (
            <div
              style={{
                border: "1px solid #ddd",
                borderRadius: "5px",
                boxShadow: "2px 2px 5px rgba(0, 0, 0, 0.2)",
                padding: "15px",
                margin: "5px",
              }}
              key={component.componentId}
            >
              {`${component.displayName} (${component.componentId})`}
            </div>
          ))}
        </div>
      </Dialog>
    </div>
  );
}
