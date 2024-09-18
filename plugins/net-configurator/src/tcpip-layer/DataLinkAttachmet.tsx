import React from "react";
import {
  Component,
} from "@mplab_harmony/harmony-plugin-client-lib";
import "./dataLinkLayer.css";
import XArrowComponent from "./XArrowComponent";
type Props = {
  components: Component[];
};
const DataLinkAttachment = ({ components }: Props) => {
  return (
    <>
      { components
        .filter((e) => e.isActive)
        .map((a) => {
          return a.attachments.map((t) => {
            return t.targets.map((element) => (
              <div className="attachmet" key={a.componentId}>
                <XArrowComponent
                  start={a.componentId}
                  end={element.componentId}
                  key={a.componentId}
                  displayType={a.displayType}
                />
              </div>
            ));
          });
        })}
    </>
  );
};

export default DataLinkAttachment;
