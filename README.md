# Mini Shell

### **Overview**
This project implements a **Mini Shell**, a command-line interface (CLI) inspired by the BASH shell. It allows users to execute commands, supports piping, handles errors, and processes special keyboard actions like `Ctrl+C`. Designed using **Linux Kernel system calls** and **IPC mechanisms** such as signals, the Mini Shell is extendable for advanced functionalities like command history.

### **Features**
- Command execution and output generation.
- Support for piping and variable usage.
- Handles special keyboard actions like `Ctrl+C`.
- Error handling during tokenization and execution.
- Extendable for advanced features like command history.

### **How It Works**
1. **Input Processing**: Accepts commands from the user via the CLI.
2. **System Call Integration**: Uses Linux Kernel system calls for command execution.
3. **Signal Handling**: Employs IPC mechanisms to handle interrupts and special actions.
4. **Output Generation**: Provides the required output or error messages based on the user's input.

### **Prerequisites**
To work on this project, knowledge of the following concepts is recommended:
- **Linux Kernel System Calls Usage**: Understanding how to interact with the operating system at a low level.
- **IPC (Signal Handling)**: Manage communication between processes using signals.
- **String Pointers and Parsing**: Process and analyze command inputs effectively.

### **Getting Started**
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/Mini-Shell.git
   cd Mini-Shell
