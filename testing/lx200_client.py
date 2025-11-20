#!/usr/bin/env python3
"""
******************************************************************************
* @file    lx200_client.py
* @brief   LX200 Protocol Test Client with GUI
* @author  LX200 Proxy Project
* @date    2025
******************************************************************************
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import serial
import threading
import time
from datetime import datetime
import json

class LX200TestClient:
    def __init__(self, root):
        self.root = root
        self.root.title("LX200 Protocol Test Client")
        self.root.geometry("800x700")
        
        # Serial connection
        self.serial_connection = None
        self.is_connected = False
        
        # Test automation
        self.is_running_all = False
        self.test_results = {}
        
        # LX200 Command definitions with expected responses
        self.lx200_commands = {
            # Information Commands
            "Get Right Ascension": {
                "command": ":GR#",
                "expected": "HH:MM:SS#",
                "description": "Get current Right Ascension"
            },
            "Get Declination": {
                "command": ":GD#",
                "expected": "sDD*MM:SS#",
                "description": "Get current Declination"
            },
            "Get Site Name": {
                "command": ":GM#",
                "expected": "Site Name#",
                "description": "Get current site name"
            },
            "Get Site Latitude": {
                "command": ":Gt#",
                "expected": "sDD*MM:SS#",
                "description": "Get site latitude"
            },
            "Get Site Longitude": {
                "command": ":Gg#",
                "expected": "sDDD*MM:SS#",
                "description": "Get site longitude"
            },
            "Get Tracking Rate": {
                "command": ":GT#",
                "expected": "60.1#",
                "description": "Get tracking rate"
            },
            
            # Movement Commands
            "Move North": {
                "command": ":Mn#",
                "expected": "",
                "description": "Start moving North"
            },
            "Move South": {
                "command": ":Ms#",
                "expected": "",
                "description": "Start moving South"
            },
            "Move East": {
                "command": ":Me#",
                "expected": "",
                "description": "Start moving East"
            },
            "Move West": {
                "command": ":Mw#",
                "expected": "",
                "description": "Start moving West"
            },
            "Halt All": {
                "command": ":Q#",
                "expected": "",
                "description": "Stop all movement"
            },
            
            # Guiding Commands (with duration)
            "Guide North 1s": {
                "command": ":Mgn1000#",
                "expected": "",
                "description": "Guide North for 1 second"
            },
            "Guide South 1s": {
                "command": ":Mgs1000#",
                "expected": "",
                "description": "Guide South for 1 second"
            },
            "Guide East 1s": {
                "command": ":Mge1000#",
                "expected": "",
                "description": "Guide East for 1 second"
            },
            "Guide West 1s": {
                "command": ":Mgw1000#",
                "expected": "",
                "description": "Guide West for 1 second"
            },
            
            # Slew Rate Commands
            "Set Slew Rate Fast": {
                "command": ":RS#",
                "expected": "",
                "description": "Set slew rate to fastest"
            },
            "Set Slew Rate Medium": {
                "command": ":RM#",
                "expected": "",
                "description": "Set slew rate to medium"
            },
            "Set Slew Rate Center": {
                "command": ":RC#",
                "expected": "",
                "description": "Set slew rate to centering"
            },
            "Set Slew Rate Guide": {
                "command": ":RG#",
                "expected": "",
                "description": "Set slew rate to guiding"
            },
            
            # Coordinate Setting (examples)
            "Set RA Example": {
                "command": ":Sr 12:34:56#",
                "expected": "1",
                "description": "Set Right Ascension to 12:34:56"
            },
            "Set DEC Example": {
                "command": ":Sd +45:30:00#",
                "expected": "1",
                "description": "Set Declination to +45:30:00"
            },
            
            # Special Commands
            "Sync": {
                "command": ":CM#",
                "expected": "",
                "description": "Sync telescope to current coordinates"
            },
            "Slew to Target": {
                "command": ":MS#",
                "expected": "0",
                "description": "Slew to target coordinates"
            },
            
            # Test Commands
            "ACK Test": {
                "command": "\x06",
                "expected": "G",
                "description": "Send ACK character (0x06)"
            }
        }
        
        # Setup GUI
        self.setup_gui()
    
    def setup_gui(self):
        # Connection Frame
        conn_frame = ttk.Frame(self.root)
        conn_frame.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Label(conn_frame, text="COM Port:").pack(side=tk.LEFT)
        self.port_var = tk.StringVar(value="COM11")
        ttk.Entry(conn_frame, textvariable=self.port_var, width=10).pack(side=tk.LEFT, padx=5)
        
        ttk.Label(conn_frame, text="Baud Rate:").pack(side=tk.LEFT, padx=(10,0))
        self.baud_var = tk.StringVar(value="115200")
        ttk.Entry(conn_frame, textvariable=self.baud_var, width=10).pack(side=tk.LEFT, padx=5)
        
        self.connect_btn = ttk.Button(conn_frame, text="Connect", command=self.toggle_connection)
        self.connect_btn.pack(side=tk.LEFT, padx=10)
        
        self.status_label = ttk.Label(conn_frame, text="Disconnected", foreground="red")
        self.status_label.pack(side=tk.LEFT, padx=10)
        
        # Test Automation Frame
        auto_frame = ttk.Frame(self.root)
        auto_frame.pack(fill=tk.X, padx=10, pady=5)
        
        self.run_all_btn = ttk.Button(auto_frame, text="Run All Tests", command=self.run_all_tests)
        self.run_all_btn.pack(side=tk.LEFT, padx=5)
        
        self.stop_tests_btn = ttk.Button(auto_frame, text="Stop Tests", command=self.stop_all_tests, state=tk.DISABLED)
        self.stop_tests_btn.pack(side=tk.LEFT, padx=5)
        
        self.progress_var = tk.StringVar(value="Ready")
        self.progress_label = ttk.Label(auto_frame, textvariable=self.progress_var)
        self.progress_label.pack(side=tk.LEFT, padx=10)
        
        # Progress bar
        self.progress_bar = ttk.Progressbar(auto_frame, mode='determinate', length=200)
        self.progress_bar.pack(side=tk.LEFT, padx=10)
        
        # Commands Frame
        cmd_frame = ttk.LabelFrame(self.root, text="LX200 Commands")
        cmd_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # Create scrollable frame for commands
        canvas = tk.Canvas(cmd_frame)
        scrollbar = ttk.Scrollbar(cmd_frame, orient="vertical", command=canvas.yview)
        scrollable_frame = ttk.Frame(canvas)
        
        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        # Create command buttons in grid
        row = 0
        col = 0
        for cmd_name, cmd_info in self.lx200_commands.items():
            btn = ttk.Button(
                scrollable_frame,
                text=cmd_name,
                command=lambda c=cmd_info: self.send_command(c),
                width=20
            )
            btn.grid(row=row, column=col, padx=2, pady=2, sticky="ew")
            
            col += 1
            if col >= 3:  # 3 columns
                col = 0
                row += 1
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        
        # Custom Command Frame
        custom_frame = ttk.LabelFrame(self.root, text="Custom Command")
        custom_frame.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Label(custom_frame, text="Command:").pack(side=tk.LEFT)
        self.custom_cmd_var = tk.StringVar()
        ttk.Entry(custom_frame, textvariable=self.custom_cmd_var, width=30).pack(side=tk.LEFT, padx=5)
        ttk.Button(custom_frame, text="Send", command=self.send_custom_command).pack(side=tk.LEFT, padx=5)
        
        # Log Frame
        log_frame = ttk.LabelFrame(self.root, text="Communication Log")
        log_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        self.log_text = scrolledtext.ScrolledText(log_frame, height=15)
        self.log_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Clear log button
        ttk.Button(log_frame, text="Clear Log", command=self.clear_log).pack(pady=5)
    
    def toggle_connection(self):
        if not self.is_connected:
            self.connect_serial()
        else:
            self.disconnect_serial()
    
    def connect_serial(self):
        try:
            self.serial_connection = serial.Serial(
                port=self.port_var.get(),
                baudrate=int(self.baud_var.get()),
                timeout=2
            )
            self.is_connected = True
            self.connect_btn.config(text="Disconnect")
            self.status_label.config(text="Connected", foreground="green")
            self.log_message(f"Connected to {self.port_var.get()} at {self.baud_var.get()} baud")
            
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect: {e}")
            self.log_message(f"Connection failed: {e}")
    
    def disconnect_serial(self):
        if self.serial_connection:
            self.serial_connection.close()
            self.serial_connection = None
        
        self.is_connected = False
        self.connect_btn.config(text="Connect")
        self.status_label.config(text="Disconnected", foreground="red")
        self.log_message("Disconnected")
    
    def send_command(self, cmd_info):
        if not self.is_connected:
            messagebox.showwarning("Not Connected", "Please connect to a COM port first")
            return
        
        command = cmd_info["command"]
        expected = cmd_info["expected"]
        description = cmd_info["description"]
        
        try:
            # Send command
            if command == "\x06":  # ACK command
                self.serial_connection.write(b'\x06')
                self.log_message(f"SENT: ACK (0x06) - {description}")
            else:
                self.serial_connection.write(command.encode('utf-8'))
                self.log_message(f"SENT: {command} - {description}")
            
            # Read response (with timeout)
            response = ""
            start_time = time.time()
            
            while time.time() - start_time < 2.0:  # 2 second timeout
                if self.serial_connection.in_waiting > 0:
                    data = self.serial_connection.read(self.serial_connection.in_waiting)
                    response += data.decode('utf-8', errors='ignore')
                    
                    # Check if we have a complete response (ends with #)
                    if '#' in response or len(response) > 50:
                        break
                time.sleep(0.1)
            
            # Log response
            if response:
                self.log_message(f"RECV: {repr(response)}")
                
                # Check if response matches expected
                if expected:
                    if expected in response:
                        self.log_message("✓ Response matches expected format", "green")
                    else:
                        self.log_message(f"✗ Expected: {expected}, Got: {repr(response)}", "red")
                else:
                    self.log_message("✓ Command sent (no response expected)", "blue")
            else:
                if expected:
                    self.log_message("✗ No response received (timeout)", "red")
                else:
                    self.log_message("✓ Command sent (no response expected)", "blue")
                    
        except Exception as e:
            self.log_message(f"Error sending command: {e}", "red")
    
    def send_custom_command(self):
        if not self.is_connected:
            messagebox.showwarning("Not Connected", "Please connect to a COM port first")
            return
        
        command = self.custom_cmd_var.get().strip()
        if not command:
            return
        
        custom_cmd_info = {
            "command": command,
            "expected": "",
            "description": "Custom command"
        }
        
        self.send_command(custom_cmd_info)
        self.custom_cmd_var.set("")  # Clear input
    
    def log_message(self, message, color="black"):
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        log_entry = f"[{timestamp}] {message}\n"
        
        # Get position before inserting
        start_index = self.log_text.index(tk.INSERT)
        
        # Insert the text
        self.log_text.insert(tk.END, log_entry)
        
        # Apply color tag if not black
        if color != "black":
            # Create unique tag name for each colored line
            import uuid
            tag_name = f"{color}_{str(uuid.uuid4())[:8]}"
            
            # Get the exact position of the inserted text (excluding newline)
            end_index = self.log_text.index(tk.END)
            start_pos = f"{end_index} -{len(log_entry)}c"
            end_pos = f"{end_index} -1c"
            
            self.log_text.tag_add(tag_name, start_pos, end_pos)
            self.log_text.tag_config(tag_name, foreground=color)
        
        self.log_text.see(tk.END)
        self.root.update()
    
    def clear_log(self):
        self.log_text.delete(1.0, tk.END)
    
    def run_all_tests(self):
        """Run all LX200 commands sequentially and generate test report"""
        if not self.is_connected:
            messagebox.showwarning("Not Connected", "Please connect to a COM port first")
            return
        
        self.is_running_all = True
        self.test_results = {}
        
        # Update UI state
        self.run_all_btn.config(state=tk.DISABLED)
        self.stop_tests_btn.config(state=tk.NORMAL)
        
        # Clear previous results
        self.log_message("=" * 60)
        self.log_message("STARTING AUTOMATED TEST SEQUENCE", "blue")
        self.log_message("=" * 60)
        
        # Get list of commands
        commands = list(self.lx200_commands.items())
        total_commands = len(commands)
        
        # Setup progress bar
        self.progress_bar['maximum'] = total_commands
        self.progress_bar['value'] = 0
        
        # Run tests in a separate thread to keep GUI responsive
        threading.Thread(target=self._run_tests_thread, args=(commands,), daemon=True).start()
    
    def _run_tests_thread(self, commands):
        """Thread function to run all tests"""
        passed = 0
        failed = 0
        
        for i, (cmd_name, cmd_info) in enumerate(commands):
            if not self.is_running_all:  # Check if user stopped tests
                break
                
            # Update progress
            self.root.after(0, self._update_progress, i + 1, len(commands), cmd_name)
            
            # Run test
            success = self._run_single_test(cmd_name, cmd_info)
            
            if success:
                passed += 1
            else:
                failed += 1
            
            self.test_results[cmd_name] = success
            
            # Wait between commands
            time.sleep(1.0)  # 1 second delay between commands
        
        # Generate final report
        self.root.after(0, self._generate_test_report, passed, failed, len(commands))
    
    def _run_single_test(self, cmd_name, cmd_info):
        """Run a single test and return success status"""
        command = cmd_info["command"]
        expected = cmd_info["expected"]
        description = cmd_info["description"]
        
        try:
            self.root.after(0, self.log_message, f"Testing: {cmd_name}")
            
            # Send command
            if command == "\x06":  # ACK command
                self.serial_connection.write(b'\x06')
                self.root.after(0, self.log_message, f"SENT: ACK (0x06) - {description}")
            else:
                self.serial_connection.write(command.encode('utf-8'))
                self.root.after(0, self.log_message, f"SENT: {command} - {description}")
            
            # Read response (with timeout)
            response = ""
            start_time = time.time()
            
            while time.time() - start_time < 3.0:  # 3 second timeout for automated tests
                if self.serial_connection.in_waiting > 0:
                    data = self.serial_connection.read(self.serial_connection.in_waiting)
                    response += data.decode('utf-8', errors='ignore')
                    
                    # Check if we have a complete response
                    if '#' in response or len(response) > 50:
                        break
                time.sleep(0.1)
            
            # Log response
            if response:
                self.root.after(0, self.log_message, f"RECV: {repr(response)}")
                
                # Check if response matches expected
                if expected:
                    if expected in response or self._response_matches_pattern(response, expected):
                        self.root.after(0, self.log_message, "✓ PASS - Response matches expected format", "green")
                        return True
                    else:
                        self.root.after(0, self.log_message, f"✗ FAIL - Expected: {expected}, Got: {repr(response)}", "red")
                        return False
                else:
                    self.root.after(0, self.log_message, "✓ PASS - Command sent (no response expected)", "blue")
                    return True
            else:
                if expected:
                    self.root.after(0, self.log_message, "✗ FAIL - No response received (timeout)", "red")
                    return False
                else:
                    self.root.after(0, self.log_message, "✓ PASS - Command sent (no response expected)", "blue")
                    return True
                    
        except Exception as e:
            self.root.after(0, self.log_message, f"✗ FAIL - Error: {e}", "red")
            return False
    
    def _response_matches_pattern(self, response, expected):
        """Check if response matches expected pattern with wildcards"""
        if expected == "HH:MM:SS#":
            # Check for time format like "12:34:56#"
            import re
            return bool(re.match(r'\d{1,2}:\d{2}:\d{2}#', response))
        elif expected.startswith("sDD*MM:SS#") or expected.startswith("sDDD*MM:SS#"):
            # Check for coordinate format like "+45*30:45#"
            import re
            return bool(re.match(r'[+-]\d{2,3}\*\d{2}:\d{2}#', response))
        elif expected == "Site Name#":
            # Any text ending with #
            return response.endswith('#')
        return False
    
    def _update_progress(self, current, total, cmd_name):
        """Update progress bar and status"""
        self.progress_bar['value'] = current
        self.progress_var.set(f"Testing {current}/{total}: {cmd_name[:30]}...")
        
    def _generate_test_report(self, passed, failed, total):
        """Generate final test report"""
        self.log_message("=" * 60)
        self.log_message("TEST SEQUENCE COMPLETED", "blue")
        self.log_message("=" * 60)
        
        # Summary
        self.log_message(f"Total Commands: {total}")
        self.log_message(f"Passed: {passed}", "green")
        self.log_message(f"Failed: {failed}", "red" if failed > 0 else "black")
        success_rate = (passed / total) * 100 if total > 0 else 0
        self.log_message(f"Success Rate: {success_rate:.1f}%", "green" if success_rate >= 80 else "red")
        
        # Detailed results
        self.log_message("\nDETAILED RESULTS:")
        self.log_message("-" * 40)
        
        for cmd_name, success in self.test_results.items():
            status = "PASS" if success else "FAIL"
            color = "green" if success else "red"
            self.log_message(f"{status:4} | {cmd_name}", color)
        
        self.log_message("=" * 60)
        
        # Reset UI
        self.is_running_all = False
        self.run_all_btn.config(state=tk.NORMAL)
        self.stop_tests_btn.config(state=tk.DISABLED)
        self.progress_var.set("Completed")
        
        # Show summary popup
        messagebox.showinfo(
            "Test Results", 
            f"Test sequence completed!\n\n"
            f"Total: {total} commands\n"
            f"Passed: {passed}\n"
            f"Failed: {failed}\n"
            f"Success Rate: {success_rate:.1f}%"
        )
    
    def stop_all_tests(self):
        """Stop the automated test sequence"""
        self.is_running_all = False
        self.run_all_btn.config(state=tk.NORMAL)
        self.stop_tests_btn.config(state=tk.DISABLED)
        self.progress_var.set("Stopped")
        self.log_message("Test sequence stopped by user", "red")

def main():
    root = tk.Tk()
    app = LX200TestClient(root)
    root.mainloop()

if __name__ == "__main__":
    main()