import React from "react";
import "./dataLinkLayer.css";
import Xarrow from "react-xarrows-updated";

type Props = {
  start: string;
  end: string;
  displayType:string
};
const XArrowComponent = ({ start, end,displayType }: Props) => {
  let startPoint=start
  let endPoint=end  
  if(displayType==='TCP/IP Library'){
   startPoint=end
   endPoint=start
  }
  return (
    <Xarrow
      strokeWidth={4}
      showHead={false}
      path="straight"
      start={endPoint}
      end={startPoint}
      key={startPoint}
      dashness={false}
      
    />
  );
};

export default XArrowComponent;
