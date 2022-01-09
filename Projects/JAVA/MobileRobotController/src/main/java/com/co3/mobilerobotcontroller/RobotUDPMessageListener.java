package com.co3.mobilerobotcontroller;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

public class RobotUDPMessageListener implements Runnable {

    private volatile boolean running = false;
    private volatile int port = 4000;
    private volatile RobotStateIn robotStateIN;

    @Override
    public void run() {
//        System.out.println("Hello World!");

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
        return robotStateIN;
    }
}
