package com.stronans.explorertest;

import com.stronans.explorertest.core.Controller;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Application {
    /**
     * The <code>Logger</code> to be used.
     */
    private static Logger log = LogManager.getLogger(Application.class);

    public static void main(String[] args) {
        log.info("Starting the Explorer Test..");

        Controller controller = new Controller();

        controller.run();
    }
}
