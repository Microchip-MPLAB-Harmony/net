import React from "react";
import "./overview.css";
import TCPIPComponentDefault from "../common-component/TCPIPComponentDefault";
const OverViewLayer = (props: any) => {
  const { label, activatedComponents, labelColor } = props;
  return (
    <div className="layer-container">
      <div className="layer-header p-button-label" data-pc-section="label">
        <div>{label}</div>
      </div>
      <div className="layer-list">
        {activatedComponents
          .filter((e: any) => e.isActive === true)
          .map((e: any) => {
            if (e?.instances?.length >= 0) {
              return e?.instances?.map((a: any) => (
                <div key={a.componentId} style={{ margin: "5px", position:'relative' }}>
                  <TCPIPComponentDefault
                    draggable={false}
                    backgroundColor={labelColor}
                    handleDragStart={""}
                    key={a.componentId}
                    componentId={a.componentId}
                    handleCardClick={""}
                    selectedComponent={""}
                  ></TCPIPComponentDefault>
                </div>
              ));
            } else {
              return (
                <div key={e.componentId} style={{ margin: "5px",  position:'relative' }}>
                  <TCPIPComponentDefault
                    draggable={false}
                    backgroundColor={labelColor}
                    handleDragStart={""}
                    key={e.componentId}
                    componentId={e.componentId}
                    handleCardClick={""}
                    selectedComponent={""}
                  ></TCPIPComponentDefault>
                </div>
              );
            }
          })}
      </div>
    </div>
  );
};

export default OverViewLayer;
