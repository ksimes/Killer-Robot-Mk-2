package com.stronans.explorertest.core;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Controller implements Runnable, MessageListener {
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(Controller.class);

    private static List<Device> ListOfNanos = new ArrayList<>();

    @Override
    public void run() {
        // Initialise GPIO connection
        // create gpio controller
        final GpioController gpio = GpioFactory.getInstance();

        // Setup communications with Device devices

        for (int i = 0; i < 10; i++) {
            Device newNano = new Device();

            newNano.setConnectionId(i);     // Sets the USB channel
            try {
                if (newNano.openConnection(this)) {  // Opens the USB serial communications
                    ListOfNanos.add(newNano);
                }
            } catch (InterruptedException | UnsupportedOperationException | IOException e) {
                log.warn(" ==>> PROBLEMS WITH SERIAL COMMUNICATIONS on : " + newNano.getConnection() + " Error " + e.getMessage());
            }
        }

        // Housekeeping at shutdown of program
        Runtime.getRuntime().addShutdownHook(new Thread() {
                                                 @Override
                                                 public void run() {
                                                     for (Device nano : ListOfNanos) {
                                                         nano.getComms().endComms();
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
                log.error(" ==>> InterruptedException while waiting: " + ie.getMessage(), ie);
            }
        }
    }

    @Override
    public void messageReceived() {
        try {
            String message = "";

            for (Device nano : ListOfNanos) {
                if (!nano.getComms().messages().isEmpty()) {
                    message = nano.getComms().messages().take();
                    processMessage(message, nano);
                }
            }

        } catch (InterruptedException e) {
            log.error("Interrupt during processing : " + e.getMessage(), e);
        }
    }

    private void processMessage(String message, Device nano) {
//        log.info("Raw message : [{}]", message);

        if (nano.isNameUnset()) {
            ObjectMapper objectMapper = new ObjectMapper();

            try {
                JsonNode jsonNode = objectMapper.readTree(message);
                JsonNode idNode = jsonNode.get("stationId");

                if (idNode != null) {
                    nano.setName(idNode.asText());
                    nano.setNameUnset(false);
                }
            } catch (JsonProcessingException e) {
                log.error("JsonProcessingException : " + e.getMessage());
            }
        }

        log.info("Msg from device {}:{}",nano.getName(), nano.getConnection());
        log.info("{}", message);
        log.info("");
    }
}
