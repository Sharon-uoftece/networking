import React, {useEffect, useState} from "react";
import Aos from "aos";
import "aos/dist/aos.css";
import sun from "../assets/sunlife3.jpg"
import { DatePicker } from "@blueprintjs/datetime";

function ModelIllustrate() {

    const [today, setToday] = useState<any>(null);
    
    useEffect(()=>{
        let now = new Date();
        let ymd = now.getFullYear()+'-'+(now.getMonth()+1)+'-'+now.getDate();  
        setToday(ymd);
    },[]);

    useEffect(() => {
        Aos.init({duration: 2000});
    }, []);
    
    return(
        <div className="model" id="model">
            <div className="model-connector">
                <p className="model-one-text">ML Model Illustration</p>
            </div>
            <div className="model-illustration">
                <h1>Model</h1>
                <h4>performance trend line display here</h4>
            </div>
            <div className="photos">
                {/* <div data-aos="slide-right" className="pic1">
                    <img src="https://tenstorrent.com/app/uploads/2021/04/Group-66.png" />
                </div> */}
                {/* <div data-aos="slide-left" className="pic2">
                    <img src="https://tenstorrent.com/app/uploads/2021/04/Group-66.png" />
                </div> */}
            </div>
            {/* <h5>Current day is: {today}</h5> */}
        </div>
    )
}

export default ModelIllustrate


