import { useComponent,ComponentHook } from "@mplab_harmony/harmony-plugin-client-lib";
import { HTMLAttributes } from "react";
import TCPIPComponent from "./ActiveDataLinkComponent";

const ActiveDataLinkComponentDefault = (
  props: {
    componentId: string;
    handleDragStart: any;
    handleCardClick:any
    selectedComponent:any
    backgroundColor:string
    draggable:boolean
  } & HTMLAttributes<HTMLButtonElement>
)=> {
  const componentHook = useComponent(props.componentId);
  const { componentId, ...exceptComponentId } = props;

  return (componentHook &&(<TCPIPComponent componentHook={componentHook} {...exceptComponentId} />));
}

export default ActiveDataLinkComponentDefault
