package com.co3.mobilerobotcontroller;

import java.io.IOException;
import java.net.*;

public class CameraUDPMessage implements Runnable {

    private volatile boolean running = false;
    private volatile boolean fresh_data = false;
    private volatile CameraStateIn CameraStateIN;
    private CameraStateIn CameraStateIN_Copy;

    private volatile boolean send_message_flag = false;
    private volatile InetSocketAddress IP;

    @Override
    public void run() {
        CameraStateIN_Copy = new CameraStateIn();

        byte[] buffer = new byte[100]; // MTU = 1500 bytes
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        DatagramSocket listener;
        try {
            listener = new DatagramSocket(IP.getPort());
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
            if(send_message_flag){
                String CAM_msg = "GIVE_ME_POSE_PLEASE";
                byte[] CAM_buffer = CAM_msg.getBytes();
                DatagramPacket CAM_packet = new DatagramPacket(CAM_buffer, CAM_buffer.length, IP);
                try {
                    listener.send(CAM_packet);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                send_message_flag = false;
            }
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
                String message = new String( packet.getData(), 0,
                        packet.getLength() );
                if(CameraStateIN_Copy.String2Object(message)){
                    fresh_data = true;
                    CameraStateIN = CameraStateIN_Copy;
                }
            }
        }
        listener.close();
    }

    public void stop() {
        running = false;
    }

    public CameraStateIn getCameraStateIN(){
        fresh_data = false;
        return CameraStateIN;
    }

    public boolean isFresh_data(){
        return fresh_data;
    }

    public void sendMessage(){
        send_message_flag = true;
    }

    public void setIP(InetSocketAddress ip){
        IP = ip;
    }
}
