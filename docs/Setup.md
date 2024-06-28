# Setup guide

## Prerequisites

- Quartus Prime Lite v20.1.1
- Eclipse IDE Mars.2 Release (4.5.2)
- Eclipse Nios II Embedded Design Suite v20.1.0

## Steps (Using Command Line)

### FPGA Setup

- Clone this repository using `git clone https://github.com/Diegovano/pathfinder.git` in the desired folder.
- Using Quartus Prime Lite, click "Open Project", under "File", then navigate to `quartus/pathfinder.qpf`.
- In "Project Navigator", switch from "Hierarchy" to "Files".
- From Project Navigator, open `nios2_system.qsys`, this opens Platform Designer.
- In Platform Designer, click "Generate HDL", then "Generate".
- In Quartus, under "Processing", click "Start Compilation".
- Now, plug in the FPGA into power and connect it to the Host computer.
- Under "Tools", open "Programmer", then click Hardware Setup, and select your device. In our case it is DE-SoC.
- Click "Auto-Detect", then select "5CSEMA5" or as appropriate.#
- Select the newly added device, then click "Change File".
- Select `pathfinder_time_limited.sof`.
- If required, check "Program / Configure", then hit start. The FPGA should now be programmed.
- In the Nios II Shell, navigate to `software/pathfinder`, run `./genbsp.sh`.
- Finally, run `make download-elf`, which will flash the FPGA with the required software. The inital compilation may take a little long, but this is only the case for the first compilation.
- Now, run `nios2-terminal` to see the output of the FPGA. Depending on the flags set in the code, varying debug messages may or may not be output.
- You have succeeded if the terminal prints "Starting Pathfinder! IRQ register return 0. IDLE:".

### ESP Setup
- Connect the ESP32 to the FPGA described [here](./Communication.md#connecting-to-esp32-to-fpga).
- Then, flash the ESP32 with `software/esp/Pathfinder/src/main.cpp`, using your preferred method. We used PlatformIO for this task.
- Activate a Hotspot on your PC in the 2.4 GHz Band.
- The ESP will connect to the network specified in the `main.cpp` file, edit the SSID and Password fields to match that of the hotspot. In our testing, we hosted the website from the same device which was acting as the hotspot.

### Web Server Setup

- See [this guide](./Website.md#prerequisites).
