package com.stronans.explorertest.device;

import lombok.Data;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.Optional;

@Data
public class Device {
    private static final String USB_CONNECTOR = "/dev/ttyUSB";
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(Device.class);
    private static int DEFAULT_CONNECTION_SPEED = 115200;
    private static String DEFAULT_NAME = "Device";
    private static int NOT_SET = -32767;
    private String name;
    private boolean nameUnset = true;
    private SerialComms comms;
    private String connection;
    private int connectionId = NOT_SET;
    private int speed = NOT_SET;

    public Device(String name, int speed, int connectionId) {
        this.setName(name);
        this.setSpeed(speed);
        this.setConnectionId(connectionId);
    }

    public Device(String name) {
        this(name, DEFAULT_CONNECTION_SPEED, NOT_SET);
    }

    public Device(String name, int connectionId) {
        this(name, DEFAULT_CONNECTION_SPEED, connectionId);
    }

    public Device() {
        this(null, NOT_SET, NOT_SET);
    }

    public String getName() {
        if (this.name == null) {
            this.name = DEFAULT_NAME;
        }
        return this.name;
    }

    public void setName(final String name) {
        if (this.name != null) {
            throw new UnsupportedOperationException("Cannot reset name once set");
        }
        this.name = name;
    }

    public void setSpeed(final int speed) {
        if (this.speed != NOT_SET) {
            throw new UnsupportedOperationException("Cannot reset speed once set");
        }

        this.speed = Optional.of(speed).filter(s -> s == NOT_SET || s > 0)
                .orElseThrow(() -> new UnsupportedOperationException("Speed must be greater than zero and a value that is supported by UART"));
    }

    public void setConnectionId(final int connectionId) {
        if (this.connectionId != NOT_SET) {
            throw new UnsupportedOperationException("Cannot reset ConnectionId once set");
        }

        if(connectionId != NOT_SET) {
            Optional.of(connectionId)
                    .filter(c -> c > 0)
                    .map(co -> {
                        this.connectionId = co;
                        this.connection = USB_CONNECTOR + co;

                        this.name = getName() + "-" + co;
                        return co;
                    })
                    .orElseThrow(() -> new
                            UnsupportedOperationException(
                                    "Must set connection Id to a value greater than zero [" + connectionId + "]."));
        }
    }

    /**
     * Must have called setConnectionId before this is called.
     *
     * @return true is connection was a success otherwise false;
     * <p>
     * Throws UnsupportedOperationException if setCconnectionId has not been called before calling this method.
     */
    public boolean openConnection(MessageListener listener) throws InterruptedException, UnsupportedOperationException, IOException {
        if (connectionId == NOT_SET ) {
            throw new UnsupportedOperationException("Must set connection Id before calling this method.");
        }

        boolean result = true;

        comms = new SerialComms(connection, speed);
        comms.addListener(listener);
        comms.startComms();

        return result;
    }
}
