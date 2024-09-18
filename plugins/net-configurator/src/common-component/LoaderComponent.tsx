import React from "react";
import { Dialog } from 'primereact/dialog';
import { ProgressSpinner } from "primereact/progressspinner";
type Props = {
    visible:boolean,
    text?:string
}
export default function Loader({visible,text}:Props) {
    return (
        <div >
            <Dialog header={text} visible={visible} style={{ width: '20vw' }} onHide={() =>{}}>
            <ProgressSpinner style={{left:'35%'}}/>
            </Dialog>
        </div>
    )
}