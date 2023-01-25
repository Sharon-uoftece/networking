import React, {useState, useEffect} from "react"
import { DatePicker } from "@blueprintjs/datetime";
import Aos from "aos";
import "aos/dist/aos.css";

function PreviousLabels() {
    const [today, setToday] = useState<any>(new Date());
    const [pickedDay, setPickedDay] = useState<any>(null);

    return (
        <div className="labels" id="labels">
            <div className="labels-connector">
                <p className="labels-one-text">Previous Labels</p>
            </div>
            <div className="labels-illustration">
                <h1>Labelling history </h1>
            </div>
            <div className="labels-picktime">
                <DatePicker
                    className="daypick-model"
                    clearButtonText="Deselect"
                    todayButtonText="Go To Today"
                    maxDate={today}
                    minDate={new Date("1/1/2018")}
                    // defaultValue={today}
                    onChange={newDate => setPickedDay(newDate)}
                    showActionsBar={true}
                />
            </div>
            <div className="label-header-1">
                {pickedDay && <h1>Showing labelling records on {pickedDay.getFullYear()}-{pickedDay.getMonth() + 1}-{pickedDay.getDate()}</h1>}
            </div>
            
        </div>
    )
}

export default PreviousLabels