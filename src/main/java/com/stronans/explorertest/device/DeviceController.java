package com.stronans.explorertest.device;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;

public class DeviceController implements MessageListener {
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(DeviceController.class);
    private Device deviceBeingControlled;

    public DeviceController(int usbPort, int speed, String name) {
        deviceBeingControlled = new Device();

        if (speed > 0) {
            deviceBeingControlled.setSpeed(speed);
        }

        if (!name.isEmpty()) {
            deviceBeingControlled.setName(name);
        }

        if (usbPort > 0) {
            deviceBeingControlled.setConnectionId(usbPort);
        }
    }

    public DeviceController(int usbPort) {
        this(usbPort, 0, null);
    }

    public String getConnectionInfo() {
        return deviceBeingControlled.getConnection();
    }

    public boolean openConnection() throws InterruptedException, UnsupportedOperationException, IOException {
        return deviceBeingControlled.openConnection(this);
    }

    public void closeConnection() {
        deviceBeingControlled.getComms().endComms();
    }

    @Override
    public void messageReceived() {
        try {
            String message;

            if (!deviceBeingControlled.getComms().messages().isEmpty()) {
                message = deviceBeingControlled.getComms().messages().take();
                processMessage(message, deviceBeingControlled);
            }

        } catch (InterruptedException e) {
            log.error("Interrupt during processing : " + e.getMessage(), e);
        }
    }

    private void processMessage(String message, Device deviceBeingControlled) {
//        log.info("Raw message : [{}]", message);

        if (deviceBeingControlled.isNameUnset()) {
            ObjectMapper objectMapper = new ObjectMapper();

            try {
                JsonNode jsonNode = objectMapper.readTree(message);
                JsonNode idNode = jsonNode.get("stationId");

                if (idNode != null) {
                    deviceBeingControlled.setName(idNode.asText());
                    deviceBeingControlled.setNameUnset(false);
                }
            } catch (JsonProcessingException e) {
                log.error("JsonProcessingException : " + e.getMessage());
            }
        }

        log.info("Msg from device {}:{}", deviceBeingControlled.getName(), deviceBeingControlled.getConnection());
        log.info("{}", message);
        log.info("");
    }
}
