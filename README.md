# Remote-Cmd: A Lightweight Client-Server Application

![Remote-Cmd Logo](https://img.shields.io/badge/Remote--Cmd-v1.0-blue.svg)  
[![Releases](https://img.shields.io/badge/releases-latest-green.svg)](https://github.com/ihesti/server-and-client/releases)

---

## Overview

**Remote-Cmd** is a lightweight client-server application designed for executing remote shell commands securely. Built in C, it offers a robust solution for system administrators and developers who need to manage servers efficiently. With features like authentication, syslog logging, optional system updates, and support for both IPv4 and IPv6, Remote-Cmd is tailored for Linux Debian environments and requires no external dependencies.

## Table of Contents

1. [Features](#features)
2. [Installation](#installation)
3. [Usage](#usage)
4. [Configuration](#configuration)
5. [Contributing](#contributing)
6. [License](#license)
7. [Support](#support)
8. [Acknowledgments](#acknowledgments)

## Features

- **Secure Execution**: Execute commands securely with built-in authentication.
- **Logging**: Log activities using syslog for easy monitoring.
- **System Updates**: Optionally update the system through the client.
- **IPv4/IPv6 Support**: Use the application in diverse network environments.
- **Dry-Run Mode**: Test commands without making changes to the system.
- **Lightweight**: Minimal resource usage, making it ideal for various server configurations.

## Installation

To install Remote-Cmd, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/ihesti/server-and-client.git
   ```

2. Navigate to the directory:

   ```bash
   cd server-and-client
   ```

3. Compile the application:

   ```bash
   make
   ```

4. The compiled binaries will be located in the `bin` directory.

For the latest release, you can also download the precompiled binaries from the [Releases section](https://github.com/ihesti/server-and-client/releases). Download the appropriate file for your system and execute it.

## Usage

After installation, you can start using Remote-Cmd. Here’s how:

### Starting the Server

To start the server, run:

```bash
./bin/server
```

### Starting the Client

To connect to the server and execute commands, run:

```bash
./bin/client
```

You will need to provide the server's IP address and your credentials.

### Example Command

Once connected, you can execute commands like:

```bash
execute ls -l
```

This command lists files in the current directory.

## Configuration

Configuration is essential for tailoring Remote-Cmd to your needs. You can modify the configuration file located at `config/config.yaml`. Here are some key settings:

- **Server Port**: Change the port on which the server listens.
- **Log Level**: Adjust the verbosity of logs (info, debug, error).
- **Allowed IPs**: Specify which IPs can connect to the server.

### Example Configuration

```yaml
server:
  port: 8080
  allowed_ips:
    - 192.168.1.1
    - 192.168.1.2
log:
  level: info
```

## Contributing

We welcome contributions to Remote-Cmd! If you have ideas for improvements or new features, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or fix.
3. Make your changes and commit them.
4. Push to your branch and create a pull request.

Your contributions help make Remote-Cmd better for everyone.

## License

Remote-Cmd is open-source software licensed under the MIT License. You can freely use, modify, and distribute it, provided you include the original license in any copies of the software.

## Support

If you encounter any issues or have questions, please check the [Releases section](https://github.com/ihesti/server-and-client/releases) for updates and documentation. You can also open an issue in the repository for help.

## Acknowledgments

We thank all contributors and users who have supported the development of Remote-Cmd. Your feedback and contributions are invaluable.

---

For more information, visit the [Releases section](https://github.com/ihesti/server-and-client/releases) to download the latest version and stay updated on new features and fixes.