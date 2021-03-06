package com.co3.mobilerobotcontroller;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

public class RobotUDPMessageListener implements Runnable {

    private volatile boolean running = false;
    private volatile int port = 34929;
    private volatile boolean fresh_data = false;
    private volatile RobotStateIn robotStateIN;
    private RobotStateIn robotStateIN_Copy;

    @Override
    public void run() {
//        System.out.println("Hello World!");

        robotStateIN_Copy = new RobotStateIn();

        byte[] buffer = new byte[1500]; // MTU = 1500 bytes
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        DatagramSocket listener;
        try {
            listener = new DatagramSocket(port);
        } catch (SocketException e) {
            e.printStackTrace();
            return;
        }
        try {
            listener.setSoTimeout(100); // in ms
        } catch (SocketException e) {
            e.printStackTrace();
        }
        running = true;
        boolean packet_received = false;
        while (running) {
            try {
                listener.receive(packet);
                packet_received = true;
            } catch (SocketTimeoutException e) {
                //e.printStackTrace();
                packet_received = false;
//                System.out.println("Not received!");
            } catch (IOException e) {
                e.printStackTrace();
            }
            if(packet_received) {
//                System.out.println(packet.getData().toString());
                String message = new String( packet.getData(), 0,
                        packet.getLength() );
                if(robotStateIN_Copy.String2Object(message)){
                    fresh_data = true;
                    robotStateIN = robotStateIN_Copy;
                }
            }
        }
        listener.close();
    }

    public void stop() {
        running = false;
    }

    public void setPort(int port){
        this.port = port;
    }

    public RobotStateIn getRobotStateIN(){
        fresh_data = false;
        return robotStateIN;
    }

    public boolean isFresh_data(){
        return fresh_data;
    }
}
