package com.co3.mobilerobotcontroller;

import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;

public class RobotStateIn {
    double X;
    double Y;
    double O;
    double VX;
    double VY;
    double WO;
    double W1;
    double W2;
    double W3;
    double W4;
    double BV;
    double BI;

    double R_VX;
    double R_VY;
    double R_WO;
    double R_W1;
    double R_W2;
    double R_W3;
    double R_W4;

    public boolean String2Object(String msg){
        try {
            JSONObject object = new JSONObject(msg);
            X = object.getDouble("X");
            Y = object.getDouble("Y");
            O = object.getDouble("O");
            VX = object.getDouble("VX");
            VY = object.getDouble("VY");
            WO = object.getDouble("WO");
            W1 = object.getDouble("W1");
            W2 = object.getDouble("W2");
            W3 = object.getDouble("W3");
            W4 = object.getDouble("W4");
            BV = object.getDouble("BATT_V");
            BI = object.getDouble("BATT_I");
            R_VX = object.getDouble("R_VX");
            R_VY = object.getDouble("R_VY");
            R_WO = object.getDouble("R_WO");
            R_W1 = object.getDouble("R_W1");
            R_W2 = object.getDouble("R_W2");
            R_W3 = object.getDouble("R_W3");
            R_W4 = object.getDouble("R_W4");
        } catch (Exception e) {
            return false;
        }
        return true;
    }
}
