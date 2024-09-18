import { useCallback, useState } from "react";
import { TabView, TabPanel } from "primereact/tabview";
import { ScrollPanel } from "primereact/scrollpanel";
import "./summary.css";
import { Card } from "primereact/card";
import TCPIPstackConfigStatus from "./TCPIPstackConfigStatus";
import NetworkInterface from "./NetworkInterface";
import SystemConfig from "./SystemConfig";
type Props = {
  componentIds: string[];
};
const ConfigSummary = ({ componentIds }: Props) => {
  return (
    <div className="main-container">
      <div className="summary-container">
        <Card style={{ textAlign: "start", fontWeight: 600, fontSize: "24px" }}>
          <div>Configuration Summary</div>
        </Card>
        <TabView className="tabView-container">
          <TabPanel header="TCP/IP Modules">
            <ScrollPanel className="scrollPanel-container">
            <TCPIPstackConfigStatus componentIds={componentIds} />
            </ScrollPanel>
          </TabPanel>
          <TabPanel header="Interface">
            <ScrollPanel className="summary-container">
              <NetworkInterface />
            </ScrollPanel>
          </TabPanel>
          <TabPanel header="System">
            <ScrollPanel className="summary-container">
              <SystemConfig/>
            </ScrollPanel>
          </TabPanel>
        </TabView>
      </div>
    </div>
  );
};

export default ConfigSummary;
