module com.co3.mobilerobotcontroller {
    requires javafx.controls;
    requires javafx.fxml;
    requires commons.validator;
    requires org.json;
    requires java.desktop;

    opens com.co3.mobilerobotcontroller to javafx.fxml;
    exports com.co3.mobilerobotcontroller;
}