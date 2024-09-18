import React from "react";
import { Button } from "primereact/button";
const AvailableHeader = ({ activateComponents, selectedComponent }: any) => {
  return (
    <div className="p-panel-header">
      <span className="p-panel-title">Available Components</span>
        <Button
          severity="success"
          rounded
          icon={"pi pi-plus"}
          style={{fontSize:'18px'}}
          onClick={activateComponents}
          disabled={selectedComponent?.length === 0}
          tooltipOptions={{ showOnDisabled: true ,position:'bottom'}}
          tooltip={selectedComponent?.length===0?'Currently, no available components have been selected':''}
        />
    </div>
  );
};

export default AvailableHeader;
