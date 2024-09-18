import React from "react";
import { Button } from "primereact/button";
const ActiveHeader = ({
  deactivateComponents,
  selectedComponentDeactivate,
}: any) => {
  return (
    <div className="p-panel-header">
      <span className="p-panel-title">Active Components</span>
      <Button
        severity="danger"
        rounded
        style={{fontSize:'18px'}}
        icon={"pi pi-trash"}
        onClick={deactivateComponents}
        disabled={selectedComponentDeactivate?.length === 0}
        tooltipOptions={{ showOnDisabled: true,position:'bottom' }}
        tooltip={
          selectedComponentDeactivate?.length === 0
            ? "Currently, no active components have been selected"
            : ""
        }
      />
    </div>
  );
};

export default ActiveHeader;
