package com.co3.mobilerobotcontroller;

public class RobotStateOut {
    int ID;
    double R_vX;
    double R_vY;
    double R_wO;

    public RobotStateOut(){
        ID = 0;
        R_vX = 0;
        R_vY = 0;
        R_wO = 0;
    }

    public String toJSON(){
        return "{\"ID\":" + ID +
                ",\"R_vX\":" + R_vX +
                ",\"R_vY\":" + R_vY +
                ",\"R_wO\":" + R_wO +
                "}";
    }
}
