import { componentUtilApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { useState } from "react";
const useLoading = () => {
  const [loading, setLoading] = useState<boolean>(false);
  const [lodingText,setLoadingText]=useState<string>('')
  const showLoader = (componentId: string[], active: boolean) => {
    setLoading(true);
    setLoadingText(active?'Activating...':'Deactivating...')
    checkIsActive(componentId, active);
  };
  const checkIsActive = (componentId: string[], active: boolean) => {
    const startAPICall = async (componentId: string[], active: boolean) => {
      const component = await componentUtilApi.getComponents(componentId);
      if (component[0]?.componentType === "GeneratorComponent") {
        setTimeout(() => {
            setLoading(false)
          }, 2000);
          return
      }
      const filterdComp = component
        .filter((c) => !c.isActive === active)
        .map((c) => c.componentId);
      if (filterdComp.length === 0) {
        setLoading(false);
        return;
      }
      setTimeout(() => {
        startAPICall(filterdComp, active);
      }, 2000);
    };
    startAPICall(componentId, active);
  };

  return { loading, showLoader,lodingText };
};

// export interface ComponentHook extends Component {}

export default useLoading;
