package com.stronans.explorertest.core;

import com.stronans.explorertest.device.Device;
import org.junit.Before;
import org.junit.Test;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.MatcherAssert.assertThat;


public class DeviceTest {
    Device underTest;

    @Before()
    public void before() {
        underTest = new Device();
    }

    @Test
    public void TestGetName_success() {
        String name = underTest.getName();

        assertThat(name, is("Device"));
    }

    @Test
    public void setName_success() {
        underTest.setName("Simon");
        String name = underTest.getName();

        assertThat(name, is("Simon"));
    }

    @Test(expected = UnsupportedOperationException.class)
    public void setSpeed_success() {
        underTest.setSpeed(115200);
        int speed = underTest.getSpeed();

        assertThat(speed, is(115200));

        underTest.setSpeed(19200);
    }

    @Test
    public void setConnectionId_success() {
    }
}