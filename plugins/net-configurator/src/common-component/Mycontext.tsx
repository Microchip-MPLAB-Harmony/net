import { createContext } from "react";
type Context = {
  contextType: string | undefined;
  show: boolean;
  contextComponentId: string | undefined;
  contextDisplayType:string|undefined
};
const MyContext = createContext<Context>({
  contextType: "",
  show: true,
  contextComponentId: "",
  contextDisplayType: "",
});

export default MyContext;
