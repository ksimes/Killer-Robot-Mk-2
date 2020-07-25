package com.stronans.explorertest.core;

import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import com.stronans.explorertest.device.DeviceController;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class OverController implements Runnable {
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(OverController.class);

    private static List<DeviceController> ListOfNanos = new ArrayList<>();

    @Override
    public void run() {
        // Initialise GPIO connection
        // create gpio controller
        final GpioController gpio = GpioFactory.getInstance();

        // Setup communications with Device devices

        for (int i = 0; i < 10; i++) {
            DeviceController newNano = new DeviceController(i); // Sets the USB channel

            try {
                if (newNano.openConnection()) {  // Opens the USB serial communications
                    ListOfNanos.add(newNano);
                }
            } catch (InterruptedException | UnsupportedOperationException | IOException e) {
                log.warn("Problems with serial communications on: " + newNano.getConnectionInfo() + " Error: " + e.getMessage());
            }
        }

        // Housekeeping at shutdown of program
        Runtime.getRuntime().addShutdownHook(new Thread() {
                                                 @Override
                                                 public void run() {
                                                     for (DeviceController nano : ListOfNanos) {
                                                         nano.closeConnection();
                                                     }

                                                     gpio.shutdown();   // <--- implement this method call if you wish to terminate the Pi4J GPIO controller
                                                     log.info("Exiting program.");
                                                 }
                                             }

        );

        while (true) {
            try {
                Thread.currentThread().sleep(100);
            } catch (InterruptedException ie) {
                log.error("InterruptedException while waiting: " + ie.getMessage(), ie);
            }
        }
    }
}
