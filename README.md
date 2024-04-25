# MQTT-SN Implementation in OMNeT++

## Project Overview
This repository presents an MQTT-SN implementation using the discrete event simulator OMNeT++. It adheres to the protocol standards defined in the specifications, with simplifications for practicality and focus. The primary aim of the project is academic, serving as the completion of the final MSc thesis. For more detailed information, access the [documentation](https://github.com/taygumus/thesis).

## Installation
1. **Install OMNeT++**: Download and install OMNeT++ from [OMNeT++](https://omnetpp.org/).

2. **Install INET Framework**: Download and integrate the INET framework into OMNeT++. Learn more at [INET](https://inet.omnetpp.org/).

3. **Clone the Repository**: Clone this repository using: `git clone https://github.com/taygumus/mqtt-sn.git`.

## Usage
To run simulations:

1. Open the OMNeT++ IDE.

2. Either use the existing simulation model or make edits to the `simulations/WifiNetwork.ned` file as required.

3. Use the existing parameters or modify them in the `simulation/omnetpp.ini` file as needed.

4. Run the simulation.

5. Results can be found in the CSV file within the `simulation/results` directory.