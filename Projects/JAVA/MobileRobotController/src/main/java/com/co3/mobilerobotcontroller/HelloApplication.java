package com.co3.mobilerobotcontroller;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;

public class HelloApplication extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 640, 480);

//        HelloController controller = (HelloController) fxmlLoader.getController();
//        stage.setOnHidden(e -> {
//            controller.shutdown();
//            Platform.exit();
//        });

        stage.setMinWidth(640+20);
        stage.setMinHeight(480+40);

        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}