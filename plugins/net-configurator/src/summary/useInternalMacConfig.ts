import { useComponent } from "@mplab_harmony/harmony-plugin-client-lib";

const useInternalMacConfig = () => {
  const instantiateComponent = useComponent("tcpipNetConfig");
  const internalMac: string[] = instantiateComponent.instances.map(
    (instance) => {
      const componentNumber: any = instance.componentId.split("_");
      let adcModule = `TCPIP_STACK_INT_MAC_IDX${componentNumber[1]}`;
      return adcModule;
    }
  );

  return {
    internalMac,
  };
};

export default useInternalMacConfig;
