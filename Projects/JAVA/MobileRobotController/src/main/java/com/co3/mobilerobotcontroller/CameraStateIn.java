package com.co3.mobilerobotcontroller;

import org.json.*;

import java.util.Arrays;
import java.util.List;

public class CameraStateIn {
    double X;
    double Y;
    double O;

    public boolean String2Object(String msg){
        List<String> e = Arrays.asList(msg.split(","));
        if(e.size() == 7) {
            X = Double.parseDouble(e.get(1));
            Y = Double.parseDouble(e.get(2));
            O = Double.parseDouble(e.get(5));
            return true;
        }
        return false;
    }
}
