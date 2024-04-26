# MQTT-SN Implementation in OMNeT++

## Project Overview
This repository presents an MQTT-SN implementation using the discrete event simulator OMNeT++. The development is mostly compliant with the protocol standards. See details at the [MQTT-SN specifications](https://groups.oasis-open.org/higherlogic/ws/public/document?document_id=66091). The primary aim of the work is academic, serving as the completion of my final MSc thesis. For more information, please access the related [documentation](https://github.com/taygumus/thesis).

## Installation
1. **Install OMNeT++**: Download and install OMNeT++ from [OMNeT++](https://omnetpp.org/).

2. **Install INET Framework**: Download and integrate the INET framework into OMNeT++. Learn more at [INET](https://inet.omnetpp.org/).

3. **Clone the Repository**: Clone this repository using: `git clone https://github.com/taygumus/mqtt-sn.git`.

## Usage
To run simulations:

1. Open the OMNeT++ IDE.

2. Either use the existing simulation model or make edits to the `simulations/WifiNetwork.ned` file as required.

3. Use the existing parameters or modify them in the `simulations/omnetpp.ini` file as needed.

4. Run the simulation.

5. Results can be found within the `simulations/results` directory.

## Contributing
There are certainly opportunities for refinement and enhancement, particularly in terms of method refactorization, overall performance improvements and addressing a few omitted minor functionalities. The project meets the academic goals for the final thesis. Contributions are warmly welcomed and your input would be highly appreciated.