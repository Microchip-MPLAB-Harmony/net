import React, {
  HTMLAttributes,
  useEffect,
  useRef,
  useState,
  createContext,
} from "react";
import { attachmentApi } from "@mplab_harmony/harmony-plugin-client-lib";
import "./shape.css";
import MyContext from "./Mycontext";
interface Dot {
  x: number;
  y: number;
}

type Props = HTMLAttributes<HTMLButtonElement>;
const AttachmentCardWrapper: React.FC<Props> = (props) => {
  const { className, draggable, children, ...onlyHTMLAttributes } = props;
  const [showConnection, setShowConnection] = useState<boolean>(true);
  const [type, setType] = useState<string | undefined>();
  const [displayType, setDisplayType] = useState<string | undefined>();

  const [startDot, setStartDot] = useState<HTMLElement | null>(null);
  const [lines, setLines] = useState<{ start: Dot; end: Dot }[]>([]);
  const svgRef = useRef<SVGSVGElement>(null);
  const [sourceComponentId, setSourceComponentId] = useState<
    string | undefined
  >();
  const showConnections = {
    show: showConnection,
    contextType: type,
    contextComponentId: sourceComponentId,
    contextDisplayType: displayType,
  };
  const [sourceAttachmentId, setSourceAttachmentId] = useState<
    string | undefined
  >();
  const [targetComponentId, setTargetComponentId] = useState<
    string | undefined
  >();
  const [targetAttachmentId, setTargetAttachmentId] = useState<
    string | undefined
  >();

  const handleMouseDown = (event: React.MouseEvent) => {
    if(event.button===0){
      if (
        event.target instanceof HTMLElement &&
        event.target.classList.contains("dot")
      ) {
        setSourceComponentId(event.target.dataset.srccomponentid);
        setSourceAttachmentId(event.target.dataset.attachmetid);
        setStartDot(event.target);
        setType(event.target.dataset.componenttype);
        setDisplayType(event.target.dataset.componentdisplaytype);
        setShowConnection(false);
      }
    }
   
  };

  const handleMouseUp = async (event: React.MouseEvent) => {
    setShowConnection(true);
    if (
      startDot &&
      event.target instanceof HTMLElement &&
      event.target.classList.contains("dot")
    ) {
      const endDot = event.target;
      const startCoords = getDotCoordinates(startDot);
      const endCoords = getDotCoordinates(endDot);
      setTargetComponentId(event.target.dataset.srccomponentid);
      setTargetAttachmentId(event.target.dataset.attachmetid);
      if (shouldConnectDots(startDot, endDot)) {
        // setLines([...lines, { start: startCoords, end: endCoords }]);
        await performApiCall();
      }
    }
    setStartDot(null);
  };

  useEffect(() => {
    if (
      sourceComponentId &&
      sourceAttachmentId &&
      targetComponentId &&
      targetAttachmentId
    ) {
      attachmentApi.createConnection(
        sourceComponentId,
        sourceAttachmentId,
        targetComponentId,
        targetAttachmentId
      );
      clearTemporaryLine();
      setSourceComponentId(undefined);
      setSourceAttachmentId(undefined);
      setTargetAttachmentId(undefined);
      setTargetComponentId(undefined);
    }
  }, [
    sourceAttachmentId,
    sourceComponentId,
    targetAttachmentId,
    targetComponentId,
  ]);

  const handleMouseMove = (event: React.MouseEvent) => {
    if (startDot && svgRef.current) {
      const svg = svgRef.current;
      const svgPoint = svg.createSVGPoint();
      svgPoint.x = event.clientX;
      svgPoint.y = event.clientY;
      const cursorPoint = svgPoint.matrixTransform(
        svg.getScreenCTM()?.inverse()
      );
      const startCoords = getDotCoordinates(startDot);
      drawTemporaryLine(startCoords, { x: cursorPoint.x, y: cursorPoint.y });
    }
  };

  const getDotCoordinates = (dot: HTMLElement): Dot => {
    const rect = dot.getBoundingClientRect();
    return {
      x: rect.left + rect.width / 2,
      y: rect.top + rect.height / 2,
    };
  };

  const shouldConnectDots = (
    startDot: HTMLElement,
    endDot: HTMLElement
  ): boolean => {
    // Add your custom logic here
    // if(some-condition)
    // return true or else false to allow connecting
    return true;
  };

  const performApiCall = async () => {
    // Perform your API call here
  };

  const drawTemporaryLine = (start: Dot, end: Dot) => {
    if (svgRef.current) {
      const tempLine = document.getElementById("temp-line");
      if (tempLine) {
        tempLine.setAttribute("x1", start.x.toString());
        tempLine.setAttribute("y1", start.y.toString());
        tempLine.setAttribute("x2", end.x.toString());
        tempLine.setAttribute("y2", end.y.toString());
      } else {
        const line = document.createElementNS(
          "http://www.w3.org/2000/svg",
          "line"
        );
        line.setAttribute("id", "temp-line");
        line.setAttribute("stroke", "black");
        line.setAttribute("stroke-width", "2");
        line.setAttribute("x1", start.x.toString());
        line.setAttribute("y1", start.y.toString());
        line.setAttribute("x2", end.x.toString());
        line.setAttribute("y2", end.y.toString());
        svgRef.current.appendChild(line);
      }
    }
  };

  const clearTemporaryLine = () => {
    const tempLine = document.getElementById("temp-line");
    if (tempLine) {
      tempLine.remove();
    }
  };

  useEffect(() => {
    clearTemporaryLine();
  }, [startDot]);

  return (
    <MyContext.Provider value={showConnections}>
      <div
        onMouseDown={(e) => handleMouseDown(e)}
        onMouseUp={handleMouseUp}
        onMouseMove={handleMouseMove}
      >
        {props.children}
        <svg ref={svgRef} id="lineCanvas" className="line-canvas">
          {lines.map((line, index) => (
            <line
              key={index}
              x1={line.start.x}
              y1={line.start.y}
              x2={line.end.x}
              y2={line.end.y}
              stroke="black"
              strokeWidth="2"
            />
          ))}
        </svg>
      </div>
    </MyContext.Provider>
  );
};
export default AttachmentCardWrapper;
