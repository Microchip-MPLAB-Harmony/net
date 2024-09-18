import { useCallback, useState } from 'react';
import { Fieldset } from 'primereact/fieldset';
import { ToggleButton } from 'primereact/togglebutton';
import {sidebarMenuList} from './sidebar-demo'
import './sidebarMenu.css'

 const SidebarMenu = (props:any) => {

    const [sidebarMenuItem, setSidebarMenuItem] = useState(sidebarMenuList);

    const setMenuItemConfig = useCallback((item: any) => {
        setSidebarMenuItem(prevState => {
            return prevState.map((val) => {
                if (val.name.indexOf(item.name) > -1) {
                    val.isSelected = true
                    props.onHandleMenuClick(val)
                    return val
                }
                else {
                    val.isSelected = false
                    return val
                }

            })

        })
    },[])

    return (
        <div className="side-menu-container">

            <div className="side-menu-sub-container">
                <ToggleButton
                    key={sidebarMenuList[0].name}
                    style={{ fontSize: '14px', padding: '4px auto' }}
                    onLabel={sidebarMenuList[0].name}
                    offLabel={sidebarMenuList[0].name}
                    onIcon={sidebarMenuList[0].iconName}
                    offIcon={sidebarMenuList[0].iconName}
                    checked={sidebarMenuList[0].isSelected}
                    onChange={() => setMenuItemConfig(sidebarMenuList[0])}
                    className={`sibe-menu-button ${sidebarMenuList[0].isSelected ? sidebarMenuList[0].className : ''}`}

                />
                <div>
                    <Fieldset legend="TCP/IP Layers">
                        <div className='side-menu-tcpip'>
                            {sidebarMenuItem.map((menuItem, index) => {
                                if (index > 0 && index < sidebarMenuItem.length - 1)
                                    return (<ToggleButton style={{ fontSize: '12px', padding: '4px auto' }}
                                        key={menuItem.name}
                                        onLabel={menuItem.name}
                                        offLabel={menuItem.name}
                                        onIcon={menuItem.iconName}
                                        offIcon={menuItem.iconName}
                                        checked={menuItem.isSelected}
                                        onChange={() => setMenuItemConfig(menuItem)}
                                        className={`sibe-menu-button ${menuItem.isSelected ? menuItem.className : ''}`}

                                    />)
                            })}
                        </div>

                    </Fieldset>

                </div>
            </div>
            <div className='side-menu-sub-container'>
                <ToggleButton style={{ fontSize: '12px', padding: '4px auto' }}
                key={sidebarMenuList[7].name}
                    onLabel={sidebarMenuList[7].name}
                    offLabel={sidebarMenuList[7].name}
                    onIcon={sidebarMenuList[7].iconName}
                    offIcon={sidebarMenuList[7].iconName}
                    checked={sidebarMenuList[7].isSelected}
                    onChange={() => setMenuItemConfig(sidebarMenuList[7])}
                    className={`sibe-menu-button ${sidebarMenuList[7].isSelected ? sidebarMenuList[7].className : ''}`}

                />
            </div>
        </div>
    );
};

export default SidebarMenu