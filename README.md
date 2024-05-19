# MQTT-SN Implementation in OMNeT++

## Project Overview
This repository presents an MQTT-SN implementation using the discrete event simulator OMNeT++. The development is mostly compliant with the protocol standards. See details in the [MQTT-SN specification](https://groups.oasis-open.org/higherlogic/ws/public/document?document_id=66091).  
The primary aim of the work is academic, serving as the completion of my final MSc thesis. For more information, please access the [related documentation](https://github.com/taygumus/thesis).

## Installation
1. **Install OMNeT++**: Download and install OMNeT++ from [OMNeT++](https://omnetpp.org/).

2. **Install INET Framework**: Download and integrate the INET framework into OMNeT++. Learn more about [INET](https://inet.omnetpp.org/).

3. **Clone the Repository**: Clone this repository using:
   ```bash
   git clone https://github.com/taygumus/mqtt-sn.git
   ```

4. **Include JSON library**: Download the `json.hpp` file from the [nlohmann JSON repository](https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp) and place it in the `src/externals/nlohmann` directory.

## Usage
To run simulations:

1. Open the OMNeT++ IDE.

2. Either use the existing simulation model or make edits to the `simulations/WifiNetwork.ned` file as required.

3. Use the existing parameters or modify them in the `simulations/omnetpp.ini` file as needed.

4. Run the simulation.

5. Results can be found within the `simulations/results` directory.

## Contributing
There are certainly opportunities for refinement and enhancement, particularly in terms of addressing a few minor omitted functionalities, some method refactoring and overall performance improvement. The project meets the academic goals for the final thesis. Contributions are warmly welcomed and your input would be highly appreciated.

## Credits and Licenses
- **OMNeT++**: Licensed under its own [Academic Public License](https://omnetpp.org/intro/license).

- **INET Framework**: GNU Lesser General Public License. For details, see [INET License repository](https://github.com/inet-framework/inet/blob/master/LICENSE.md).

- **JSON for Modern C++ by Niels Lohmann**: Used under the MIT License. The license details can be viewed in the [nlohmann JSON MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).