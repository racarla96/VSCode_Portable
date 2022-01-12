package com.co3.mobilerobotcontroller;

public class ExperimentState {
    double t;

    double CAM_X;
    double CAM_Y;
    double CAM_O;

    double R_X;
    double R_Y;
    double R_O;
    double R_VX;
    double R_VY;
    double R_WO;
    double R_W1;
    double R_W2;
    double R_W3;
    double R_W4;

    double ODOM_X;
    double ODOM_Y;
    double ODOM_O;
    double ODOM_VX;
    double ODOM_VY;
    double ODOM_WO;
    double ODOM_W1;
    double ODOM_W2;
    double ODOM_W3;
    double ODOM_W4;

    double BV;
    double BI;

    public ExperimentState(double t) {
        this.t = t;
        R_X = Double.NaN;
        R_Y = Double.NaN;
        R_O = Double.NaN;
        R_VX = Double.NaN;
        R_VY = Double.NaN;
        R_WO = Double.NaN;
        R_W1 = Double.NaN;
        R_W2 = Double.NaN;
        R_W3 = Double.NaN;
        R_W4 = Double.NaN;

        CAM_X = Double.NaN;
        CAM_Y = Double.NaN;
        CAM_O = Double.NaN;

        ODOM_X = Double.NaN;
        ODOM_Y = Double.NaN;
        ODOM_O = Double.NaN;
        ODOM_VX = Double.NaN;
        ODOM_VY = Double.NaN;
        ODOM_WO = Double.NaN;
        ODOM_W1 = Double.NaN;
        ODOM_W2 = Double.NaN;
        ODOM_W3 = Double.NaN;
        ODOM_W4 = Double.NaN;

        BV = Double.NaN;
        BI = Double.NaN;
    }

    public String Header(){
        return  "t," +
                ",CAM_X" +
                ",CAM_Y" +
                ",CAM_O" +
                ",R_X" +
                ",R_Y" +
                ",R_O" +
                ",R_VX" +
                ",R_VY" +
                ",R_WO" +
                ",R_W1" +
                ",R_W2" +
                ",R_W3" +
                ",R_W4" +
                ",ODOM_X" +
                ",ODOM_Y" +
                ",ODOM_O" +
                ",ODOM_VX" +
                ",ODOM_VY" +
                ",ODOM_WO" +
                ",ODOM_W1" +
                ",ODOM_W2" +
                ",ODOM_W3" +
                ",ODOM_W4" +
                ",BV" +
                ",BI";
    }

    @Override
    public String toString() {
        return t +
                "," + CAM_X +
                "," + CAM_Y +
                "," + CAM_O +
                "," + R_X +
                "," + R_Y +
                "," + R_O +
                "," + R_VX +
                "," + R_VY +
                "," + R_WO +
                "," + R_W1 +
                "," + R_W2 +
                "," + R_W3 +
                "," + R_W4 +
                "," + ODOM_X +
                "," + ODOM_Y +
                "," + ODOM_O +
                "," + ODOM_VX +
                "," + ODOM_VY +
                "," + ODOM_WO +
                "," + ODOM_W1 +
                "," + ODOM_W2 +
                "," + ODOM_W3 +
                "," + ODOM_W4 +
                "," + BV +
                "," + BI;
    }

    public void setR_X(double r_X) {
        R_X = r_X;
    }

    public void setR_Y(double r_Y) {
        R_Y = r_Y;
    }

    public void setR_O(double r_O) {
        R_O = r_O;
    }

    public void setR_VX(double r_VX) {
        R_VX = r_VX;
    }

    public void setR_VY(double r_VY) {
        R_VY = r_VY;
    }

    public void setR_WO(double r_WO) {
        R_WO = r_WO;
    }

    public void setR_W1(double r_W1) {
        R_W1 = r_W1;
    }

    public void setR_W2(double r_W2) {
        R_W2 = r_W2;
    }

    public void setR_W3(double r_W3) {
        R_W3 = r_W3;
    }

    public void setR_W4(double r_W4) {
        R_W4 = r_W4;
    }

    public void setCAM_X(double CAM_X) {
        this.CAM_X = CAM_X;
    }

    public void setCAM_Y(double CAM_Y) {
        this.CAM_Y = CAM_Y;
    }

    public void setCAM_O(double CAM_O) {
        this.CAM_O = CAM_O;
    }

    public void setODOM_X(double ODOM_X) {
        this.ODOM_X = ODOM_X;
    }

    public void setODOM_Y(double ODOM_Y) {
        this.ODOM_Y = ODOM_Y;
    }

    public void setODOM_O(double ODOM_O) {
        this.ODOM_O = ODOM_O;
    }

    public void setODOM_VX(double ODOM_VX) {
        this.ODOM_VX = ODOM_VX;
    }

    public void setODOM_VY(double ODOM_VY) {
        this.ODOM_VY = ODOM_VY;
    }

    public void setODOM_WO(double ODOM_WO) {
        this.ODOM_WO = ODOM_WO;
    }

    public void setODOM_W1(double ODOM_W1) {
        this.ODOM_W1 = ODOM_W1;
    }

    public void setODOM_W2(double ODOM_W2) {
        this.ODOM_W2 = ODOM_W2;
    }

    public void setODOM_W3(double ODOM_W3) {
        this.ODOM_W3 = ODOM_W3;
    }

    public void setODOM_W4(double ODOM_W4) {
        this.ODOM_W4 = ODOM_W4;
    }

    public void setBV(double BV) {
        this.BV = BV;
    }

    public void setBI(double BI) {
        this.BI = BI;
    }

    public void setState(String[] line_split) {
        CAM_X = Double.parseDouble(line_split[1]);
        CAM_Y = Double.parseDouble(line_split[2]);
        CAM_O = Double.parseDouble(line_split[3]);

        R_X = Double.parseDouble(line_split[4]);
        R_Y = Double.parseDouble(line_split[5]);
        R_O = Double.parseDouble(line_split[6]);
        R_VX = Double.parseDouble(line_split[7]);
        R_VY = Double.parseDouble(line_split[8]);
        R_WO = Double.parseDouble(line_split[9]);
        R_W1 = Double.parseDouble(line_split[10]);
        R_W2 = Double.parseDouble(line_split[11]);
        R_W3 = Double.parseDouble(line_split[12]);
        R_W4 = Double.parseDouble(line_split[13]);

        ODOM_X = Double.parseDouble(line_split[14]);
        ODOM_Y = Double.parseDouble(line_split[15]);
        ODOM_O = Double.parseDouble(line_split[16]);
        ODOM_VX = Double.parseDouble(line_split[17]);
        ODOM_VY = Double.parseDouble(line_split[18]);
        ODOM_WO = Double.parseDouble(line_split[19]);
        ODOM_W1 = Double.parseDouble(line_split[20]);
        ODOM_W2 = Double.parseDouble(line_split[21]);
        ODOM_W3 = Double.parseDouble(line_split[22]);
        ODOM_W4 = Double.parseDouble(line_split[23]);

        BV = Double.parseDouble(line_split[24]);
        BI = Double.parseDouble(line_split[25]);
    }
}
