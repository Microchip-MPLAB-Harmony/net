import React, { useState } from "react";
import { Tooltip } from "primereact/tooltip";

type Props = {
  requiredDependency: boolean;
  requiredDepList: { name: string; displayType: string }[];
  optionalDependency: boolean;
  optionalDepList: { name: string; displayType: string }[];
  componentId: string;
};
export default function CustomeTooltip({
  requiredDependency,
  requiredDepList,
  optionalDependency,
  optionalDepList,
  componentId,
}: Props) {
  return (
    <Tooltip target={`.custom-tooltip-required-${componentId}`}>
      {requiredDependency && <div>Required Dependency</div>}
      {requiredDepList.length > 0 ? (
        <>
          <ul>
            {requiredDepList.map((dependency: any) => {
              return (
                <li
                  key={dependency.name}
                >{`${dependency.name} (${dependency.displayType})`}</li>
              );
            })}
          </ul>
        </>
      ) : (
        requiredDependency && <div> No satisfiers available!</div>
      )}
      {optionalDependency && <div>Optional Dependency</div>}
      {optionalDepList.length > 0 ? (
        <>
          <ul>
            {optionalDepList.map((dependency: any) => {
              return (
                <li
                  key={dependency.name}
                >{`${dependency.name} (${dependency.displayType})`}</li>
              );
            })}
          </ul>
        </>
      ) : (
        optionalDependency && <div>No satisfiers available!</div>
      )}
    </Tooltip>
  );
}
