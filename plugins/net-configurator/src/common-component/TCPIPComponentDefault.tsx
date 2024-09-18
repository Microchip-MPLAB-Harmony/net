import {
  useComponent,
  attachmentApi,
} from "@mplab_harmony/harmony-plugin-client-lib";
import { HTMLAttributes, useEffect } from "react";
import TCPIPComponent from "./TCPIPComponent";

function TCPIPComponentDefault(
  props: {
    componentId: string;
    handleDragStart: any;
    handleCardClick: any;
    selectedComponent: any;
    backgroundColor: string;
    draggable: boolean;
  } & HTMLAttributes<HTMLButtonElement>
) {
  const componentHook = useComponent(props.componentId);
  const { componentId, ...exceptComponentId } = props;
  

  return (
    <TCPIPComponent componentHook={componentHook} {...exceptComponentId} />
  );
}

export default TCPIPComponentDefault;
