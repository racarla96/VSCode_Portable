package com.co3.mobilerobotcontroller;

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
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.json.*;

import static java.lang.Math.sqrt;

public class HelloController {

    // ROBOT STATES
    double t;
    double tc; // tc is time in counts

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


    // CONTROL MODES
    ObservableList<String> CM_list = FXCollections.observableArrayList("MANUAL", "AUTOMATIC");
    @FXML
    private ChoiceBox CB_CTRL_Mode;
    @FXML
    private VBox Manual_Mode;
    @FXML
    private ScrollPane Automatic_Mode;

    // MANUAL MODE
    private double MM_Slider_Gain;
    @FXML
    private Slider MM_BTN_SLIDER_GAIN;

    // PLOTS - CHARTS
    // XY PLOT
    @FXML
    private LineChart PLOT_XY_LC;
    @FXML
    private NumberAxis PLOT_XY_NB_X;
    // POSITIONS
    // tX PLOT
    @FXML
    private LineChart PLOT_tX_LC;
    @FXML
    private NumberAxis PLOT_tX_NAxis_X;
    // tY PLOT
    @FXML
    private LineChart PLOT_tY_LC;
    // tO PLOT
    @FXML
    private LineChart PLOT_tO_LC;
    // VELOCITIES
    // tVX PLOT
    @FXML
    private LineChart PLOT_tVX_LC;
    // tVY PLOT
    @FXML
    private LineChart PLOT_tVY_LC;
    // tWO PLOT
    @FXML
    private LineChart PLOT_tWO_LC;
    // MOTORS
    // MOTOR 1 - tW1 PLOT
    @FXML
    private LineChart PLOT_tW1_LC;
    // MOTOR 2 - tW2 PLOT
    @FXML
    private LineChart PLOT_tW2_LC;
    // MOTOR 3 - tW3 PLOT
    @FXML
    private LineChart PLOT_tW3_LC;
    // MOTOR 4 - tW4 PLOT
    @FXML
    private LineChart PLOT_tW4_LC;
    // BATTERY
    // VOLTAGE
    @FXML
    private LineChart PLOT_BATT_tV_LC;
    // INTENSITY
    @FXML
    private LineChart PLOT_BATT_tI_LC;

    // DATA SERIES
    // XY PLOT
    XYChart.Series PLOT_XY_DT_BEACONS = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_OUT_NOW = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_XY_DT_OUT = new XYChart.Series();
    // POSITIONS
    // tX PLOT
    XYChart.Series PLOT_tX_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tX_DT_OUT = new XYChart.Series();
    // tY PLOT
    XYChart.Series PLOT_tY_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tY_DT_OUT = new XYChart.Series();
    // tO PLOT
    XYChart.Series PLOT_tO_DT_REF = new XYChart.Series();
    XYChart.Series PLOT_tO_DT_OUT = new XYChart.Series();
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
    private boolean isRunningService = false;
    private ScheduledExecutorService scheduledExecutorService;
    @FXML
    private Button BTN_StartStop;

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

        // POSITIONS
        // tX PLOT
        PLOT_tX_DT_REF.setName("Reference");
        PLOT_tX_DT_OUT.setName("Output");
        PLOT_tX_DT_OUT.getData().add(new XYChart.Data<>(0,0));

        // tY PLOT
        PLOT_tY_DT_REF.setName("Reference");
        PLOT_tY_DT_OUT.setName("Output");
        // tO PLOT
        PLOT_tO_DT_REF.setName("Reference");
        PLOT_tO_DT_OUT.setName("Output");
        // VELOCITIES
        // tVX PLOT
        PLOT_tVX_DT_REF.setName("Reference");
        PLOT_tVX_DT_OUT.setName("Output");
        // tVY PLOT
        PLOT_tVY_DT_REF.setName("Reference");
        PLOT_tVY_DT_OUT.setName("Output");
        // tWO PLOT
        PLOT_tWO_DT_REF.setName("Reference");
        PLOT_tWO_DT_OUT.setName("Output");
        // MOTORS
        // MOTOR 1 - tW1 PLOT
        PLOT_tW1_DT_REF.setName("Reference");
        PLOT_tW1_DT_OUT.setName("Output");
        // MOTOR 2 - tW2 PLOT
        PLOT_tW2_DT_REF.setName("Reference");
        PLOT_tW2_DT_OUT.setName("Output");
        // MOTOR 3 - tW1 PLOT
        PLOT_tW3_DT_REF.setName("Reference");
        PLOT_tW3_DT_OUT.setName("Output");
        // MOTOR 4 - tW4 PLOT
        PLOT_tW4_DT_REF.setName("Reference");
        PLOT_tW4_DT_OUT.setName("Output");
        // BATTERY
        // VOLTAGE
        PLOT_BATT_tV_DT_OUT.setName("Output");
        // INTENSITY
        PLOT_BATT_tI_DT_OUT.setName("Output");

        // PLOTS - CHARTS
        // XY PLOT
        PLOT_XY_LC.getData().add(PLOT_XY_DT_BEACONS);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_REF);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_OUT);
        PLOT_XY_LC.getData().add(PLOT_XY_DT_OUT_NOW);
        // POSITIONS
        // tX PLOT
        PLOT_tX_LC.getData().add(PLOT_tX_DT_REF);
        PLOT_tX_LC.getData().add(PLOT_tX_DT_OUT);
        PLOT_tX_NAxis_X.setAutoRanging(false);
        PLOT_tX_NAxis_X.setLowerBound(0);
        PLOT_tX_NAxis_X.setUpperBound((WINDOW_SIZE + 1) * ComPeriod);
        PLOT_tX_NAxis_X.setTickUnit(ComPeriod * 10);
        PLOT_tX_NAxis_X.setMinorTickCount(10);
        // tY PLOT
        PLOT_tY_LC.getData().add(PLOT_tY_DT_REF);
        PLOT_tY_LC.getData().add(PLOT_tY_DT_OUT);
        // tO PLOT
        PLOT_tO_LC.getData().add(PLOT_tO_DT_REF);
        PLOT_tO_LC.getData().add(PLOT_tO_DT_OUT);
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
    }

    private void auto_select_Mode_View() {
        if (CB_CTRL_Mode.getSelectionModel().getSelectedIndex() == 0) {
            Manual_Mode.setDisable(false);
            Manual_Mode.setVisible(true);

            Automatic_Mode.setDisable(!false);
            Automatic_Mode.setVisible(!true);
        } else {
            Manual_Mode.setDisable(!false);
            Manual_Mode.setVisible(!true);

            Automatic_Mode.setDisable(false);
            Automatic_Mode.setVisible(true);
        }
    }

    // Periodic Timer
    @FXML
    protected void BTN_StartStop_Action() {
        if(isRunningService) {
            scheduledExecutorService.shutdownNow();
            isRunningService = false;
            BTN_StartStop.setText("START");

            SendSocket.close();
            robotUDPMessageListener.stop();
        } else {

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
            tc = 0;
            // put dummy data onto graph per second
            scheduledExecutorService.scheduleAtFixedRate(() -> Update_UI(), 0, period, TimeUnit.MILLISECONDS);

            Thread clientListenerThread = new Thread(this.robotUDPMessageListener);
            robotUDPMessageListener.setPort(UDP_Ports[0]);
            clientListenerThread.start(); // start thread in the background
        }
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


//        // Update the chart
//        Platform.runLater(() -> {
//
//            // put random number with current time
//            PLOT_XY_DT_REF.getData().add(new XYChart.Data<>(X, Y));
//            PLOT_tX_DT_OUT.getData().add(new XYChart.Data<>(t, X));
//            PLOT_tY_DT_OUT.getData().add(new XYChart.Data<>(t, Y));
//
//            if (tc > WINDOW_1) {
//                PLOT_XY_DT_REF.getData().remove(0);
//            }
//
//            if (tc > WINDOW_SIZE) {
//                //PLOT_tX_DT_OUT.getData().remove(0);
//                PLOT_tY_DT_OUT.getData().remove(0);
//                PLOT_tX_NAxis_X.setLowerBound(t - WINDOW_SIZE * ComPeriod);
//                PLOT_tX_NAxis_X.setUpperBound(t + ComPeriod);
//            }
//        });
//
//        t += ComPeriod;
//        tc += 1;
    }

    // DISABLE AND ENABLE UI INTERACTION
    private void disable_UI_interaction(){

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
}