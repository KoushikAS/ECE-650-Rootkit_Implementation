# Project #5: Rootkit Implementation
ECE 650 – Spring 2023

## Overview
This project focuses on implementing a portion of a Rootkit to gain practical experience in kernel programming, understanding system calls within the kernel, and executing malicious activities against a system. The project involved developing a kernel module and an attack program that discreetly performs actions on a Linux system running Ubuntu 20.04.

## Project Structure
- `sneaky_mod.c`: Source code for the sneaky kernel module.
- `sneaky_process.c`: Source code for the sneaky attack program.
- `Makefile`: Compilation instructions for the sneaky process and kernel module.

## Features
- **Kernel Module (`sneaky_mod.c`):**
  - Conceals the `sneaky_process` executable from commands like `ls` and `find`.
  - Hides the `/proc/<sneaky_process_id>` directory.
  - Masks changes made to `/etc/passwd` by the sneaky process.
  - Removes the module's entry from `/proc/modules`.

- **Attack Program (`sneaky_process.c`):**
  - Prints the process ID of the sneaky process.
  - Copies `/etc/passwd` to `/tmp/passwd` and appends a new line for user authentication.
  - Loads the sneaky kernel module, passing its process ID as an argument.
  - Waits for a user input ('q') to proceed with the next steps.
  - Unloads the sneaky kernel module.
  - Restores the original `/etc/passwd` file.

## Compilation and Execution
- Compile the project using the provided `Makefile`:
  ```bash
   make all
  ```

- Run the sneaky process with root privileges:
  ```bash
   sudo ./sneaky_process
  ```


## Working with the Virtual Machine
- Tested on a Linux VM (Ubuntu 20.04) hosted on Duke's VM service.
- Kernel version: Linux 5.4.0-104-generic.
- Development environment set up using `sudo apt-get install`.

## Safety Measures
- Implemented on a dedicated VM to prevent system damage.
- Utilized VM snapshots for easy recovery from potential issues.

## Submission
- Project submission includes `sneaky_mod.c`, `sneaky_process.c`, and `Makefile`.
- Packaged as a zip file named `proj5_netid.zip` for submission.

## Additional Notes
- The project provided insights into how rootkits can manipulate system-level operations.
- Emphasized the importance of understanding kernel-level programming for cybersecurity.


## Contributions

This project was completed as part of an academic assignment with requirments provided requirments.pdf. Contributions were made solely by Koushik Annareddy Sreenath, adhering to the project guidelines and requirements set by the course ECE-650 Systems Program & Engineering

## License

This project is an academic assignment and is subject to university guidelines on academic integrity and software use.

## Acknowledgments

- Thanks to Rabih Younes and the course staff for providing guidance and support throughout the project.

