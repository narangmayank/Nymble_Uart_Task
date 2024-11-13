| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Nymble UART Task

This project is based on uart_echo sample project of the esp idf sdk.

Note: I've done this task using UART0 which is connected to serial terminal of ESP32. A working demo video is [here](./video/Uart_bitrate.mp4). You can change the UART port in the menuconfig thats not a problem. For more instructions see below,

### Hardware Required

The example can be run on any development board, that is based on the Espressif SoC. The board shall be connected to a computer with a single USB cable for flashing and monitoring. The external interface should have 3.3V outputs. You may
use e.g. 3.3V compatible USB-to-Serial dongle.

### Setup the Hardware

Connect the external serial interface to the board as follows.

```
  -----------------------------------------------------------------------------------------
  | Target chip Interface | Kconfig Option     | Default ESP Pin      | External UART Pin |
  | ----------------------|--------------------|----------------------|--------------------
  | Transmit Data (TxD)   | EXAMPLE_UART_TXD   | GPIO4                | RxD               |
  | Receive Data (RxD)    | EXAMPLE_UART_RXD   | GPIO5                | TxD               |
  | Ground                | n/a                | GND                  | GND               |
  -----------------------------------------------------------------------------------------
```
Note: Some GPIOs can not be used with certain chips because they are reserved for internal use. Please refer to UART documentation for selected target.

Optionally, you can set-up and use a serial interface that has RTS and CTS signals in order to verify that the
hardware control flow works. Connect the extra signals according to the following table, configure both extra pins in
the example code `uart_echo_example_main.c` by replacing existing `UART_PIN_NO_CHANGE` macros with the appropriate pin
numbers and configure UART1 driver to use the hardware flow control by setting `.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS`
and adding `.rx_flow_ctrl_thresh = 122` to the `uart_config` structure.

```
  ---------------------------------------------------------------
  | Target chip Interface | Macro           | External UART Pin |
  | ----------------------|-----------------|--------------------
  | Transmit Data (TxD)   | ECHO_TEST_RTS   | CTS               |
  | Receive Data (RxD)    | ECHO_TEST_CTS   | RTS               |
  | Ground                | n/a             | GND               |
  ---------------------------------------------------------------
```

### Configure the project

Use the command below to configure project using Kconfig menu as showed in the table above.
The default Kconfig values can be changed such as: EXAMPLE_TASK_STACK_SIZE, EXAMPLE_UART_BAUD_RATE, EXAMPLE_UART_PORT_NUM (Refer to Kconfig file).
```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


### Running PC Code

From the root just hit the below command,

```
python script/main.py COM 2400
```

(To exit the python terminal, type ``Ctrl-C``.)
