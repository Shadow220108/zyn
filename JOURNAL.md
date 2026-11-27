<div align="center">
  <h1>zyn</h1>
  <h3>By Shadow</h3>
  <h3>Total Time Spent : 43 Hours</h3>
</div>

---

### Journal #1 Starting with the brain of the board
Time spent : 2 hours   

First I needed to pick the MCU. I wanted wifi and bluetooth on one chip, so I went through the ESP32-C3-WROOM-02 datasheet. The cover had what I needed: 4 MB of flash, a PCB antenna so I didn't need an external one, bluetooth 5 and 15 GPIOs :D

<div align="center">

| zyn |
| --- |
| <img width="600" alt="esp32-c3-datasheet-cover" src="images/esp32-c3-datasheet-cover.png" /> |

</div>

---

### Journal #2 Reading the pin layout
Time spent : 2 hours   

I kept the pin layout page open the whole time. There are 18 pads, with 3V3 and EN first and then the IO pins. Some of the GPIOs are strapping pins, so their level at boot matters and I had to be careful with those. After I took out the USB pins and the two buttons, I wrote down which pins were left for me to use.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="esp32-c3-pin-layout" src="images/esp32-c3-pin-layout.png" /> |

</div>

---

### Journal #3 Checking the mosfets for the matrix
Time spent : 2 hours   

The matrix is 128 pixels, so the current adds up when they're all white. I looked at the CSD17302Q5A (30V n-channel) and its transient thermal curve, which showed me a part can take more current in short pulses. I didn't end up using it because the schematic uses an AO3401A for the matrix feed, but it made me think about the feed current before I picked the bulk caps.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="csd17302-mosfet-datasheet" src="images/csd17302-mosfet-datasheet.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="mosfet-thermal-curve" src="images/mosfet-thermal-curve.png" /> |

</div>

---

### Journal #4 The lipo charger
Time spent : 1.5 hours   

The first real sheet was the battery charger. I used an MCP73831 with PROG setting 500 mA, and CHG_STAT drives the red LED (D2). The battery goes in on a JST PH connector (J2).

<div align="center">

| zyn |
| --- |
| <img width="600" alt="lipo-charger" src="images/lipo-charger.png" /> |

</div>

---

### Journal #5 VLED switch
Time spent : 1.5 hours   

The matrix gets its own rail (VLED) so I can cut it when the board is idle. Q3 is a 2N7002 driven from LED_EN on the MCU. It pulls the gate of Q2 (AO3401A) low through LED_DRV, which connects VSYS to VLED. R8 (100k) keeps Q2 off when Q3 is off. I think I used the 2N7002 because the MCU's 3.3V can't switch the p-channel off VSYS cleanly, but I still want to double check that.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="vled-high-side-switch" src="images/vled-high-side-switch.png" /> |

</div>

---

### Journal #6 Boot, reset and buttons
Time spent : 1 hour   

Boot is SW3 (BTN_A), the user button is SW4 (BTN_B) and reset is SW2. The blue status LED (D3) is on IO8. I also added a few test pads on the sheet so I can probe the rails.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="reset-boot-buttons" src="images/reset-boot-buttons.png" /> |

</div>

---

### Journal #7 Wiring up the MCU
Time spent : 1.5 hours   

This is where I wired every MCU pin. Once the sheet was done the netlist gave me the pin numbers, so I checked them against my notes:

pin 1 3V3, pin 2 EN, pin 3 LED_EN, pin 4 MIC_WS, pin 5 MIC_SCK, pin 6 MIC_SD, pin 7 IO8_LED, pin 8 BTN_A, pin 9 GND, pin 10 LED_DATA, pin 11 BTN_B, pin 12 TXD, pin 13 USB_D-, pin 14 USB_D+, pin 15 CC2_SENSE, pin 16 IO2, pin 17 CC1_SENSE, pin 18 VBAT_SENSE, pin 19 GND

The mic (ICS-43434) sits on the same three I2S lines (WS, SCK, SD) on pins 4 to 6.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="esp32-c3-symbol" src="images/esp32-c3-symbol.png" /> |

</div>

---

### Journal #8 USB-C connector
Time spent : 1.5 hours   

USB-C was the connector with the most pins to mess up. I used a HRO TYPE-C-31-M-12. CC1 and CC2 each get a 5.1k pulldown (R1, R2) so the board shows up as a sink.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="usbc-receptacle" src="images/usbc-receptacle.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="usbc-dp-resistors" src="images/usbc-dp-resistors.png" /> |

</div>

---

### Journal #9 USB-C sense and ESD
Time spent : 1.5 hours   

Then I added 10k resistors (R3, R4) to take each CC line to CC1_SENSE and CC2_SENSE, so the MCU can tell which way the plug is in. VBUS_RAW goes through F1, a 2A polyfuse in 1812 size. D+ and D- go through a USBLC6-2SC6 (U6) for ESD before they reach the MCU. I put a PWR_FLAG on VBUS so ERC doesn't complain.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="usbc-cc-pulldowns" src="images/usbc-cc-pulldowns.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="usbc-esd" src="images/usbc-esd.png" /> |

</div>

---

### Journal #10 Level shifter for the pixels
Time spent : 1.5 hours   

The pixels run from VLED, which is higher than the 3.3V logic from the MCU. I put an SN74LV1T125 (U5) on the data line, powered from VLED, so the output is at the level the pixels need. R20 (33R) is on the output and R21 (100k) pulls the input low during reset. The bulk caps C13/C14 (47u) and C15/C16 (22u) sit right at the matrix feed.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="vled-bulk-caps" src="images/vled-bulk-caps.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="data-level-shifter" src="images/data-level-shifter.png" /> |

</div>

---

### Journal #11 Decoupling every pixel
Time spent : 1 hour   

Every SK6812 mini-e pixel gets its own 100n X7R 0402 cap. The BOM has 132 of them, and I split the sheet over three pages so it's easier to read.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-decoupling-a" src="images/pixel-decoupling-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-decoupling-b" src="images/pixel-decoupling-b.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-decoupling-c" src="images/pixel-decoupling-c.png" /> |

</div>

---

### Journal #12 Checking the whole schematic
Time spent : 1 hour   

I went through the overview pages once more and checked that the labels match between the sheets.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="schematic-overview-a" src="images/schematic-overview-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="schematic-overview-b" src="images/schematic-overview-b.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="schematic-overview-c" src="images/schematic-overview-c.png" /> |

</div>

---

### Journal #13 Placing the matrix first
Time spent : 2 hours   

The board is 127 x 92 mm on four layers. All 128 pixels sit on the bottom in a 16 by 8 grid. I placed the matrix first and fit everything else around it.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="placement-overview" src="images/placement-overview.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="led-first-row" src="images/led-first-row.png" /> |

</div>

---

### Journal #14 Placing the power parts
Time spent : 1.5 hours   

The charger, LDO, power switch and battery connector went along the bottom edge near where the battery plugs in. USB-C is on the left edge so the cable doesn't cross the board.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="placement-power-strip" src="images/placement-power-strip.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="placement-usb-end" src="images/placement-usb-end.png" /> |

</div>

---

### Journal #15 Routing the battery path
Time spent : 1.5 hours   

I routed VBAT from the connector to the charger and then to the power switch, and I made it wider than the signal lines. The VBAT sense line branches off the same point.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="power-zoom-a" src="images/power-zoom-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="power-vbat-routing" src="images/power-vbat-routing.png" /> |

</div>

---

### Journal #16 Routing VBUS and VSYS
Time spent : 1.5 hours   

VBUS and VSYS carry the real current, so they got the widest traces on the board. They come from the fuse and the power switch.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="power-vbus-routing" src="images/power-vbus-routing.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="vsys-routing" src="images/vsys-routing.png" /> |

</div>

---

### Journal #17 Wide traces for the matrix
Time spent : 1.5 hours   

The VLED feed to the matrix has to be wide because every pixel draws from it. I put the level shifter close to the matrix input so the data line is short before the first pixel.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="power-routing-wide" src="images/power-routing-wide.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="level-shifter-layout" src="images/level-shifter-layout.png" /> |

</div>

---

### Journal #18 Routing the USB pair
Time spent : 1.5 hours   

I routed D+ and D- as a pair with matched lengths. The USBLC6 sits by the connector and the CC resistors sit near their pins.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="usbc-routing" src="images/usbc-routing.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="routing-overview" src="images/routing-overview.png" /> |

</div>

---

### Journal #19 IO and sense lines
Time spent : 1.5 hours   

The IO lines from the MCU (IO2, IO8, the buttons and the sense lines) go out to the sides of the board. The two CC sense lines run next to each other, so I checked their spacing in the rules.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="io-sense-routing" src="images/io-sense-routing.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="io-routing-overview" src="images/io-routing-overview.png" /> |

</div>

---

### Journal #20 The pixel chain, left side
Time spent : 1.5 hours   

The data chain goes DIN to DOUT through every pixel in order, so I routed it along the grid. The caps sit beside each pixel, so the traces have to go around them.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="routing-left" src="images/routing-left.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="routing-middle" src="images/routing-middle.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="routing-right" src="images/routing-right.png" /> |

</div>

---

### Journal #21 The pixel chain, middle
Time spent : 1.5 hours   

The middle of the chain is where it gets tight. I checked each pixel against its neighbours so no trace touches another pad.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-chain-routing" src="images/pixel-chain-routing.png" /> |

</div>

---

### Journal #22 DRC, three unrouted nets
Time spent : 1.5 hours   

My first DRC had 167 nets with 3 unrouted. After rerouting, it's 169 nets with 0 unrouted. Vias went from 447 to 449 and track segments from 1293 to 1294.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="drc-3-unrouted" src="images/drc-3-unrouted.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="drc-0-unrouted" src="images/drc-0-unrouted.png" /> |

</div>

---

### Journal #23 Full board check
Time spent : 1 hour   

With DRC clean I looked at the full board. The final stats are 989 pads, 449 vias, 1294 track segments, 169 nets and 0 unrouted.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="full-board-view" src="images/full-board-view.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="layer-orange-view" src="images/layer-orange-view.png" /> |

</div>

---

### Journal #24 Pixel rows
Time spent : 1 hour   

I zoomed in on the pixel rows to check the chain row by row, because DRC doesn't catch everything.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-chain-row-a" src="images/pixel-chain-row-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-chain-row-b" src="images/pixel-chain-row-b.png" /> |

</div>

---

### Journal #25 Checking each copper layer
Time spent : 2.5 hours   

I checked each copper layer on its own: the LED matrix, the pixel details, the J4 header at the edge, and then front, inner 1, inner 2 and back.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="led-matrix-dense" src="images/led-matrix-dense.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-detail-a" src="images/pixel-detail-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="pixel-detail-b" src="images/pixel-detail-b.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="j4-header" src="images/j4-header.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="matrix-full" src="images/matrix-full.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="layer-front" src="images/layer-front.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="layer-in1" src="images/layer-in1.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="layer-in2" src="images/layer-in2.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="layer-back" src="images/layer-back.png" /> |

</div>

---

### Journal #26 Silkscreen art and the name
Time spent : 2 hours   

I added the silkscreen art and the name. I checked the logo on its own layer so I could see it against the copper.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="logo-render" src="images/logo-render.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="made-by-shadow-text" src="images/made-by-shadow-text.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="silkscreen-art" src="images/silkscreen-art.png" /> |

</div>

---

### Journal #27 3D renders
Time spent : 2 hours   

I rendered the board in 3D from a few angles. The blue one shows the back with the traces, and the full board one is the one I'd put at the top of the repo.

<div align="center">

| zyn |
| --- |
| <img width="600" alt="render-angle-a" src="images/render-angle-a.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="render-angle-b" src="images/render-angle-b.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="render-angle-c" src="images/render-angle-c.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="board-blue-view" src="images/board-blue-view.png" /> |

</div>

<div align="center">

| zyn |
| --- |
| <img width="600" alt="full-board-final" src="images/full-board-final.png" /> |

</div>

---


### Journal #28 Making the poster card
Time spent : 2 hours   

Once the board, the journal and the README were done, I made a poster style card to show the board off. I used the front render as the main picture, put the name ZYN above it and wrote a one line description under the board. I also added a footer bar at the bottom, so it looks like something I'd post with the project.

<div align="center">

| Poster card |
| --- |
| <img width="1080" height="1920" alt="zyn" src="https://github.com/user-attachments/assets/08128611-b540-49fa-9411-057d09848d66" /> |

</div>

---
