
import {componentApi } from "@mplab_harmony/harmony-plugin-client-lib";
import { componentUtilApi } from "@mplab_harmony/harmony-plugin-client-lib";

export const activateComponent = async (groupComponent:string,componentId:string[]) => {
  //checking tcpIpCore is active or not from data link layer
   const tcpIpIsActive=(await componentApi.getComponent("tcpipStack")).isActive
   if(!tcpIpIsActive){
     const isTCPIPStackGroup = await componentUtilApi.findGroupComponent(
       "TCP/IP STACK"
     );
     if (!isTCPIPStackGroup) {
       await componentUtilApi.setActiveGroup("__ROOTVIEW");
       await componentUtilApi.createGroupComponent("TCP/IP STACK");
     }
     const isBasicConfigGroup = await componentUtilApi.findGroupComponent(
       "BASIC CONFIGURATION"
     );
     if (!isBasicConfigGroup) {
       await componentUtilApi.createGroupComponent(
         "BASIC CONFIGURATION",
         "TCP/IP STACK"
       );
     }
     componentUtilApi.activateComponents(["tcpipStack"], "BASIC CONFIGURATION");
   }
   const isTCPIPStackGroup = await componentUtilApi.findGroupComponent(
     "TCP/IP STACK"
   );
   if (!isTCPIPStackGroup) {
     await componentUtilApi.setActiveGroup("__ROOTVIEW");
     await componentUtilApi.createGroupComponent("TCP/IP STACK");
   }
   const isCurrentGroup = await componentUtilApi.findGroupComponent(
    groupComponent
   );
   if (!isCurrentGroup) {
     await componentUtilApi.createGroupComponent(
        groupComponent,
       "TCP/IP STACK"
     );
   }
   await componentUtilApi.activateComponents(componentId, groupComponent)
  
   
 };