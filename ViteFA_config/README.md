# ViteFA_config: Configuration Server and Agent System

`ViteFA_config` is a highly efficient configuration management system implemented in **C**. It is designed to handle over 100,000 concurrent agent connections with sub-second propagation of configuration changes. The system uses **MySQL** for storage and relies on **libshmcache** for shared memory management. The shared memory type is configured to **mmap**, and `recycle_valid_entries` must be set to **false** for the cache.

## Project Overview

The ViteFA_config project is structured into three main components:

1. **Config Center Cluster**: This is where configuration data is stored and managed. It consists of multiple **config servers** that interact with the **environment & config storage**, which uses MySQL. The servers have a peer-to-peer relationship, ensuring reliability and high availability.
   
2. **Admin Tool**: The **admin tool** interacts with the config servers via **API calls** to manage configurations and environment variables. This acts as a control panel for the system.

3. **Config Agent**: Installed on the **server host**, it interacts with the shared memory, which is used to store and retrieve configuration data. The config agent receives configuration changes from the config servers and writes them to shared memory. Multiple processes on the server host can then read the updated configurations.

## Architectural Diagram

![architect](images/architect.png)

## Key Components

### Config Center Cluster
- **Config Servers**: Multiple config servers work in a **peer-to-peer** manner to maintain consistency across the environment. These servers are responsible for pushing configuration updates to the connected agents. The servers use **MySQL** as their storage backend for environmental configurations and settings.
- **Storage**: Config data is stored in MySQL, and the servers synchronize this data to ensure consistency.

### Admin Tool
- The **admin tool** interacts with the config servers via **API calls**. It allows for:
  - Managing environment configurations.
  - Monitoring and controlling configuration changes.
  - Initiating config push requests to agents.

### Server Host
- **Config Agent**: Deployed on each **Server Host**. It listens for configuration changes from the config servers and updates the shared memory when changes occur.
- **Shared Memory**: **libshmcache** is used for managing shared memory. The shared memory type is set to **mmap**, ensuring that the memory is mapped to files. Each process on the server reads from this shared memory, which guarantees that they are using the latest configuration values. Configuring `recycle_valid_entries` to **false** ensures that the cache does not recycle valid entries, maintaining consistency.
- **Processes**: Various processes running on the server host read the configurations from shared memory. This allows for real-time updates and ensures that all processes operate with the latest settings.

## Code Structure

Here’s a breakdown of the **code structure** and the roles of different components in the project:

### Admin (tools)
- **`fcfg_admin_config.c`**: Manages the configuration logic for the admin tool.
- **`fcfg_admin_func.c`**: Contains functions to handle API interactions.
- **`fcfg_admin_launch.c`**: The entry point to start the admin tool.

### Agent
- **`fcfg_agent_func.c`**: Contains the main functions for the agent, responsible for communication with config servers.
- **`fcfg_agent_global.c`**: Defines global variables and configurations used by the agent.
- **`fcfg_agent_handler.c`**: Manages the handling of incoming configuration data from the config server and updates the shared memory accordingly.
- **`fcfg_agent_types.h`**: Defines the data types and structures used by the agent.

### Server
- **`fcfg_server_cfg.c`**: Manages the configuration logic for the config server, handling data retrieval and updates to MySQL.
- **`fcfg_server_push.c`**: Handles pushing configuration changes to agents.
- **`fcfg_server_env.c`**: Manages the environment configurations and ensures they are synced across all servers.

### Common
- **`fcfg_global.c`**: Contains shared global settings used across both the server and agent.
- **`fcfg_types.h`**: Defines shared types used across various parts of the system, ensuring compatibility between the different components.

## How It Works

1. **Configuration Management**: Configuration data is stored in MySQL in the Config Center Cluster. The Admin Tool allows an administrator to push configuration updates through API calls.
   
2. **Pushing Configurations**: Once a change is made, the Config Servers ensure that the updated configuration is pushed to all agents connected to them.

3. **Agent Updates**: The agents receive the new configuration data and write it to the shared memory using **libshmcache**. The memory type is set to **mmap** to map it to a file, allowing for persistence and consistency.

4. **Process Reads**: On each server host, multiple processes read the latest configurations directly from shared memory. This ensures that all processes are working with up-to-date settings.

## Shared Memory Configuration

The system uses **libshmcache** for managing shared memory between processes. You need to ensure that the shared memory type is set to **mmap** and `recycle_valid_entries` is set to **false**. This ensures that valid entries are not overwritten, maintaining the integrity of configuration data across processes.

## Additional Notes

- **Scalability**: The system is designed to handle a large number of concurrent agents, ensuring fast propagation of configuration changes.
- **Consistency**: Config servers operate in a peer-to-peer relationship, ensuring that changes are synchronized across all servers and propagated to agents in near real-time.
- **Persistence**: With the use of shared memory, processes on each server can immediately access the latest configuration without querying external services, improving performance and reliability.

## Future Enhancements

- Support for dynamic scaling of configuration agents.
- Integration with additional storage backends beyond MySQL.
- Enhanced security measures for API calls between the admin tool and config servers.


## Usage 

See [Install](./INSTALL) for usage instructions.

