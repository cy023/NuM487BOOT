# NuM487BOOT

A serial and secure programming tool for SAME54 series microcontroller.

## Boot Scheme

![boot_scheme](./Img/boot_scheme.png)

## Boot Process

![boot_process](./Img/boot_process.png)

## Software Satck

## Memory Layout

## Communication handshake

## Communication command

## Flowchart

![flowchart](./Img/flowchart.png)

## Bootloader packet format
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     HEADER                    |    COMMAND    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            LENGTH             |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
|                                                               |
+                                                               +
|                     DATA (variable length)                    |
+                                               +-+-+-+-+-+-+-+-+
|                                               |    CHECKSUM   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

- HEADER   : 0xA5A5A5
- COMMAND  : See details in the previous intruduction
- LENGTH   : Data length
- DATA     : Variable length
- CHECKSUM : sum(data) % 255

## Bootloader data format

## HOST Tool

ref: <https://github.com/cy023/SerProg>
