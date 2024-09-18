import { componentApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { componentUtilApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { groupComponentConstant } from "../constants/tcpIpStack";
export const initGroupComponent = async () => {
  const isTCPIPStackGroup = await componentUtilApi.findGroupComponent(
    groupComponentConstant.TCP_IP_STACK
  );
  if (!isTCPIPStackGroup) {
    await componentUtilApi.setActiveGroup(groupComponentConstant.ROOT_VIEW);
    await componentUtilApi.createGroupComponent(
      groupComponentConstant.TCP_IP_STACK
    );
  }
};

export const initApplicationLayer = async()=>{

    const applicationGroup = await componentUtilApi.findGroupComponent(
        groupComponentConstant.APPLICATION_LAYER
      );
      if (!applicationGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.APPLICATION_LAYER,
          groupComponentConstant.TCP_IP_STACK
        );
      }

}
export const initTransportLayer = async()=>{
    const transportGroup = await componentUtilApi.findGroupComponent(
        groupComponentConstant.TRANSPORT_LAYER
      );
      if (!transportGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.TRANSPORT_LAYER,
          groupComponentConstant.TCP_IP_STACK
        );
      }
}
export const initNetworkLayer = async()=>{
    const networkGroup = await componentUtilApi.findGroupComponent(
        groupComponentConstant.NETWORK_LAYER
      );
      if (!networkGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.NETWORK_LAYER,
          groupComponentConstant.TCP_IP_STACK
        );
      }
}
export const initDataLinkLayer =async()=>{
    const dataLinkGroup = await componentUtilApi.findGroupComponent(
        groupComponentConstant.DATA_LINK_LAYER
      );
      if (!dataLinkGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.DATA_LINK_LAYER,
          groupComponentConstant.TCP_IP_STACK
        );
      }
}
export const initPresentationLayer =async()=>{
    const presentationGroup =await componentUtilApi.findGroupComponent(
        groupComponentConstant.PRESENTATION_LAYER
      );
      if (!presentationGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.PRESENTATION_LAYER,
          groupComponentConstant.TCP_IP_STACK
        );
      }
}
export const initBasicConfigLayer =async()=>{
    const basicConfigGroup = await componentUtilApi.findGroupComponent(
        groupComponentConstant.BASIC_CONFIGURATION
      );
      if (!basicConfigGroup) {
        await componentUtilApi.createGroupComponent(
          groupComponentConstant.BASIC_CONFIGURATION,
          groupComponentConstant.TCP_IP_STACK
        );
      }
}
