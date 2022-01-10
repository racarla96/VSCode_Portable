package com.co3.mobilerobotcontroller;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;

import org.apache.commons.validator.routines.InetAddressValidator;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.json.*;

import static java.lang.Math.sqrt;

public class HelloController {

    // CONFIGURATION
    private String config_JSON_Name = "config.json";
    // JSON Keys
    private String Beacons_Key = "Beacons";
    private String IP_Key = "IP";
    private String UDP_Key = "UDP";

    @FXML
    private TextField TF_B1X;
    @FXML
    private TextField TF_B1Y;
    @FXML
    private TextField TF_B2X;
    @FXML
    private TextField TF_B2Y;
    @FXML
    private TextField TF_B3X;
    @FXML
    private TextField TF_B3Y;
    @FXML
    private TextField TF_B4X;
    @FXML
    private TextField TF_B4Y;

    private final Integer Number_Beacons = 4;
    private Double Beacons[] = new Double[2*Number_Beacons]; // Default values to zero

    // COMUNICATION VARIABLES
    private final double min_ComPeriod = 0.1;
    private final double max_ComPeriod = 15;
    private double ComPeriod = min_ComPeriod;

    private final String default_IPV4_Address = "127.0.0.1"; // Local loopback address
    private String IPV4_Address = default_IPV4_Address;
    private final Integer[] default_UDP_Ports = {34928, 34929}; // Recv and Send Ports
    private Integer UDP_Ports[] = default_UDP_Ports; // Recv and Send Ports

    private DatagramSocket SendSocket;
    private InetSocketAddress SendIP;
    private RobotUDPMessageListener robotUDPMessageListener = new RobotUDPMessageListener();

    private volatile RobotStateOut robotStateOut;
    private RobotStateIn robotStateIn;

    @FXML
    private TextField TF_ComPeriod;

    @FXML
    private TextField TF_IPV4;

    @FXML
    private TextField TF_UDP_Recv;
    @FXML
    private TextField TF_UDP_Send;

    // CAMERA
    private String CAM_IPV4_Address = default_IPV4_Address;
    private final Integer default_CAM_UDP_Port = 34927; // Recv and Send Ports
    private Integer CAM_UDP_Port = default_CAM_UDP_Port; // Recv and Send Ports
    private int CAM_Counter;
    private int CAM_DecimPeriod_Trigger = 1;
    private InetSocketAddress CAM_SendIP;
    private CameraUDPMessage cameraUDPMessageListener = new CameraUDPMessage();
    private CameraStateIn CameraStateGet;
    private boolean CAM_NEW_DATA;

    @FXML
    private TextField TF_CAM_DecimPeriod;

    @FXML
    private TextField TF_CAM_IPV4;

    @FXML
    private TextField TF_CAM_UDP_Send;

    @FXML
    private CheckBox CB_CAM_Enabled;
    private boolean CAM_Enabled;

    // CONTROL MODES
    ObservableList<String> CM_list = FXCollections.observableArrayList("MANUAL", "AUTOMATIC - SQUARE");
    @FXML
    private ChoiceBox CB_CTRL_Mode;
    @FXML
    private VBox Manual_Mode;
    @FXML
    private ScrollPane Automatic_Mode;
    @FXML
    private VBox Automatic_Pure_Pursuit_Square_Mode;

    // MANUAL MODE
    private double MM_Slider_Gain;
    @FXML
    private Slider MM_BTN_SLIDER_GAIN;

    // AUTOMATIC PURE PURSUIT SQUARE MODE


    // PLOTS - CHARTS
    // XY PLOT
    @FXML
    private LineChart PLOT_XY_LC;
    // POSITIONS
    // tX PLOT
    @FXML
    private LineChart PLOT_tX_LC;
    @FXML
    private NumberAxis PLOT_tX_NAxis_X;
    // tY PLOT
    @FXML
    private LineChart PLOT_tY_LC;
    @FXML
    private NumberAxis PLOT_tY_NAxis_X;
    // tO PLOT
    @FXML
    private LineChart PLOT_tO_LC;
    @FXML
    private NumberAxis PLOT_tO_NAxis_X;
    // VELOCITIES
    // tVX PLOT
    @FXML
    private LineChart PLOT_tVX_LC;
    @FXML
    private NumberAxis PLOT_tVX_NAxis_X;
    // tVY PLOT
    @FXML
    private LineChart PLOT_tVY_LC;
    @FXML
    private NumberAxis PLOT_tVY_NAxis_X;
    // tWO PLOT
    @FXML
    private LineChart PLOT_tWO_LC;
    @FXML
    private NumberAxis PLOT_tWO_NAxis_X;
    // MOTORS
    // MOTOR 1 - tW1 PLOT
    @FXML
    private LineChart PLOT_tW1_LC;
    @FXML
    private NumberAxis PLOT_tW1_NAxis_X;
    // MOTOR 2 - tW2 PLOT
    @FXML
    private LineChart PLOT_tW2_LC;
    @FXML
    private NumberAxis PLOT_tW2_NAxis_X;
    // MOTOR 3 - tW3 PLOT
    @FXML
    private LineChart PLOT_tW3_LC;
    @FXML
    private NumberAxis PLOT_tW3_NAxis_X;
    // MOTOR 4 - tW4 PLOT
    @FXML
    private LineChart PLOT_tW4_LC;
    @FXML
    private NumberAxis PLOT_tW4_NAxis_X;
    // BATTERY
    // VOLTAGE
    @FXML
    private LineChart PLOT_BATT_tV_LC;
    @FXML
    private NumberAxis PLOT_BATT_tV_NAxis_X;
    // INTENSITY
    @FXML
    private LineChart PLOT_BATT_tI_LC;
    @FXML
    private NumberAxis PLOT_BATT_tI_NAxis_X;

    // DATA SERIES
    // XY PLOT
    XYChart.Series PLOT_XY_DT_BEACONS = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_OUT = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_OUT_NOW = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_CAM = new XYChart.Series();

    // POSITIONS
    // tX PLOT
    XYChart.Series PLOT_tX_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tX_DT_OUT = new XYChart.Series();
    XYChart.Series PLOT_tX_DT_CAM = new XYChart.Series();

    // tY PLOT
    XYChart.Series PLOT_tY_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tY_DT_OUT = new XYChart.Series();
    XYChart.Series PLOT_tY_DT_CAM = new XYChart.Series();

    // tO PLOT
    XYChart.Series PLOT_tO_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tO_DT_OUT = new XYChart.Series();
    XYChart.Series PLOT_tO_DT_CAM = new XYChart.Series();

    // VELOCITIES
    // tVX PLOT
    XYChart.Series PLOT_tVX_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tVX_DT_OUT = new XYChart.Series();
    // tVY PLOT
    XYChart.Series PLOT_tVY_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tVY_DT_OUT = new XYChart.Series();
    // tWO PLOT
    XYChart.Series PLOT_tWO_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tWO_DT_OUT = new XYChart.Series();
    // MOTORS
    // MOTOR 1 - tW1 PLOT
    XYChart.Series PLOT_tW1_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tW1_DT_OUT = new XYChart.Series();
    // MOTOR 2 - tW2 PLOT
    XYChart.Series PLOT_tW2_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tW2_DT_OUT = new XYChart.Series();
    // MOTOR 3 - tW3 PLOT
    XYChart.Series PLOT_tW3_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tW3_DT_OUT = new XYChart.Series();
    // MOTOR 4 - tW4 PLOT
    XYChart.Series PLOT_tW4_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tW4_DT_OUT = new XYChart.Series();
    // BATTERY
    // VOLTAGE
    XYChart.Series PLOT_BATT_tV_DT_OUT = new XYChart.Series();
    // INTENSITY
    XYChart.Series PLOT_BATT_tI_DT_OUT = new XYChart.Series();

    // TIMER
    private final int WINDOW_1 = 1;
    private final int WINDOW_SIZE = 100;
    double t;
    double WINDOW_MOD_COUNTER;
    double WINDOW_COUNTER;
    private boolean isRunningService = false;
    private ScheduledExecutorService scheduledExecutorService;
    @FXML
    private Button BTN_StartStop;

    // TESTING
    Random rand = new Random(); //instance of random class

    // PERFORMANCE
// https://stackoverflow.com/questions/34771612/javafx-linechart-performance
    // TIMER
    // https://levelup.gitconnected.com/realtime-charts-with-javafx-ed33c46b9c8d
    // TIMER - UI
    // https://stackoverflow.com/questions/16128423/how-to-update-the-label-box-every-2-seconds-in-java-fx

    public HelloController() {}

    @FXML
    public void initialize() {
        // Load Config method
        BTN_Load_Config_Action();

        CB_CTRL_Mode.getItems().addAll(CM_list);
        CB_CTRL_Mode.getSelectionModel().selectFirst();
        auto_select_Mode_View();

        CB_CTRL_Mode.setOnAction((e) -> {
            auto_select_Mode_View();
        });

        initPlots();

        install_TextField_Listeners();

        robotStateOut = new RobotStateOut();
    }


    private void initPlots() {
        // DATA SERIES
        // XT PLOT
        PLOT_XY_DT_BEACONS.setName("Beacons");
        PLOT_XY_DT_BEACONS.getData().add(new XYChart.Data( Beacons[0], Beacons[1]));
        PLOT_XY_DT_BEACONS.getData().add(new XYChart.Data( Beacons[2], Beacons[3]));
        PLOT_XY_DT_BEACONS.getData().add(new XYChart.Data( Beacons[4], Beacons[5]));
        PLOT_XY_DT_BEACONS.getData().add(new XYChart.Data( Beacons[6], Beacons[7]));

        PLOT_XY_DT_REF.setName("Reference");
        PLOT_XY_DT_OUT.setName("Output");
        PLOT_XY_DT_OUT_NOW.setName("REMOVE");
        PLOT_XY_DT_CAM.setName("Camera");

        // POSITIONS
        // tX PLOT
        PLOT_tX_DT_REF.setName("Reference");
        PLOT_tX_DT_OUT.setName("Output");
        PLOT_tX_DT_CAM.setName("Camera");

        PLOT_tX_DT_OUT.getData().add(new XYChart.Data<>(0,0));
        PLOT_tX_NAxis_X.setAutoRanging(false);
        PLOT_tX_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tX_NAxis_X.setMinorTickCount(10);

        // tY PLOT
        PLOT_tY_DT_REF.setName("Reference");
        PLOT_tY_DT_OUT.setName("Output");
        PLOT_tY_DT_CAM.setName("Camera");

        PLOT_tY_NAxis_X.setAutoRanging(false);
        PLOT_tY_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tY_NAxis_X.setMinorTickCount(10);

        // tO PLOT
        PLOT_tO_DT_REF.setName("Reference");
        PLOT_tO_DT_OUT.setName("Output");
        PLOT_tO_DT_CAM.setName("Camera");

        PLOT_tO_NAxis_X.setAutoRanging(false);
        PLOT_tO_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tO_NAxis_X.setMinorTickCount(10);

        // VELOCITIES
        // tVX PLOT
        PLOT_tVX_DT_REF.setName("Reference");
        PLOT_tVX_DT_OUT.setName("Output");
        PLOT_tVX_NAxis_X.setAutoRanging(false);
        PLOT_tVX_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tVX_NAxis_X.setMinorTickCount(10);
        // tVY PLOT
        PLOT_tVY_DT_REF.setName("Reference");
        PLOT_tVY_DT_OUT.setName("Output");
        PLOT_tVY_NAxis_X.setAutoRanging(false);
        PLOT_tVY_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tVY_NAxis_X.setMinorTickCount(10);
        // tWO PLOT
        PLOT_tWO_DT_REF.setName("Reference");
        PLOT_tWO_DT_OUT.setName("Output");
        PLOT_tWO_NAxis_X.setAutoRanging(false);
        PLOT_tWO_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tWO_NAxis_X.setMinorTickCount(10);
        // MOTORS
        // MOTOR 1 - tW1 PLOT
        PLOT_tW1_DT_REF.setName("Reference");
        PLOT_tW1_DT_OUT.setName("Output");
        PLOT_tW1_NAxis_X.setAutoRanging(false);
        PLOT_tW1_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tW1_NAxis_X.setMinorTickCount(10);
        // MOTOR 2 - tW2 PLOT
        PLOT_tW2_DT_REF.setName("Reference");
        PLOT_tW2_DT_OUT.setName("Output");
        PLOT_tW2_NAxis_X.setAutoRanging(false);
        PLOT_tW2_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tW2_NAxis_X.setMinorTickCount(10);
        // MOTOR 3 - tW1 PLOT
        PLOT_tW3_DT_REF.setName("Reference");
        PLOT_tW3_DT_OUT.setName("Output");
        PLOT_tW3_NAxis_X.setAutoRanging(false);
        PLOT_tW3_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tW3_NAxis_X.setMinorTickCount(10);
        // MOTOR 4 - tW4 PLOT
        PLOT_tW4_DT_REF.setName("Reference");
        PLOT_tW4_DT_OUT.setName("Output");
        PLOT_tW4_NAxis_X.setAutoRanging(false);
        PLOT_tW4_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tW4_NAxis_X.setMinorTickCount(10);
        // BATTERY
        // VOLTAGE
        PLOT_BATT_tV_DT_OUT.setName("Output");
        PLOT_BATT_tV_NAxis_X.setAutoRanging(false);
        PLOT_BATT_tV_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_BATT_tV_NAxis_X.setMinorTickCount(10);
        // INTENSITY
        PLOT_BATT_tI_DT_OUT.setName("Output");
        PLOT_BATT_tI_NAxis_X.setAutoRanging(false);
        PLOT_BATT_tI_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_BATT_tI_NAxis_X.setMinorTickCount(10);

        // ASSOCIATE DATASERIES TO CHARTS
        // XY PLOT
        PLOT_XY_LC.getData().add(PLOT_XY_DT_BEACONS);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_REF);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_OUT);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_CAM);
        // POSITIONS
        // tX PLOT
        PLOT_tX_LC.getData().add(PLOT_tX_DT_REF);
        PLOT_tX_LC.getData().add(PLOT_tX_DT_OUT);
        PLOT_tX_LC.getData().add(PLOT_tX_DT_CAM);

        // tY PLOT
        PLOT_tY_LC.getData().add(PLOT_tY_DT_REF);
        PLOT_tY_LC.getData().add(PLOT_tY_DT_OUT);
        PLOT_tY_LC.getData().add(PLOT_tY_DT_CAM);

        // tO PLOT
        PLOT_tO_LC.getData().add(PLOT_tO_DT_REF);
        PLOT_tO_LC.getData().add(PLOT_tO_DT_OUT);
        PLOT_tO_LC.getData().add(PLOT_tO_DT_CAM);

        // VELOCITIES
        // tVX PLOT
        PLOT_tVX_LC.getData().add(PLOT_tVX_DT_REF);
        PLOT_tVX_LC.getData().add(PLOT_tVX_DT_OUT);
        // tVY PLOT
        PLOT_tVY_LC.getData().add(PLOT_tVY_DT_REF);
        PLOT_tVY_LC.getData().add(PLOT_tVY_DT_OUT);
        // tWO PLOT
        PLOT_tWO_LC.getData().add(PLOT_tWO_DT_REF);
        PLOT_tWO_LC.getData().add(PLOT_tWO_DT_OUT);
        // MOTORS
        // MOTOR 1 - tW1 PLOT
        PLOT_tW1_LC.getData().add(PLOT_tW1_DT_REF);
        PLOT_tW1_LC.getData().add(PLOT_tW1_DT_OUT);
        // MOTOR 2 - tW2 PLOT
        PLOT_tW2_LC.getData().add(PLOT_tW2_DT_REF);
        PLOT_tW2_LC.getData().add(PLOT_tW2_DT_OUT);
        // MOTOR 3 - tW3 PLOT
        PLOT_tW3_LC.getData().add(PLOT_tW3_DT_REF);
        PLOT_tW3_LC.getData().add(PLOT_tW3_DT_OUT);
        // MOTOR 4 - tW4 PLOT
        PLOT_tW4_LC.getData().add(PLOT_tW4_DT_REF);
        PLOT_tW4_LC.getData().add(PLOT_tW4_DT_OUT);
        // BATTERY
        // VOLTAGE
        PLOT_BATT_tV_LC.getData().add(PLOT_BATT_tV_DT_OUT);
        // INTENSITY
        PLOT_BATT_tI_LC.getData().add(PLOT_BATT_tI_DT_OUT);

        reset_plot_view();

        // FILTERING LEGEND

    }

    private void reset_plot_view(){
        // CLEAR DATA SERIES
        // XY PLOT
        PLOT_XY_DT_OUT_NOW.getData().clear();
        PLOT_XY_DT_REF.getData().clear();
        PLOT_XY_DT_OUT.getData().clear();
        // POSITIONS
        // tX PLOT
        PLOT_tX_DT_REF.getData().clear();
        PLOT_tX_DT_OUT.getData().clear();
        // tY PLOT
        PLOT_tY_DT_REF.getData().clear();
        PLOT_tY_DT_OUT.getData().clear();
        // tO PLOT
        PLOT_tO_DT_REF.getData().clear();
        PLOT_tO_DT_OUT.getData().clear();
        // VELOCITIES
        // tVX PLOT
        PLOT_tVX_DT_REF.getData().clear();
        PLOT_tVX_DT_OUT.getData().clear();
        // tVY PLOT
        PLOT_tVY_DT_REF.getData().clear();
        PLOT_tVY_DT_OUT.getData().clear();
        // tWO PLOT
        PLOT_tWO_DT_REF.getData().clear();
        PLOT_tWO_DT_OUT.getData().clear();
        // MOTORS
        // MOTOR 1 - tW1 PLOT
        PLOT_tW1_DT_REF.getData().clear();
        PLOT_tW1_DT_OUT.getData().clear();
        // MOTOR 2 - tW2 PLOT
        PLOT_tW2_DT_REF.getData().clear();
        PLOT_tW2_DT_OUT.getData().clear();
        // MOTOR 3 - tW3 PLOT
        PLOT_tW3_DT_REF.getData().clear();
        PLOT_tW3_DT_OUT.getData().clear();
        // MOTOR 4 - tW4 PLOT
        PLOT_tW4_DT_REF.getData().clear();
        PLOT_tW4_DT_OUT.getData().clear();
        // BATTERY
        // VOLTAGE
        PLOT_BATT_tV_DT_OUT.getData().clear();
        // INTENSITY
        PLOT_BATT_tI_DT_OUT.getData().clear();

        // SET AXIS
        set_Plot_View(0, WINDOW_SIZE * ComPeriod);
    }

    private void auto_select_Mode_View() {
        Manual_Mode.setDisable(true);
        Manual_Mode.setVisible(false);

        Automatic_Mode.setDisable(true);
        Automatic_Mode.setVisible(false);

        Automatic_Pure_Pursuit_Square_Mode.setDisable(true);
        Automatic_Pure_Pursuit_Square_Mode.setVisible(false);

        if (CB_CTRL_Mode.getSelectionModel().getSelectedIndex() == 0) { // MANUAL MODE
            Manual_Mode.setDisable(!true);
            Manual_Mode.setVisible(!false);
        } else if (CB_CTRL_Mode.getSelectionModel().getSelectedIndex() == 1) { // AUTOMATIC SQUARE MODE
            Automatic_Pure_Pursuit_Square_Mode.setDisable(!true);
            Automatic_Pure_Pursuit_Square_Mode.setVisible(!false);
        }
    }

    // Periodic Timer
    @FXML
    protected void BTN_StartStop_Action() {
        if(isRunningService) {
            scheduledExecutorService.shutdownNow();
            isRunningService = false;

            SendSocket.close();
            robotUDPMessageListener.stop();

            if (CAM_Enabled) {
                cameraUDPMessageListener.stop();
            }

            BTN_StartStop.setText("START");
            enable_UI_interaction();
            set_Plot_View(0, WINDOW_COUNTER * ComPeriod);
        } else {
            disable_UI_interaction();

            reset_plot_view();

//            try {
//                debug_NetworkInterfaces();
//            } catch (SocketException e) {
//                e.printStackTrace();
//            } catch (UnknownHostException e) {
//                e.printStackTrace();
//            }

            SendIP = new InetSocketAddress(IPV4_Address, UDP_Ports[1]);

            try {
                SendSocket  = new DatagramSocket();
            } catch (SocketException e) {
                e.printStackTrace();
                return;
            }

            isRunningService = true;

            BTN_StartStop.setText("STOP");
            int period = (int) (ComPeriod * 1000.0);

            // setup a scheduled executor to periodically put data into the chart
            scheduledExecutorService = Executors.newSingleThreadScheduledExecutor();

            t = 0;
            WINDOW_MOD_COUNTER = 0;
            WINDOW_COUNTER = 0;
            // put dummy data onto graph per second
            scheduledExecutorService.scheduleAtFixedRate(() -> Update_UI(), 0, period, TimeUnit.MILLISECONDS);

            Thread clientListenerThread = new Thread(this.robotUDPMessageListener);
            robotUDPMessageListener.setPort(UDP_Ports[0]);
            clientListenerThread.start(); // start thread in the background

            if (CAM_Enabled){
                CAM_SendIP = new InetSocketAddress(CAM_IPV4_Address, CAM_UDP_Port);
                Thread CAM_clientListenerThread = new Thread(this.cameraUDPMessageListener);
                cameraUDPMessageListener.setIP(CAM_SendIP);
                CAM_clientListenerThread.start(); // start thread in the background
                CAM_Counter = 1;
            }
        }
    }

    private void set_Plot_View(double low, double up) {
        // SET AXIS
        // tX PLOT
        PLOT_tX_NAxis_X.setLowerBound(low);
        PLOT_tX_NAxis_X.setUpperBound(up);
        // tY PLOT
        PLOT_tY_NAxis_X.setLowerBound(low);
        PLOT_tY_NAxis_X.setUpperBound(up);
        // tO PLOT
        PLOT_tO_NAxis_X.setLowerBound(low);
        PLOT_tO_NAxis_X.setUpperBound(up);
        // VELOCITIES
        // tVX PLOT
        PLOT_tVX_NAxis_X.setLowerBound(low);
        PLOT_tVX_NAxis_X.setUpperBound(up);
        // tVY PLOT
        PLOT_tVY_NAxis_X.setLowerBound(low);
        PLOT_tVY_NAxis_X.setUpperBound(up);
        // tWO PLOT
        PLOT_tWO_NAxis_X.setLowerBound(low);
        PLOT_tWO_NAxis_X.setUpperBound(up);
        // MOTORS
        // MOTOR 1 - tW1 PLOT
        PLOT_tW1_NAxis_X.setLowerBound(low);
        PLOT_tW1_NAxis_X.setUpperBound(up);
        // MOTOR 2 - tW2 PLOT
        PLOT_tW2_NAxis_X.setLowerBound(low);
        PLOT_tW2_NAxis_X.setUpperBound(up);
        // MOTOR 3 - tW1 PLOT
        PLOT_tW3_NAxis_X.setLowerBound(low);
        PLOT_tW3_NAxis_X.setUpperBound(up);
        // MOTOR 4 - tW4 PLOT
        PLOT_tW4_NAxis_X.setLowerBound(low);
        PLOT_tW4_NAxis_X.setUpperBound(up);
        // BATTERY
        // VOLTAGE
        PLOT_BATT_tV_NAxis_X.setLowerBound(low);
        PLOT_BATT_tV_NAxis_X.setUpperBound(up);
        // INTENSITY
        PLOT_BATT_tI_NAxis_X.setLowerBound(low);
        PLOT_BATT_tI_NAxis_X.setUpperBound(up);
    }


    // https://stackoverflow.com/questions/1881714/how-to-start-stop-restart-a-thread-in-java
    protected void Update_UI() {
        String msg = robotStateOut.toJSON();

        byte[] buffer = msg.getBytes();
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length, SendIP);
        try {
            SendSocket.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
            // Contar una perdida de paquete
        }

        if(CAM_Enabled){
            if(CAM_Counter == CAM_DecimPeriod_Trigger){
                cameraUDPMessageListener.sendMessage();
                CAM_Counter = 0;
            }
            CAM_Counter++;
        }

//        Double X = rand.nextDouble();
//        Double Y = rand.nextDouble();


        if(CAM_NEW_DATA = cameraUDPMessageListener.isFresh_data()) CameraStateGet = cameraUDPMessageListener.getCameraStateIN();

        // Update the chart
        Platform.runLater(() -> {

            // put random number with current time
//            PLOT_XY_DT_REF.getData().add(new XYChart.Data<>(X, Y));
//            PLOT_tX_DT_OUT.getData().add(new XYChart.Data<>(t, X));
//            PLOT_tY_DT_OUT.getData().add(new XYChart.Data<>(t, Y));

            if(CAM_NEW_DATA) {
                PLOT_XY_DT_CAM.getData().add(new XYChart.Data<>(CameraStateGet.X, CameraStateGet.Y));
                PLOT_tX_DT_CAM.getData().add(new XYChart.Data<>(t, CameraStateGet.X));
                PLOT_tY_DT_CAM.getData().add(new XYChart.Data<>(t, CameraStateGet.Y));
                PLOT_tO_DT_CAM.getData().add(new XYChart.Data<>(t, CameraStateGet.O));
            }

//            if (WINDOW_COUNTER > WINDOW_1) {
//                PLOT_XY_DT_REF.getData().remove(0);
//            }

            if (WINDOW_MOD_COUNTER >= WINDOW_SIZE) {
                set_Plot_View((WINDOW_COUNTER * ComPeriod), ((WINDOW_COUNTER + WINDOW_MOD_COUNTER) * ComPeriod));
                WINDOW_MOD_COUNTER = 0;
            }
        });

        t += ComPeriod;
        WINDOW_MOD_COUNTER += 1;
        WINDOW_COUNTER += 1;
    }

    // DISABLE AND ENABLE UI INTERACTION
    private void disable_UI_interaction(){
        CB_CAM_Enabled.setDisable(true);
    }

    private void enable_UI_interaction(){
        CB_CAM_Enabled.setDisable(false);
    }

    // CONTROL MODES //
    // MANUAL MODE
    @FXML
    private void MM_BTN_Interaction(ActionEvent event){
        Button source = (Button) event.getSource();
        switch (source.getId()){
            case "MM_BTN_UP_LEFT":
                robotStateOut.R_vX = Math.round(MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_vY = Math.round(MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_UP":
                robotStateOut.R_vX = MM_Slider_Gain;
                robotStateOut.R_vY = 0;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_UP_RIGHT":
                robotStateOut.R_vX = Math.round(MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_vY = Math.round(-MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_LEFT":
                robotStateOut.R_vX = 0;
                robotStateOut.R_vY = MM_Slider_Gain;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_STOP":
                robotStateOut.R_vX = 0;
                robotStateOut.R_vY = 0;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_RIGHT":
                robotStateOut.R_vX = 0;
                robotStateOut.R_vY = -MM_Slider_Gain;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_DOWN_LEFT":
                robotStateOut.R_vX = Math.round(-MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_vY = Math.round(MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_DOWN":
                robotStateOut.R_vX = -MM_Slider_Gain;
                robotStateOut.R_vY = 0;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_DOWN_RIGHT":
                robotStateOut.R_vX = Math.round(-MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_vY = Math.round(-MM_Slider_Gain * (sqrt(2.0)/2.0) * 100) / 100d;
                robotStateOut.R_wO = 0;
                break;
            case "MM_BTN_ROT_LEFT":
                robotStateOut.R_vX = 0;
                robotStateOut.R_vY = 0;
                robotStateOut.R_wO = MM_Slider_Gain;
                System.out.println("Hey");
                break;
            case "MM_BTN_ROT_RIGHT":
                robotStateOut.R_vX = 0;
                robotStateOut.R_vY = 0;
                robotStateOut.R_wO = -MM_Slider_Gain;
                System.out.println("Hey");
                break;
        }
    }

    @FXML
    private void MM_onSliderGainChange(){
        MM_Slider_Gain = Math.round(MM_BTN_SLIDER_GAIN.getValue() * 100) / 100d;
    }

    // TEXT FIELDS VALIDATIONS
    private void install_TextField_Listeners() {
        // IPv4 Listener
        TF_IPV4.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    // Getting actual text from view
                    String possible_IPV4 = TF_IPV4.getText();

                    // Get an instance of `InetAddressValidator`
                    InetAddressValidator validator = InetAddressValidator.getInstance();

                    // Validate an IPv4 address
                    if (!validator.isValidInet4Address(possible_IPV4)) TF_IPV4.setText(IPV4_Address);
                    else IPV4_Address = possible_IPV4;
                }
            }
        });

        // UDP Listeners
        TF_UDP_Recv.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    Integer number = Integer_Validation(TF_UDP_Recv.getText());
                    // Checking if is a number and Port Range check - 0 to 65535
                    if (number == null || !isPortInDomain(number)) TF_UDP_Recv.setText(UDP_Ports[0].toString());
                    else UDP_Ports[0] = number;
                }
            }
        });
        TF_UDP_Send.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    Integer number = Integer_Validation(TF_UDP_Send.getText());
                    // Checking if is a number and Port Range check - 0 to 65535
                    if (number == null || !isPortInDomain(number)) TF_UDP_Send.setText(UDP_Ports[1].toString());
                    else UDP_Ports[1] = number;
                }
            }
        });
        TF_ComPeriod.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    // Getting actual text from view
                    String possible_ComPeriod_str = TF_ComPeriod.getText();

                    Double possible_ComPeriod = Double_Validation(possible_ComPeriod_str);

                    System.out.println(possible_ComPeriod);
                    if (possible_ComPeriod == null
                            || !(possible_ComPeriod >= min_ComPeriod && possible_ComPeriod <= max_ComPeriod))
                        TF_ComPeriod.setText(String.valueOf(ComPeriod));
                    else ComPeriod = possible_ComPeriod;
                }
            }
        });

        // Beacons Validation
        TF_B1X.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B1X);
            }
        });
        TF_B1Y.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B1Y);
            }
        });
        TF_B2X.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B2X);
            }
        });
        TF_B2Y.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B2Y);
            }
        });
        TF_B3X.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B3X);
            }
        });
        TF_B3Y.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B3Y);
            }
        });
        TF_B4X.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue)
                    TF_Beacons_Validation(TF_B4X);
            }
        });
        TF_B4Y.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue) TF_Beacons_Validation(TF_B4Y);
            }
        });

        // CAMERA Listeners

        TF_CAM_UDP_Send.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    Integer number = Integer_Validation(TF_CAM_UDP_Send.getText());
                    // Checking if is a number and Port Range check - 0 to 65535
                    if (number == null || !isPortInDomain(number)) TF_CAM_UDP_Send.setText(CAM_UDP_Port.toString());
                    else CAM_UDP_Port = number;
                }
            }
        });
        // IPv4 Listener
        TF_CAM_IPV4.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    // Getting actual text from view
                    String possible_IPV4 = TF_CAM_IPV4.getText();

                    // Get an instance of `InetAddressValidator`
                    InetAddressValidator validator = InetAddressValidator.getInstance();

                    // Validate an IPv4 address
                    if (!validator.isValidInet4Address(possible_IPV4)) TF_CAM_IPV4.setText(CAM_IPV4_Address);
                    else CAM_IPV4_Address = possible_IPV4;
                }
            }
        });
        TF_CAM_DecimPeriod.focusedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if(oldValue){
                    Integer number = Integer_Validation(TF_CAM_DecimPeriod.getText());
                    // Checking if is a number and Port Range check - 0 to 65535
                    if (number == null || number < 0) TF_CAM_DecimPeriod.setText(String.valueOf(CAM_DecimPeriod_Trigger));
                    else CAM_DecimPeriod_Trigger = number;
                }
            }
        });
    }

    protected void TF_Beacons_Validation(TextField TF_Beacon) {
        int id = -1;
        switch (TF_Beacon.getId()) {
            case "TF_B1X":
                id = 0;
                break;
            case "TF_B1Y":
                id = 1;
                break;
            case "TF_B2X":
                id = 2;
                break;
            case "TF_B2Y":
                id = 3;
                break;
            case "TF_B3X":
                id = 4;
                break;
            case "TF_B3Y":
                id = 5;
                break;
            case "TF_B4X":
                id = 6;
                break;
            case "TF_B4Y":
                id = 7;
                break;
        }

        if (id >= 0) {
            Double number = Double_Validation(TF_Beacon.getText());
            if (number != null) Beacons[id] = number;
            else TF_Beacon.setText(Beacons[id].toString());
        }
    }

    // CONFIG ACTIONS
    @FXML
    protected void BTN_Save_Config_Action() {
        JSONObject config_JSON_Object = new JSONObject();
        config_JSON_Object.put(Beacons_Key, Beacons);
        config_JSON_Object.put(IP_Key, IPV4_Address);
        config_JSON_Object.put(UDP_Key, UDP_Ports);
        System.out.println();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(config_JSON_Name, false));) {
            bw.write(config_JSON_Object.toString());
            //Guardamos los cambios del fichero
            bw.flush();
            bw.close();
        } catch (IOException e) {
            System.out.println("Error E/S: " + e);
        }
    }

    @FXML
    protected void BTN_Load_Config_Action() {

        File f = new File(config_JSON_Name);
        if (f.exists() && !f.isDirectory()) {
            // Triying to find and read configuration
            try (BufferedReader br = new BufferedReader(new FileReader(config_JSON_Name))) {
                String strCurrentLine;
                if ((strCurrentLine = br.readLine()) != null) { // Assume only one line
                    try {
                        JSONObject possible_config_JSON_Object = new JSONObject(strCurrentLine);
                        Object obj;

                        if (possible_config_JSON_Object.has(Beacons_Key)) {
                            obj = possible_config_JSON_Object.get(Beacons_Key);
                            if (obj instanceof JSONArray) {
                                JSONArray aux = (JSONArray) obj;
                                if(aux.length() != Beacons.length) throw new Exception(Beacons_Key + " has not the expected vector size");
                                for (int i = 0; i < Beacons.length; i++) {
                                    try {
                                        Beacons[i] = aux.getDouble(i);
                                    } catch (Exception e) {
                                        throw new Exception(Beacons_Key + " has not the expected type in the element " + i + " of array");
                                    }
                                }
                            } else throw new Exception(Beacons_Key + " has not the expected type");
                        } else throw new Exception("The " + Beacons_Key + "has not present");

                        if (possible_config_JSON_Object.has(IP_Key)) {
                            obj = possible_config_JSON_Object.get(IP_Key);
                            if (obj instanceof String) {
                                IPV4_Address = (String) obj;
                            } else throw new Exception(IP_Key + " has not the expected type");
                        } else throw new Exception("The " + IP_Key + "has not present");

                        if (possible_config_JSON_Object.has(UDP_Key)) {
                            obj = possible_config_JSON_Object.get(UDP_Key);
                            if (obj instanceof JSONArray) {
                                JSONArray aux = (JSONArray) obj;
                                if(aux.length() != UDP_Ports.length) throw new Exception(UDP_Key + " has not the expected vector size");
                                for (int i = 0; i < UDP_Ports.length; i++) {
                                    try {
                                        UDP_Ports[i] = aux.getInt(i);
                                    } catch (Exception e) {
                                        throw new Exception(UDP_Key + " has not the expected type in the element " + i + " of array");
                                    }
                                }
                            } else throw new Exception(UDP_Key + " has not the expected type");
                        } else throw new Exception("The " + UDP_Key + "has not present");

                    } catch (JSONException err) {
                        // Show Error Alert loading config
                        Alert alert = new Alert(Alert.AlertType.ERROR);
                        alert.setHeaderText(null);
                        alert.setTitle("Error");
                        alert.setContentText("Error parsing config.json file" + ",\n" +
                                "loading default configuration.");
                        alert.showAndWait();
                        LoadDefaultConfig();
                    } catch (Exception e) {
                        // Show Error Alert loading config
                        Alert alert = new Alert(Alert.AlertType.ERROR);
                        alert.setHeaderText(null);
                        alert.setTitle("Error");
                        alert.setContentText(e.getMessage() + ",\n" +
                                "loading default configuration.");
                        alert.showAndWait();
                        LoadDefaultConfig();
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setHeaderText(null);
            alert.setTitle("Info");
            alert.setContentText("No configuration saved,\n" +
                    "loading default configuration.");
            alert.showAndWait();
            LoadDefaultConfig();
        }

        UpdateConfig();
    }

    @FXML
    private void BTN_Restore_Default_Config_Action() {
        LoadDefaultConfig();
        UpdateConfig();
        BTN_Save_Config_Action();
    }

    private void LoadDefaultConfig(){
        for (int i = 0; i < Beacons.length; i++) Beacons[i] = 0.0;
        IPV4_Address = default_IPV4_Address;
        UDP_Ports = default_UDP_Ports;

        CAM_IPV4_Address = default_IPV4_Address;
        CAM_UDP_Port = default_CAM_UDP_Port;
        CAM_DecimPeriod_Trigger = 1;
    }

    private void UpdateConfig() {
        TF_ComPeriod.setText(String.valueOf(ComPeriod));

        TF_B1X.setText(String.valueOf(Beacons[0]));
        TF_B1Y.setText(String.valueOf(Beacons[1]));
        TF_B2X.setText(String.valueOf(Beacons[2]));
        TF_B2Y.setText(String.valueOf(Beacons[3]));
        TF_B3X.setText(String.valueOf(Beacons[4]));
        TF_B3Y.setText(String.valueOf(Beacons[5]));
        TF_B4X.setText(String.valueOf(Beacons[6]));
        TF_B4Y.setText(String.valueOf(Beacons[7]));

        TF_IPV4.setText(IPV4_Address);
        TF_UDP_Recv.setText(UDP_Ports[0].toString());
        TF_UDP_Send.setText(UDP_Ports[1].toString());

        TF_CAM_IPV4.setText(CAM_IPV4_Address);
        TF_CAM_DecimPeriod.setText(String.valueOf(CAM_DecimPeriod_Trigger));
        TF_CAM_UDP_Send.setText(CAM_UDP_Port.toString());
    }

    // CAMERA
    @FXML
    private void CB_CAM_Action(){
        CAM_Enabled = CB_CAM_Enabled.isSelected();
    }


    // UTILS
    private Double Double_Validation(String possible_double_str) {
        Double possible_number = null;
        try {
            possible_number = Double.parseDouble(possible_double_str);
        } catch (NumberFormatException e) {
            //the parseDouble failed and you need to handle it here
        }
        return possible_number;
    }

    private Integer Integer_Validation(String possible_integer_str) {
        Integer possible_number = null;
        try {
            possible_number = Integer.parseInt(possible_integer_str);
        } catch (NumberFormatException e) {
            //the parseDouble failed and you need to handle it here
        }
        return possible_number;
    }

    private boolean isPortInDomain(int port){
        return port >= 0 && port <= 65535;
    }

    private void debug_NetworkInterfaces() throws SocketException, UnknownHostException {
        // getNetworkInterfaces() returns a list of all interfaces
        // present in the system.
        ArrayList<NetworkInterface> interfaces = Collections.list(
                NetworkInterface.getNetworkInterfaces());

        System.out.println("Information about present Network Interfaces...\n");
        for (NetworkInterface iface : interfaces)
        {
            // isUp() method used for checking whether the interface in process
            // is up and running or not.
            if (iface.isUp())
            {

                // getName() method
                System.out.println("Interface Name: " + iface.getName());

                // getDisplayName() method
                System.out.println("Interface display name: " + iface.getDisplayName());

                // gethardwareaddress() method
                System.out.println("Hardware Address: " +
                        Arrays.toString(iface.getHardwareAddress()));

                // getParent() method
                System.out.println("Parent: " + iface.getParent());

                // getIndex() method
                System.out.println("Index: " + iface.getIndex());
                // Interface addresses of the network interface
                System.out.println("\tInterface addresses: ");

                // getInterfaceAddresses() method
                for (InterfaceAddress addr : iface.getInterfaceAddresses())
                {
                    System.out.println("\t\t" + addr.getAddress().toString());
                }
                // Interface addresses of the network interface
                System.out.println("\tInetAddresses associated with this interface: ");

                // getInetAddresses() method returns list of all
                // addresses currently bound to this interface
                Enumeration<InetAddress> en = iface.getInetAddresses();
                while (en.hasMoreElements())
                {
                    System.out.println("\t\t" + en.nextElement().toString());
                }

                // getMTU() method
                System.out.println("\tMTU: " + iface.getMTU());

                // getSubInterfaces() method
                System.out.println("\tSubinterfaces: " +
                        Collections.list(iface.getSubInterfaces()));

                // isLoopback() method
                System.out.println("\tis loopback: " + iface.isLoopback());

                // isVirtual() method
                System.out.println("\tis virtual: " + iface.isVirtual());

                // isPointToPoint() method
                System.out.println("\tis point to point: " + iface.isPointToPoint());

                // supportsMulticast() method
                System.out.println("Supports Multicast: " + iface.supportsMulticast());

            }
        }
    }
}