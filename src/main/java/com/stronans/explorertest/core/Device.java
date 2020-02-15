package com.stronans.explorertest.core;

import lombok.Data;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;

@Data
public class Device {
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(Device.class);

    private static int DEFAULT_CONNECTION_SPEED = 115200;
    private static final String USB_CONNECTOR = "/dev/ttyUSB";

    private String name = "Device-";
    private boolean nameUnset = true;
    private SerialComms comms;
    private String connection;
    private int connectionId = -1;
    private int speed;

    public Device(String name, int speed) {
        if (name != null) {
            this.name = name;
        }
        this.speed = speed;
    }

    public Device(String name) {
        this(name, DEFAULT_CONNECTION_SPEED);
    }

    public Device() {
        this(null, DEFAULT_CONNECTION_SPEED);
    }

    public void setConnectionId(int connectionId) {
        this.connectionId = connectionId;
        this.connection = USB_CONNECTOR + connectionId;
        this.name += connectionId;
    }

    /**
     * Must have called setConnectionId before this is called.
     *
     * @return true is connection was a success otherwise false;
     * <p>
     * Throws UnsupportedOperationException if setCconnectionId has not been called before calling this method.
     */
    public boolean openConnection(MessageListener listener) throws InterruptedException, UnsupportedOperationException, IOException {
        if (connectionId == -1) {
            throw new UnsupportedOperationException("Must set connection Id before calling this method.");
        }

        boolean result = true;

        comms = new SerialComms(connection, speed);
        comms.addListener(listener);
        comms.startComms();

        return result;
    }
}
